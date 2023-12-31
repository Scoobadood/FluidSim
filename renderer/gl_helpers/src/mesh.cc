#include "mesh_internal.h"
#include "gl_common.h"

#include <string>
#include <map>
#include <sstream>

#include "string_utils.h"
#include "spdlog/spdlog-inl.h"


bool parse_face_elements(const std::string &face_elem,
                         int32_t *vertex_idx,
                         bool include_normal,
                         int32_t *normal_idx,
                         bool include_tex_coord,
                         int32_t *tex_coord_idx
) {
  using namespace std;

  if (face_elem.empty()) {
    spdlog::error("  invalid (empty) element string");
    return false;
  }

  auto f = face_elem;
  trim(f);

  int32_t idx1 = -1, idx2 = -1;
  for (auto i = 0; i < f.size(); ++i) {
    const char c = f[i];
    if (!isdigit(c) && (c != '/')) {
      spdlog::error("  invalid element string: {}", f);
      return false;
    }
    if (c == '/') {
      if (idx1 == -1) {
        idx1 = i;
      } else if (idx2 == -1) {
        idx2 = i;
      } else {
        spdlog::error("  invalid form. Too many '/'. {}", f);
        return false;
      }
    }
  }

  // Sanity check slash positions
  if (idx1 == 0 || idx1 == f.size() - 1 || idx2 == f.size() - 1) {
    spdlog::error("  invalid form: {}", f);
    return false;
  }

  bool tex_coord_idx_present = (idx1 != -1 && ((idx2 == -1) || (idx2 - idx1 > 1)));
  bool normal_idx_present = (idx2 != -1);


  if (vertex_idx == nullptr) {
    spdlog::error("  vertex_idx may not be null");
    return false;
  }
  *vertex_idx = (idx1 == -1)
                ? stoi(f)
                : stoi(f.substr(0, idx1));
  if (!include_tex_coord && !include_normal) return true;




  if (include_tex_coord) {
    if (tex_coord_idx == nullptr) {
      spdlog::error("  tex_coord_idx requested but pointer is null");
      return false;
    }
    if (!tex_coord_idx_present) {
      spdlog::error("  tex_coord_idx requested but not present: {}", f);
      return false;
    }
    *tex_coord_idx = (idx2 == -1)
                  ? stoi(f.substr(idx1 + 1))
                  : stoi(f.substr(idx1 + 1, idx2 - idx1 - 1));
  }
  if (!include_normal) return true;


  if (normal_idx == nullptr) {
    spdlog::error("  normal_idx requested but pointer is null");
    return false;
  }
  if (!normal_idx_present) {
    spdlog::error("  normal_idx requested but not present: {}", f);
    return false;
  }
  *normal_idx = stoi(f.substr(idx2 + 1));
  return true;
}

bool parse_3f(const std::string &args, float &x, float &y, float &z) {
  std::string t_args = args;
  trim(t_args);
  auto tokens = tokenise(t_args, ' ');
  if (tokens.size() < 3) {
    spdlog::error("  expected three values in : {}", args);
    return false;
  }
  if (tokens.size() > 3) {
    spdlog::warn("  found {} values, expected only three in : {}", tokens.size(), args);
  }
  x = std::stof(tokens[0]);
  y = std::stof(tokens[1]);
  z = std::stof(tokens[2]);
  return true;
}

bool parse_2f(const std::string &args, float &x, float &y) {
  std::string t_args = args;
  trim(t_args);
  auto tokens = tokenise(t_args, ' ');
  if (tokens.size() < 2) {
    spdlog::error("  expected two values in : {}", args);
    return false;
  }
  if (tokens.size() > 2) {
    spdlog::warn("  found {} values, expected only two in : {}", tokens.size(), args);
  }
  x = std::stof(tokens[0]);
  y = std::stof(tokens[1]);
  return true;
}

bool parse_face(const std::string &args, int32_t *vertices,
                bool include_normals,
                int32_t *normals,
                bool include_tex_coords,
                int32_t *tex_coords) {
  auto args_t = args;
  trim(args_t);
  auto face_elems = tokenise(args_t, ' ');
  if (face_elems.size() != 3) {
    spdlog::error("  ignoring {}, expected 3 tokens", args_t);
    return false;
  }
  if (include_normals && normals == nullptr) {
    spdlog::error("  normals requested but normalIdx is null");
    return false;
  }
  if (include_tex_coords && tex_coords == nullptr) {
    spdlog::error("  tex coords requested but tex_coords is null");
    return false;
  }

  for (auto i = 0; i < 3; ++i) {
    bool ok = true;
    if (include_normals && include_tex_coords) {
      ok = parse_face_elements(face_elems[i], vertices + i,
                               true, normals + i,
                               true, tex_coords + i);
    } else if (include_normals) {
      ok = parse_face_elements(face_elems[i], vertices + i,
                               true, normals + i,
                               false, nullptr);
    } else if (include_tex_coords) {
      ok = parse_face_elements(face_elems[i], vertices + i,
                               false, nullptr,
                               true, tex_coords + i);
    } else {
      ok = parse_face_elements(face_elems[i], vertices + i);
    }
    if (!ok) {
      spdlog::error("  failed to parse entry {} in {}", i, args_t);
      return false;
    }
  }
  // Correct all indices as they are off by one from OBJ file
  for (auto i = 0; i < 3; ++i) {
    vertices[i]--;
    if (include_normals) normals[i]--;
    if (include_tex_coords) tex_coords[i]--;
  }
  return true;
}

