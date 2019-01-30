#pragma once

#include <cstdio>
#include <string>
#include <optional>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

class Camera
{
  public:
    std::string name;
    glm::vec<2,int> resolution;
    glm::vec3 eye;
    glm::vec3 origin;
    float angle;
    float near{0.1}, far{100};

    std::optional<glm::vec2> project2D(glm::mat4 mvp, glm::vec3 point, glm::vec2 viewSize);
    glm::mat4 getMVP(glm::vec2 viewSize);
    glm::mat4 getAspectMatrix(glm::vec2 viewSize);
};
