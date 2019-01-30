#pragma once

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

class Model;
class Camera;

class Parser
{
  public:
    static std::tuple<Model*, Camera*, int> readScene(std::string fileName);
    static int saveScene(std::string fileName, Model *mdl, Camera *cam);

};
