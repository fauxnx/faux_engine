#include "faux_engine/render/geometry.h"
#include "faux_engine/core.h"

#include "tinyobj/tiny_obj_loader.h"
#include <iostream>

nx::Geometry::Geometry() {}

nx::Geometry::Geometry(Geometry&& other) noexcept {

  version_ = other.version_;
  gpu_version_ = other.gpu_version_;

  std::swap(shapes_, other.shapes_);


  other.version_ = 0;
  other.gpu_version_ = 0;

  other.shapes_.clear();

}

nx::Geometry& nx::Geometry::operator=(Geometry&& other) noexcept {

  version_ = other.version_;
  gpu_version_ = other.gpu_version_;

  std::swap(shapes_, other.shapes_);


  other.version_ = 0;
  other.gpu_version_ = 0;

  other.shapes_.clear();


  return *this;
}

nx::Geometry::~Geometry() {}

struct vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;

  bool operator==(const vertex& other) const {
    return (pos == other.pos && normal == other.normal && uv == other.uv);
  }
};
namespace std {

  template <>
  struct hash<vertex> {
    std::size_t operator()(const vertex& k) const {
      using std::hash;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

      return ((hash<glm::vec3>()(k.pos)
        ^ (hash<glm::vec3>()(k.normal) << 1)) >> 1)
        ^ (hash<glm::vec2>()(k.uv) << 1);
    }
  };

}

nx::Result nx::Geometry::loadFromObj(std::string path) {
  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = ""; // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();
  auto& materials = reader.GetMaterials();

  for (auto& shape : shapes) {

    Shape s;
    s.name_ = shape.name;
    u32 index_offset = 0;
    u32 shape_material = -1;

    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      int fv = shape.mesh.num_face_vertices[f];
      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {

        tinyobj::index_t index = shape.mesh.indices[(u64)index_offset + v];

        float x = attrib.vertices[(u64)index.vertex_index * 3 + 0];
        float y = attrib.vertices[(u64)index.vertex_index * 3 + 1];
        float z = attrib.vertices[(u64)index.vertex_index * 3 + 2];

        s.vertices_.push_back(glm::vec3(x, y, z));

        float nx = attrib.normals[(u64)index.normal_index * 3 + 0];
        float ny = attrib.normals[(u64)index.normal_index * 3 + 1];
        float nz = attrib.normals[(u64)index.normal_index * 3 + 2];

        s.normals_.push_back(glm::vec3(nx, ny, nz));

        float tx = attrib.texcoords[(u64)index.texcoord_index * 2 + 0];
        float ty = attrib.texcoords[(u64)index.texcoord_index * 2 + 1];

        s.texcoords_.push_back(glm::vec2(tx, ty));
        // TODO: Check for texcoords and normals

      }
      index_offset += fv;

      shape_material = shape.mesh.material_ids[f];
    }

    //addMaterialSlotTexture(materials[shape_material].name);

    //setShapeMaterialSlot(materials[shape_material].name);

    shapes_.push_back(s);
  }

  return nx::Result::Success;
}

