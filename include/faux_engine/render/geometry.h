#ifndef __NX_GEOMETRY_H__
#define __NX_GEOMETRY_H__

#include <vector>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "faux_engine/types.h"


namespace nx {
  class Geometry {
  public:
    Geometry();
    Geometry(const Geometry&) = delete;
    Geometry(Geometry&& other) noexcept;
    Geometry& operator=(const Geometry& other) = delete;
    Geometry& operator=(Geometry&& other) noexcept;
    ~Geometry();

    nx::Result loadFromObj(std::string path);
    nx::Result optimize();
    nx::Result computeTangents();
    nx::Result clear();
    nx::Result upload();
    nx::Result draw();

    u32 version_ = 0;
    u32 gpu_version_ = 0;

    struct Shape {
      std::string name_;

      std::vector<glm::vec3> vertices_;
      std::vector<glm::vec3> normals_;
      std::vector<glm::vec2> texcoords_;
      std::vector<glm::vec3> tangents_;
      std::vector<glm::vec3> bitangents_;
      std::vector<u32> indices_;

      u32 gpu_attributes_handle_ = -1;
      u32 gpu_indices_handle_ = -1;
      u32 gpu_vao_handle_ = -1;
      
      u32 material_handle_ = -1;
    };
    std::vector<Shape> shapes_;

  };
}

#endif