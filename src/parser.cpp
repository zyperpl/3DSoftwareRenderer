#include "parser.hpp"
#include "model.hpp"
#include "camera.hpp"

#include <exception>
#include <cstdlib>
#include <cstdio>

std::tuple<Model*, Camera*, int> Parser::readScene(std::string fileName)

{
  std::ifstream file(fileName);

  if (!file.good())
  {
    fprintf(stderr, "File %s cannot be opened!\n", fileName.c_str());
    return { 0, 0, 0 };
  }

  /*
   * Reads number num of 3D vectors of type specific to container.
   */
  auto loadVectors3 = [&file](auto &container, int num)
  {
    auto c = container;
    using T = typename decltype(c)::value_type;

    std::string l;
    while (std::getline(file,l))
    {
      std::stringstream iss(l);
      //static int i = 0; printf("%d:%s\n", i++, l.c_str());
      T t;
      if (iss >> t[0] >> t[1] >> t[2])
      {
        container.push_back(t);
      } else
      {
        break;
      }

      if (container.size() == num) break;
    }
  };

  /*
   * Reads until TOKEN  DATA
   * and returns DATA.
   */
  auto readUntil1 = [&file](std::string token, auto &data)
  {
    std::string l;
    while (std::getline(file, l))
    {
      std::stringstream iss(l);
      std::string t;
      if (iss >> t >> data)
      {
        if (t == token) return data;
      }
    }
    fprintf(stderr, "Error: Token '%s' cannot be reached!\n", token.c_str());
    throw new std::runtime_error("Token cannot be found");
  };

  /*
   * Reads until TOKEN  DATA0 DATA1
   * and returns DATA0.
   */
  auto readUntil2 = [&file](std::string token, 
        auto &data0, auto &data1)
  {
    std::string l;
    while (std::getline(file, l))
    {
      std::stringstream iss(l);
      std::string t;
      if (iss >> t >> data0 >> data1)
      {
        if (t == token) return data0;
      }
    }
    fprintf(stderr, "Error: Token '%s' cannot be reached!\n", token.c_str());
    throw new std::runtime_error("Token cannot be found");
  };

  /*
   * Reads until TOKEN  DATA0 DATA1 DATA2
   * and returns DATA0.
   */
  auto readUntil3 = [&file](std::string token, 
        auto &data0, auto &data1, auto &data2)
  {
    std::string l;
    while (std::getline(file, l))
    {
      std::stringstream iss(l);
      std::string t;
      if (iss >> t >> data0 >> data1)
      {
        if (t == token) return data0;
      }
    }
    fprintf(stderr, "Error: Token '%s' cannot be reached!\n", token.c_str());
    throw new std::runtime_error("Token cannot be found");
  };


  Model *model = nullptr;
  Camera *camera = nullptr;

  model = new Model();
  
  int pointsCount, trianglesCount;
  readUntil1("points_count", pointsCount);
  loadVectors3(model->vertices, pointsCount);
  readUntil1("triangles_count", trianglesCount);
  loadVectors3(model->faces, trianglesCount);
  
  printf("File Points: %d\t Vertices: %zu\n", pointsCount, model->vertices.size());
  printf("File Triangles: %d\t Faces: %zu\n", trianglesCount, model->faces.size());

  // reads only one camera
  std::string cameraName;
  readUntil1("cam_name", cameraName);
  printf("Camera name: %s\n", cameraName.c_str());

  camera = new Camera();
  camera->name = cameraName;
  readUntil2("resolution", camera->resolution.x, camera->resolution.y);
  printf("Camera resolution: %d %d\n", camera->resolution.x, camera->resolution.y);
  readUntil3("pos", camera->eye.x, camera->eye.y, camera->eye.z);
  readUntil3("lookAt", camera->origin.x, camera->origin.y, camera->origin.z);
  readUntil1("fov", camera->angle);
  //readUntil1("rotation", camera->rotation);
  //
  
  file.close();

  return { model, camera, model->vertices.size() };
}


int Parser::saveScene(std::string fileName, Model *mdl, Camera *cam)
{
  std::ofstream file(fileName);
  if (!file.good())
  {
    fprintf(stderr, "Cannot create file %s!\n", fileName.c_str());
    return 0;
  }

  file << "points_count " << mdl->vertices.size() << std::endl;
  for (const auto &v : mdl->vertices)
  {
    file << v.x << ' ' << v.y << ' ' << v.z << std::endl;
  }

  file << "triangles_count " << mdl->faces.size() << std::endl;
  for (const auto &f : mdl->faces)
  {
    file << f.vid1 << ' ' << f.vid2 << ' ' << f.vid3 << std::endl;
  }

  file << std::endl;

  file << "cam_name " << cam->name << std::endl;
  file << "resolution " << cam->resolution.x << ' ' << cam->resolution.y << std::endl;
  file << "pos " << cam->eye.x << ' ' << cam->eye.y << ' ' << cam->eye.z << std::endl;
  file << "lookAt " << cam->origin.x << ' ' << cam->origin.y << ' ' << cam->origin.z << std::endl;
  file << "fov " << cam->angle << std::endl;
  //file << "rotation" << cam->rotation << std::endl;
  
  return file.tellp();
}
