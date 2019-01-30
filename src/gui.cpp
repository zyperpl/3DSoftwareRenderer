#include "gui.hpp"

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "zdlib.h"
#include "camera.hpp"

void GUI::update(zPixel &lineColor, Camera **cameraPtr, bool &showGrid, float &gridGap, bool &showModel, bool &showMarkers, bool &relativeToOrigin, bool &showProjectionShape, bool &perspectiveFullscreen, float &orthoScale)
{
  ImGui_ImplZDLib_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowSize({width, height});
  ImGui::SetNextWindowPos({0,0});
  
  Camera *camera = *cameraPtr;

  if (ImGui::Begin("main", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar))
  {
    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
          if (ImGui::MenuItem("Open..", "Ctrl+O")) { state = GUI::SceneOpen; openFilePicker(); }
          if (ImGui::MenuItem("Save", "Ctrl+S"))   { state = GUI::SceneSave; openFilePicker(); }
          if (ImGui::MenuItem("Close"))  { running = false; }
          ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    ImGui::SetNextWindowPos({16,48});

    if (ImGui::BeginChild("Info") && camera)
    {
      static float fLineColor[3] = { 1.0, 1.0, 0.9 };

      ImGui::ColorEdit3("Color", fLineColor);
      lineColor.r = fLineColor[0]*255;
      lineColor.g = fLineColor[1]*255;
      lineColor.b = fLineColor[2]*255;
      ImGui::SliderFloat("Field of view", &camera->angle, 1.0f, 180.0f);
      
      ImGui::NewLine();
      ImGui::Text("Distance to origin: %6.2f\n", glm::distance(camera->eye, camera->origin));
      ImGui::DragFloat3("Camera position", &camera->eye[0], 0.1f );
      ImGui::DragFloat3("Origin position", &camera->origin[0], 0.1f);
      
      ImGui::Checkbox("Enable grid rendering", &showGrid);
      ImGui::SliderFloat("Grid gap", &gridGap, 0.0f, 30.f);
      ImGui::Checkbox("Show model", &showModel);
      ImGui::Checkbox("Show markers", &showMarkers);
      ImGui::Checkbox("Camera relative to origin", &relativeToOrigin);
      ImGui::Checkbox("Show projection shape", &showProjectionShape);
      ImGui::Checkbox("Perspective on fullscreen", &perspectiveFullscreen);
      ImGui::SliderFloat("Ortho scale", &orthoScale, 0.0f, 30.f);
      ImGui::NewLine();

    } 
    ImGui::EndChild();
  } 
  ImGui::End();


  if (isFilePickerActive)
  {
    ImGui::SetNextWindowSize({width*2, height-100});
    if (ImGuiFileDialog::Instance()->FileDialog("Choose scene file",".scn\0.obj\0\0", "./data", ""))
    {
      this->isFilePickerActive = false;
    }
  }
}

void GUI::openFilePicker() { this->isFilePickerActive = true; }


std::string GUI::getFilePickerString() 
{
  if (ImGuiFileDialog::Instance()->IsOk == true)
  {
    return ImGuiFileDialog::Instance()->GetFilepathName();
  }
  return "";
}


