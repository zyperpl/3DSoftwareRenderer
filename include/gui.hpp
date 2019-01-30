#pragma once

#include <string>

#include "imgui.h"
#include "imgui_impl_zdlib.h"

#define GUI_WIDTH 340.f

struct zPixel;
class Camera;

class GUI
{
  public:
    GUI(float width, float height) : width{width}, height{height} {};

    void update(zPixel &lineColor, Camera **cameraPtr, bool &showGrid, 
        float &gridGap, bool &showModel, bool &showMarkers, bool &relativeToOrigin, 
        bool &showProjectionShape, bool &perspectiveFullscreen, float &orthoScale);
    void openFilePicker();
    std::string getFilePickerString();

    enum
    {
      SceneOpen, SceneSave, Normal, STATE_MAX
    } state;

    float width, height;
    bool running{true};

  private:
    bool isFilePickerActive{false};
};