nx::Result nx::Geometry::optimize() {

  for (Shape& shape : shapes_) {
    // Adapted from https://github.com/opengl-tutorials/ogl/blob/master/common/vboindexer.cpp

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec3> out_normals;
    std::vector<glm::vec2> out_texcoords;
    std::vector<glm::vec3> out_tangents;
    std::vector<glm::vec3> out_bitangents;
    std::vector<u32> out_indices;

    std::unordered_map<vertex, u32> vi_map;

    bool has_normals = shape.normals_.size() > 0;
    bool has_texcoords = shape.texcoords_.size() > 0;
    bool has_tangents = shape.tangents_.size() > 0;
    bool has_bitangents = shape.bitangents_.size() > 0;

    // For each input vertex
    for (u32 i = 0; i < shape.vertices_.size(); i++) {

      vertex v;
      v.pos = shape.vertices_[i];
      if (has_normals) { v.normal = shape.normals_[i]; }
      if (has_texcoords) { v.uv = shape.texcoords_[i]; }
      
      // Try to find a similar vertex in out_XXXX
      bool found = vi_map.find(v) != vi_map.end();

      if (found) { // A similar vertex is already in the VBO, use it instead !
        u32 index = vi_map[v];
        out_indices.push_back(index);

        // Average the tangents and the bitangents
        if (has_tangents) { out_tangents[index] += shape.tangents_[i]; }
        if (has_bitangents) { out_bitangents[index] += shape.bitangents_[i]; }
      }
      else { // If not, it needs to be added in the output data.
        out_vertices.push_back(shape.vertices_[i]);
        if (has_normals)    { out_normals.push_back(shape.normals_[i]); }
        if (has_texcoords)  { out_texcoords.push_back(shape.texcoords_[i]); }
        if (has_tangents)   { out_tangents.push_back(shape.tangents_[i]); }
        if (has_bitangents) { out_bitangents.push_back(shape.bitangents_[i]); }
        out_indices.push_back((u32)out_vertices.size() - 1);
        vi_map[v] = out_indices.back();
      }
    }

    std::swap(shape.vertices_, out_vertices);
    std::swap(shape.normals_, out_normals);
    std::swap(shape.texcoords_, out_texcoords);
    std::swap(shape.tangents_, out_tangents);
    std::swap(shape.bitangents_, out_bitangents);
    std::swap(shape.indices_, out_indices);
  }
  version_++;

  return nx::Result::Success;
}

nx::Result nx::Geometry::computeTangents() {

  for (Shape& shape : shapes_) {
    // Adapted from https://github.com/opengl-tutorials/ogl/blob/master/common/tangentspace.cpp

    for (size_t i = 0; i < shape.vertices_.size(); i += 3) {

      // Shortcuts for vertices
      glm::vec3& v0 = shape.vertices_[i + 0];
      glm::vec3& v1 = shape.vertices_[i + 1];
      glm::vec3& v2 = shape.vertices_[i + 2];

      // Shortcuts for UVs
      glm::vec2& uv0 = shape.texcoords_[i + 0];
      glm::vec2& uv1 = shape.texcoords_[i + 1];
      glm::vec2& uv2 = shape.texcoords_[i + 2];

      // Edges of the triangle : position delta
      glm::vec3 deltaPos1 = v1 - v0;
      glm::vec3 deltaPos2 = v2 - v0;

      // UV delta
      glm::vec2 deltaUV1 = uv1 - uv0;
      glm::vec2 deltaUV2 = uv2 - uv0;


      float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
      glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
      glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;


      // Set the same tangent for all three vertices of the triangle.
      // They will be merged later, in vboindexer.cpp
      shape.tangents_.push_back(tangent);
      shape.tangents_.push_back(tangent);
      shape.tangents_.push_back(tangent);

      // Same thing for bitangents
      shape.bitangents_.push_back(bitangent);
      shape.bitangents_.push_back(bitangent);
      shape.bitangents_.push_back(bitangent);

    }
    // See "Going Further"
    for (u32 i = 0; i < shape.vertices_.size(); i += 1) {

      glm::vec3& n = shape.normals_[i];
      glm::vec3& t = shape.tangents_[i];
      glm::vec3& b = shape.bitangents_[i];

      // Gram-Schmidt orthogonalize
      t = glm::normalize(t - n * glm::dot(n, t));

      // Calculate handedness
      if (glm::dot(glm::cross(n, t), b) < 0.0f) {
        t = t * -1.0f;
      }

    }
  }
  version_++;

  return nx::Result::Success;
}

nx::Result nx::Geometry::clear() {

  for (Shape& shape : shapes_) {

    shape.vertices_.clear();
    shape.normals_.clear();
    shape.texcoords_.clear();
    shape.indices_.clear();
    shape.tangents_.clear();
    shape.bitangents_.clear();
  }
  
  version_++;

  return nx::Result::Success;
}

nx::Result nx::Geometry::upload() {

  return NXCore.renderer_.backend_->uploadGeometry(this);
}

nx::Result nx::Geometry::draw() {

  return NXCore.renderer_.backend_->drawGeometry(this);
}
