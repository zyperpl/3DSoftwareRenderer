#include "model.hpp"
#include "camera.hpp"
#include "parser.hpp"

#include "zdlib.h"
#include "gui.hpp"

#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define WIDTH  1900
#define HEIGHT 1000

#define DOTS_N 50

#define CORRECTIONS_N 10000

#define MOUSE_MARKER_DISTANCE 15
#define M_SIZE 8

#include "imgui.h"
#include "imgui_impl_zdlib.h"
#include "ImGuiFileDialog.h"

Model  *model  = nullptr;
Camera *camera = nullptr;

bool loadScene(const char *name)
{
  delete model;
  delete camera;

  printf("Loading \"%s\" scene...\n", name);
  int read;
  std::tie(model, camera, read) = Parser::readScene(name);

  return read > 0;
}

bool saveScene(const char *name)
{
  printf("Saving scene to %s file...\n", name);

  return Parser::saveScene(name, model, camera) > 0;
}

int main(int argc, char *argv[]) 
{
  if (argc > 1)
  {
    if (!loadScene(argv[1])) 
    {
      fprintf(stderr, "Cannot read model %s!\n", argv[1]);
      return 1;
    }
  }

  zPixel lineColor;
  bool showGrid = false;
  bool showModel = true;
  bool showMarkers = true;
  bool showProjectionShape = true;
  bool relativeToOrigin = true;
  bool perspectiveFullscreen = false;
  float gridGap = 0.5;
  float orthoScale = 1.0;
  
  zCreateWindow(WIDTH, HEIGHT, "Models renderer");  
  ImGui_ImplZDLib_Init(WIDTH, HEIGHT);
  ImGuiIO &io = ImGui::GetIO();

  GUI *gui = new GUI(GUI_WIDTH, HEIGHT);

  static glm::vec3 mousePoint(0,0,0);
  static glm::vec3 nearPlanePoints[4];
  
  // initialize variables
  const int WIN_N = 4;
  glm::vec2 windowsSizes[WIN_N] = { 
    { WIDTH/2-GUI_WIDTH/2, HEIGHT/2 }, { WIDTH/2-GUI_WIDTH/2, HEIGHT/2 }, 
    { WIDTH/2-GUI_WIDTH/2, HEIGHT/2 }, { WIDTH/2-GUI_WIDTH/2, HEIGHT/2 } 
  };

  glm::vec2 windowsPositions[WIN_N] = {
    { GUI_WIDTH, 0 }, { GUI_WIDTH+windowsSizes[0].x, 0 }, { GUI_WIDTH, windowsSizes[1].y }, { GUI_WIDTH+windowsSizes[0].x, windowsSizes[1].y }
  };
    
  static enum { MovingOrigin, MovingEye, Normal } mouseState = Normal;

  const zPixel cursorColor  = { 255,0,0 };
  const zPixel eyeColor     = { 0,0,255 };
  const zPixel originColor  = { 0,255,255 };

  // debuging variable
  static float plz = 0.8;

  while (zIsOpen())
  {
    static long iteration = -1;
    iteration++;

    zUpdate();

    /*
     *  camera/origin movement
     */
    float step = 0.1;

    if (zKey(GLFW_KEY_LEFT_SHIFT))   step *= 10;
    if (zKey(GLFW_KEY_LEFT_CONTROL)) step *= 10;

    if (camera)
    {
      if (!relativeToOrigin)
      {
        if (zKey(GLFW_KEY_A)) camera->eye.x -= step;
        if (zKey(GLFW_KEY_D)) camera->eye.x += step;
        if (zKey(GLFW_KEY_S)) camera->eye.y -= step;
        if (zKey(GLFW_KEY_W)) camera->eye.y += step;

        if (zKey(GLFW_KEY_E)) camera->eye.z += step;
        if (zKey(GLFW_KEY_C)) camera->eye.z -= step;
        
        if (zKey(GLFW_KEY_J)) camera->origin.x -= step;
        if (zKey(GLFW_KEY_L)) camera->origin.x += step;
        if (zKey(GLFW_KEY_K)) camera->origin.y -= step;
        if (zKey(GLFW_KEY_I)) camera->origin.y += step;
        
        if (zKey(GLFW_KEY_O))      camera->origin.z += step;
        if (zKey(GLFW_KEY_PERIOD)) camera->origin.z -= step;

        if (zKey(GLFW_KEY_EQUAL)) camera->angle += step;
        if (zKey(GLFW_KEY_MINUS)) camera->angle -= step;
      } else
      {
        auto forward = glm::normalize(camera->eye - camera->origin);
        auto right = glm::cross(glm::vec3(0,1,0), forward);
        auto up = glm::cross(right, forward);

        forward *= step; right *= step; up *= step;

        if (zKey(GLFW_KEY_A)) camera->eye -= forward;
        if (zKey(GLFW_KEY_D)) camera->eye += forward;
        if (zKey(GLFW_KEY_S)) camera->eye += up;
        if (zKey(GLFW_KEY_W)) camera->eye -= up;

        if (zKey(GLFW_KEY_E)) camera->eye += right;
        if (zKey(GLFW_KEY_C)) camera->eye -= right;
        
        if (zKey(GLFW_KEY_O)) camera->origin -= forward;
        if (zKey(GLFW_KEY_PERIOD)) camera->origin += forward;
        if (zKey(GLFW_KEY_K)) camera->origin += up;
        if (zKey(GLFW_KEY_I)) camera->origin -= up;
        
        if (zKey(GLFW_KEY_J)) camera->origin += right;
        if (zKey(GLFW_KEY_L)) camera->origin -= right;

      }
        
      if (zKey(GLFW_KEY_EQUAL)) camera->angle += step;
      if (zKey(GLFW_KEY_MINUS)) camera->angle -= step;
    }


    zClear();
      

    /*
     *  GUI update
     */
    gui->update(lineColor, &camera, showGrid, 
        gridGap, showModel, showMarkers, relativeToOrigin, 
        showProjectionShape, perspectiveFullscreen, orthoScale);

    if (!gui->running) zClose();

    auto fileString = gui->getFilePickerString();
    if (fileString != "")
    {
      /**/ if (gui->state == GUI::SceneOpen) loadScene(fileString.c_str());
      else if (gui->state == GUI::SceneSave) saveScene(fileString.c_str());

      gui->state = GUI::Normal;
    }

    ImGui::Render();
    ImGui_ImplZDLib_Render();

    
    // helper functions
    auto insideWindow = [](glm::vec2 p, glm::vec2 windowPosition, glm::vec2 windowSize)->bool
    {
      //printf("%f %f < %f %f\n", p.x, p.y, windowSize.x, windowSize.y);

      if (p.x < 0 || p.y < 0) return false;
      if (p.x >= windowSize.x) return false;
      if (p.y >= windowSize.y) return false;      
      return true;
    };

    auto drawLineInWindow = [insideWindow](glm::vec2 p1, glm::vec2 p2, 
        glm::vec2 windowPosition, glm::vec2 windowSize, zPixel color)
    {
      bool p1Inside = insideWindow(p1, windowPosition, windowSize);
      bool p2Inside = insideWindow(p2, windowPosition, windowSize);

      if (!p1Inside && !p2Inside) return;

      //*
      auto vec = glm::normalize(p2-p1);
      int corrections = CORRECTIONS_N;
      while (!insideWindow(p1, windowPosition, windowSize))
      {
        p1 += vec;
        if (corrections-- < 0) return;
      }
      
      while (!insideWindow(p2, windowPosition, windowSize))
      {
        p2 -= vec;
        if (corrections-- < 0) return;
      }
      // */

      p1 += windowPosition;
      p2 += windowPosition;
      zDrawLine(p1.x, p1.y, p2.x, p2.y, color);
    };
    
    auto drawPixelInWindow = [insideWindow](glm::vec2 p, 
        glm::vec2 windowPosition, glm::vec2 windowSize, zPixel color)
    {
      if (insideWindow(p, windowPosition, windowSize))
      {
        p += windowPosition;
        zDrawPixel(p.x, p.y, color);
      }
    };


    /*
     *  objects drawing
     */
    if (camera && model)
    {


      if (zKey(GLFW_KEY_M)) plz += 0.1;
      if (zKey(GLFW_KEY_N)) plz -= 0.1;
        
      static auto lastCameraOrigin = camera->origin;
      if (mouseState != MovingOrigin) lastCameraOrigin = camera->origin;

      static auto lastCameraEye = camera->eye;
      if (mouseState != MovingEye) lastCameraEye = camera->eye;

      auto distance = glm::distance(lastCameraOrigin, lastCameraEye);

      // iterate on each window
      for (int w = 0; w < WIN_N; w++)
      {
        auto wSize = windowsSizes[w];
        auto wPosition = windowsPositions[w];

        // change window size if fullscreen
        if (perspectiveFullscreen)
        {
          if (w > 0) break;

          wSize = {WIDTH,HEIGHT};
          wPosition = {0,0};
        }
      
        auto mousePosition = zGetMousePosition();

        //printf("w%d#: %.2fx%.2f %.2f;%.2f\n", w, wSize.x, wSize.y, wPosition.x, wPosition.y);

        glm::mat4 matrix(1.f);

        if (w == 0)
        {
          matrix = camera->getMVP(wSize);

          auto forward = glm::vec4(glm::normalize(camera->eye - camera->origin), 1);
          auto screenPlanePoint = [matrix](float x, float y, float dst) -> glm::vec4
          {
            auto t = glm::inverse(matrix) * glm::vec4(x, y, dst, 1.0f);
            t /= t.w;
            return t;
          };

          float dst = 0.9;

          nearPlanePoints[0] = screenPlanePoint(-1.f, -1.f, dst);
          nearPlanePoints[1] = screenPlanePoint( 1.f, -1.f, dst);
          nearPlanePoints[2] = screenPlanePoint( 1.f,  1.f, dst);
          nearPlanePoints[3] = screenPlanePoint(-1.f,  1.f, dst);
        } else
        if (w > 0)
        {
          matrix = camera->getAspectMatrix(wSize);
          matrix = glm::scale(matrix, glm::vec3(orthoScale/distance, orthoScale/distance, orthoScale/distance));

          // rotate projection for different windows
          if (w == 2)
            matrix = glm::rotate(matrix, glm::radians(90.f), glm::vec3(0,1,0));
          else if (w == 3)
            matrix = glm::rotate(matrix, glm::radians(90.f), glm::vec3(1,0,0));
          


          matrix = glm::translate(matrix, -lastCameraOrigin);
        }
        
        // change mouse position relative to window
        glm::vec2 wMousePos(mousePosition.x-wPosition.x, mousePosition.y-wPosition.y);
        if (insideWindow(wMousePos, wPosition, wSize))
        {
          wMousePos.x = wMousePos.x/wSize.x*2.f - 1.0f;
          wMousePos.y = wMousePos.y/wSize.y*2.f - 1.0f;

          float dst = plz;
          if (w > 0) dst = 0;

          glm::vec4 mp = glm::inverse(matrix)*glm::vec4(-wMousePos.x, -wMousePos.y, dst, 1);
          mp /= mp.w;
          
          //printf("Mouse point position w%d#: %f %f %f %f\n", w, mp.x, mp.y, mp.z, mp.w);
          
          mousePoint = mp;
        }

        if (!io.WantCaptureMouse && zMouseButton(ZMOUSE_BUTTON_LEFT))
        {
          printf("%f %f %f\n", mousePoint.x, mousePoint.y, mousePoint.z);
          if (w > 0 && false)
          {
            if (mouseState == MovingEye)
            {
              if (w == 2) mousePoint.z = camera->eye.z;
              if (w == 3) mousePoint.y = camera->eye.y;
              if (w == 1) mousePoint.x = camera->eye.x;
            }
            if (mouseState == MovingOrigin)
            {
              if (w == 2) mousePoint.x = camera->origin.x;
              if (w == 3) mousePoint.y = camera->origin.y;
              if (w == 1) mousePoint.z = camera->origin.z;
            }
          }

          if (mouseState == MovingEye)
          {
            camera->eye = mousePoint;
          }
          if (mouseState == MovingOrigin)
          {
            camera->origin = mousePoint;
          }
        }

        // draw projection cone
        if (showProjectionShape && w != 0)
        {
          if (auto np1 = camera->project2D(matrix, nearPlanePoints[0], wSize))
          if (auto np2 = camera->project2D(matrix, nearPlanePoints[1], wSize))
          if (auto np3 = camera->project2D(matrix, nearPlanePoints[2], wSize))
          if (auto np4 = camera->project2D(matrix, nearPlanePoints[3], wSize))
          if (auto e   = camera->project2D(matrix, camera->eye, wSize))
          {

            drawLineInWindow(e.value(), np1.value(), wPosition, wSize, {255,255,255});
            drawLineInWindow(e.value(), np2.value(), wPosition, wSize, {255,255,255});
            drawLineInWindow(e.value(), np3.value(), wPosition, wSize, {255,255,255});
            drawLineInWindow(e.value(), np4.value(), wPosition, wSize, {255,255,255});

            drawLineInWindow(np1.value(), np2.value(), wPosition, wSize, {255,255,255});
            drawLineInWindow(np2.value(), np3.value(), wPosition, wSize, {255,255,255});
            drawLineInWindow(np3.value(), np4.value(), wPosition, wSize, {255,255,255});
            drawLineInWindow(np1.value(), np4.value(), wPosition, wSize, {255,255,255});
          }
        }

        // draw grid
        if (showGrid)
        {
          // draw multiple grid levels
          for (int y = -1; y < 0; y++)
          {
            zPixel dotColor = { 
                    static_cast<uint8_t>(((sin(iteration/400.f)+1)/2.f)*255), 
                    static_cast<uint8_t>(((sin(iteration/100.f)+1)/2.f)*255), 
                    static_cast<uint8_t>(((sin(iteration/500.f)+1)/2.f)*255) 
                  };

            dotColor = dotColor/(abs(y)+1);

            #pragma omp parallel for
            for (int z = -DOTS_N; z < DOTS_N; z++)
            #pragma omp parallel for
            for (int x = -DOTS_N; x < DOTS_N; x++)
            {
              if (auto p1 = camera->project2D(matrix, {x*gridGap, y*gridGap/100.f, z*gridGap}, wSize))
              if (auto p2 = camera->project2D(matrix, {x*gridGap, y*gridGap/100.f, (z+1)*gridGap}, wSize))
              if (auto p3 = camera->project2D(matrix, {(x+1)*gridGap, y*gridGap/100.f, z*gridGap}, wSize))
              {
                drawLineInWindow(p1.value(), p2.value(), wPosition, wSize, dotColor);
                drawLineInWindow(p1.value(), p3.value(), wPosition, wSize, dotColor);
              }
            }
          }
        }


        // draw model
        if (showModel)
        {
          for (const auto &f : model->faces)
          {
            int i1 = f.vid1;
            int i2 = f.vid2;
            int i3 = f.vid3;
            
            auto v1 = model->vertices.at(i1).toGlmVec3();
            auto v2 = model->vertices.at(i2).toGlmVec3();
            auto v3 = model->vertices.at(i3).toGlmVec3();

            if (auto p1 = camera->project2D(matrix, v1, wSize))
            if (auto p2 = camera->project2D(matrix, v2, wSize))
            if (auto p3 = camera->project2D(matrix, v3, wSize))
            {
              drawLineInWindow(p1.value(), p2.value(), wPosition, wSize, lineColor);
              drawLineInWindow(p1.value(), p3.value(), wPosition, wSize, lineColor);
              drawLineInWindow(p3.value(), p2.value(), wPosition, wSize, lineColor);
            }
          }
        }

        // draw markers and move them (mouse, eye, origin)

        if (showMarkers)
        { 

          if (auto p1 = camera->project2D(matrix, mousePoint, wSize))
          if (auto p2 = camera->project2D(matrix, glm::vec3(mousePoint.x+.02,mousePoint.y,mousePoint.z+.02), wSize))
          if (auto p3 = camera->project2D(matrix, glm::vec3(mousePoint.x,mousePoint.y+.02,mousePoint.z), wSize))
          {
            drawLineInWindow(p1.value(), p2.value(), wPosition, wSize, cursorColor);
            drawLineInWindow(p1.value(), p3.value(), wPosition, wSize, cursorColor);
            drawLineInWindow(p2.value(), p3.value(), wPosition, wSize, cursorColor);
          }
          if (auto p = camera->project2D(matrix, camera->origin, wSize))
          {
            drawPixelInWindow(p.value(), wPosition, wSize, originColor);

            if (mouseState == Normal && !io.WantCaptureMouse)
            {
              auto sp = *p + wPosition;

              if (glm::distance(sp, {mousePosition.x,mousePosition.y}) < MOUSE_MARKER_DISTANCE)
              {
                drawLineInWindow({p->x-M_SIZE,p->y-M_SIZE},{p->x+M_SIZE,p->y-M_SIZE},wPosition,wSize,originColor);
                drawLineInWindow({p->x+M_SIZE,p->y-M_SIZE},{p->x+M_SIZE,p->y+M_SIZE},wPosition,wSize,originColor);
                drawLineInWindow({p->x+M_SIZE,p->y+M_SIZE},{p->x-M_SIZE,p->y+M_SIZE},wPosition,wSize,originColor);
                drawLineInWindow({p->x-M_SIZE,p->y+M_SIZE},{p->x-M_SIZE,p->y-M_SIZE},wPosition,wSize,originColor);

                if (zMouseButton(ZMOUSE_BUTTON_LEFT)) mouseState = MovingOrigin;
              }
            }
          }

          if (auto p = camera->project2D(matrix, camera->eye, wSize))
          {
            drawPixelInWindow(p.value(), wPosition, wSize, eyeColor);
            
            if (mouseState == Normal && !io.WantCaptureMouse)
            {
              auto sp = *p + wPosition;

              if (glm::distance(sp, {mousePosition.x,mousePosition.y}) < MOUSE_MARKER_DISTANCE)
              {
                drawLineInWindow({p->x-M_SIZE,p->y-M_SIZE},{p->x+M_SIZE,p->y-M_SIZE},wPosition,wSize,eyeColor);
                drawLineInWindow({p->x+M_SIZE,p->y-M_SIZE},{p->x+M_SIZE,p->y+M_SIZE},wPosition,wSize,eyeColor);
                drawLineInWindow({p->x+M_SIZE,p->y+M_SIZE},{p->x-M_SIZE,p->y+M_SIZE},wPosition,wSize,eyeColor);
                drawLineInWindow({p->x-M_SIZE,p->y+M_SIZE},{p->x-M_SIZE,p->y-M_SIZE},wPosition,wSize,eyeColor);
                
                if (zMouseButton(ZMOUSE_BUTTON_LEFT)) mouseState = MovingEye;
              }
            }
          }
        }

        if (!zMouseButton(ZMOUSE_BUTTON_LEFT)) mouseState = Normal;
      }


    }

    zRender();
  }

  zFree();
  ImGui_ImplZDLib_Shutdown();
}
