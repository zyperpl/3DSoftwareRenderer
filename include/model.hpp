#pragma once

#include <vector>

#include <glm/glm.hpp>

struct Vertex
{
  double x, y, z;
  Vertex(double xCoord, double yCoord, double zCoord) : x{xCoord}, y{yCoord}, z{zCoord} {};
  Vertex() : x{0}, y{0}, z{0} {};

  double &operator[](const size_t idx)
  {
    if (idx == 0) return x;
    if (idx == 1) return y;
    if (idx == 2) return z;
    return x;
  }

  glm::vec3 toGlmVec3() { return { x, y, z }; }

};

struct Face
{
  int vid1;
  int vid2;
  int vid3;

  int &operator[](const size_t idx)
  {
    if (idx == 0) return vid1;
    if (idx == 1) return vid2;
    if (idx == 2) return vid3;
    return vid1;
  }
};

class Model
{
  public:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};
