#include "camera.hpp"

#include "glm/glm.hpp"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"

std::optional<glm::vec2> Camera::project2D(glm::mat4 mvp, glm::vec3 point, glm::vec2 viewSize)
{ 
  //float dst = glm::distance(point,eye);
  //printf("%f %f %f (%f) ->", point.x, point.y, point.z, dst);
  auto np = mvp * glm::vec4(point,1.f);

  //printf("%f %f %f %f\n", np.x, np.y, np.z, np.w);

  if (np.w != 1)
  {
    if (np.z < 0) return {};
  }

  np /= -np.w;

  np.x = (np.x+1.f)*viewSize.x/2.f;
  np.y = (np.y+1.f)*viewSize.y/2.f;

  return np;
};

glm::mat4 Camera::getMVP(glm::vec2 viewSize)
{
  using namespace glm;

  double aspect = viewSize.x / viewSize.y;
  float ang     = tan(radians(angle / 2.f));

  // projection
  mat4 projection = mat4(1.0f);
  projection[0][0] = 1.f / (aspect * ang);
  projection[1][1] = 1.f / (ang);
  projection[2][2] = -(far+near)/(far-near);
  projection[2][3] = -1;
  projection[3][2] = -(2*far*near) / (far-near);

  mat4 view = mat4(1.0f);  
  // view (lookAt)
  auto forward = normalize(eye - origin);
  auto right   = cross(vec3(0,1,0), forward);
  auto up      = cross(forward, right);
  
  view[0][0] = right.x;
  view[0][1] = right.y;
  view[0][2] = right.z;
  view[0][3] = 0;

  view[1][0] = up.x;
  view[1][1] = up.y;
  view[1][2] = up.z;
  view[1][3] = 0;

  view[2][0] = forward.x;
  view[2][1] = forward.y;
  view[2][2] = forward.z;
  view[2][3] = 0;

  view[3][0] = eye.x;
  view[3][1] = eye.y;
  view[3][2] = eye.z;
  view[3][3] = 1;
  
  view = inverse(view);

  view = lookAt(eye, origin, vec3(0,1,0));
  
  // model
  mat4 model = mat4(1.f);

  return projection * view * model;
};

glm::mat4 Camera::getAspectMatrix(glm::vec2 viewSize)
{
  using namespace glm;

  double aspect = viewSize.x / viewSize.y;
  mat4 projection = mat4(1.0f);
  projection[0][0] = 1.f / aspect;
  projection[1][1] = 1.f;

  return projection;
}