bool parse_raw_data(std::ifstream &f,
                    std::vector<std::tuple<float, float, float>> &vertices,
                    std::vector<std::vector<std::tuple<int32_t, int32_t, int32_t>>> &faces,
                    bool include_normals,
                    std::vector<std::tuple<float, float, float>> *normals,
                    bool include_tex_coords,
                    std::vector<std::tuple<float, float>> *tex_coords
) {
  using namespace std;

  string line;
  while (getline(f, line)) {
    trim(line);
    if (line.empty()) continue;
    if (line[0] == '#') continue;

    istringstream is(line);
    string type;
    is >> type;
    to_lower(type);
    if ("v" == type) {
      float x, y, z;
      auto ok = parse_3f(line.substr(1), x, y, z);
      if (!ok) {
        spdlog::error("  ignored vertex line: {}", line);
        continue;
      }
      vertices.emplace_back(x, y, z);
    } else if ("vt" == type) {
      if (!include_tex_coords) continue;

      float u, v;
      auto ok = parse_2f(line.substr(2), u, v);
      if (!ok) {
        spdlog::error("  ignored texture coord line: {}", line);
        continue;
      }
      tex_coords->emplace_back(u, v);
    } else if ("vn" == type) {
      if (!include_normals) continue;

      float x, y, z;
      auto ok = parse_3f(line.substr(2), x, y, z);
      if (!ok) {
        spdlog::error("  ignored normal line: {}", line);
        continue;
      }
      normals->emplace_back(x, y, z);
    } else if ("f" == type) {
      int32_t v[3]{-1, -1, -1},
              n[3]{-1, -1, -1},
              t[3]{-1, -1, -1};
      auto ok = parse_face(line.substr(1), v,
                           include_normals, n,
                           include_tex_coords, t);
      if (!ok) {
        spdlog::error("  ignored bad face definition: {}", line);
        continue;
      }
      faces.emplace_back();
      faces.back().emplace_back(v[0], n[0], t[0]);
      faces.back().emplace_back(v[1], n[1], t[1]);
      faces.back().emplace_back(v[2], n[2], t[2]);
    } else {
      // Ignore l, object, vp, materials etc.
      spdlog::warn("  ignored : {}", line);
    }
  }
  if (vertices.empty()) {
    spdlog::error("  no vertices found.");
    return false;
  }
  return true;
}

/*
 * Load meshes from OBJ files and then
 * * Remap vertices to allow use of ELEMENT indexing for unique vertices
 * * Reorder vertices into triangle strips to optimise draw calls/render time.
 *
 */
bool load_obj(const std::string &obj_file_name,
              uint32_t &vao,
              uint32_t &vbo,
              uint32_t &ebo,
              int32_t &num_elements,
              uint32_t pos_attr,
              bool include_normals,
              uint32_t norm_attr,
              bool include_textures,
              uint32_t tx_attr
) {
  using namespace std;

  spdlog::info("load_obj( \"{}\" )", obj_file_name);

  ifstream f(obj_file_name);
  if (!f.is_open()) {
    spdlog::error("Couldn't open OBJ file {}", obj_file_name);
    return false;
  }

  vector<tuple<float, float, float>> vertices;
  vector<tuple<float, float, float>> normals;
  vector<tuple<float, float>> tex_coords;
  vector<vector<tuple<int32_t, int32_t, int32_t>>> faces;

  spdlog::info("1. Parse raw data");
  if (!parse_raw_data(f, vertices, faces, include_normals, &normals,
                      include_textures, &tex_coords)) {
    return false;
  }

  spdlog::info("Found {:3} vertices", vertices.size());
  if (include_normals) spdlog::info("      {:3} normals", normals.size());
  if (include_textures) spdlog::info("      {:3} tex_coords", tex_coords.size());
  spdlog::info("      {:3} faces", faces.size());

  spdlog::info("2. Identifying unique faces");
  vector<float> vertex_data;
  vector<int32_t> eidx;

  map<tuple<int32_t, int32_t, int32_t>, int32_t> vtx_lookup;
  auto gidx = 0;
  for (const auto &face: faces) {
    for (const auto &face_elem: face) {

      auto it = vtx_lookup.find(face_elem);
      if (it != vtx_lookup.end()) {
        eidx.push_back(it->second);
        continue;
      }

      // New combo. Create a new vertex.
      auto vidx = get<0>(face_elem);
      vertex_data.push_back(get<0>(vertices.at(vidx)));
      vertex_data.push_back(get<1>(vertices.at(vidx)));
      vertex_data.push_back(get<2>(vertices.at(vidx)));

      if (include_normals) {
        auto nidx = get<1>(face_elem);
        vertex_data.push_back(get<0>(normals.at(nidx)));
        vertex_data.push_back(get<1>(normals.at(nidx)));
        vertex_data.push_back(get<2>(normals.at(nidx)));
      }
      if (include_textures) {
        auto tidx = get<2>(face_elem);
        vertex_data.push_back(get<0>(tex_coords.at(tidx)));
        vertex_data.push_back(get<1>(tex_coords.at(tidx)));
      }

      vtx_lookup.emplace(face_elem, gidx);
      eidx.push_back(gidx);
      ++gidx;
    }
  }

  spdlog::info("3. Building VAO, VBO and EBO");
  // VAO

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  auto vtx_sz = 4 * (3 + (include_normals ? 3 : 0) + (include_textures ? 2 : 0));
  auto num_vertices = gidx;

  // Vertex locations
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vtx_sz * num_vertices, vertex_data.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(pos_attr);
  glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) nullptr);

  if (include_normals) {
    glEnableVertexAttribArray(norm_attr);
    glVertexAttribPointer(norm_attr, 3, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 12);
  }
  if (include_textures) {
    glEnableVertexAttribArray(tx_attr);
    glVertexAttribPointer(tx_attr, 2, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 24);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, eidx.size() * sizeof(int32_t), eidx.data(), GL_STATIC_DRAW);

  num_elements = eidx.size();
  return true;
}