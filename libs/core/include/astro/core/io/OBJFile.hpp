
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "astro/math/math.hpp"
#include "astro/graphics/graphics.hpp"

namespace astro {
namespace core {
namespace io {
    
/**
 * @brief OBJ File Parser
 */
class OBJFile {
public:
    OBJFile() = default;
    explicit OBJFile(const std::string& filepath) { loadFromFile(filepath); }
    ~OBJFile() = default;

    // Parsed data
    std::vector<graphics::VertexAttributes> vertices;
    std::vector<uint32_t> indices;

    void loadFromFile(const std::string& filepath) {
        std::vector<math::Vec3f> temp_positions;
        std::vector<math::Vec3f> temp_normals;
        std::vector<math::Vec2f> temp_texcoords;

        // Maps {pos_idx, uv_idx, norm_idx} -> Index in 'vertices' vector
        std::map<std::tuple<int, int, int>, uint32_t> unique_vertices;

        std::ifstream fs(filepath);
        std::string line;
        while (std::getline(fs, line)) {
            std::istringstream ss(line);
            std::string type;
            ss >> type;

            if (type == "v") {
                math::Vec3f v; ss >> v.x >> v.y >> v.z;
                temp_positions.push_back(v);
            } else if (type == "vt") {
                math::Vec2f vt; ss >> vt.x >> vt.y;
                temp_texcoords.push_back(vt);
            } else if (type == "vn") {
                math::Vec3f vn; ss >> vn.x >> vn.y >> vn.z;
                temp_normals.push_back(vn);
            } else if (type == "f") {
                std::vector<uint32_t> face_indices;
                std::string part;
                
                while (ss >> part) {
                    auto [v_idx, vt_idx, vn_idx] = parseFacePart(part, temp_positions, temp_texcoords, temp_normals);
                    
                    auto key = std::make_tuple(v_idx, vt_idx, vn_idx);
                    if (unique_vertices.find(key) == unique_vertices.end()) {
                        // Create new VertexAttribute
                        graphics::VertexAttributes attr;
                        attr.pos = math::Vec4f(temp_positions[v_idx - 1], 1.0f);
                        attr.uv = (vt_idx > 0) ? temp_texcoords[vt_idx - 1] : math::Vec2f(0, 0);
                        attr.normal = (vn_idx > 0) ? temp_normals[vn_idx - 1] : math::Vec3f(0, 0);
                        
                        unique_vertices[key] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(attr);
                    }
                    face_indices.push_back(unique_vertices[key]);
                }

                // 2. Triangulation (Fan-style)
                // This handles quads and n-gons exported by Blender/Maya
                if (face_indices.size() < 3) {
                    std::cerr << "OBJFile Warning: Skipping degenerate face with " << face_indices.size() << " vertices." << std::endl;
                    continue; 
                }
                for (size_t i = 1; i < face_indices.size() - 1; ++i) {
                    indices.push_back(face_indices[0]);
                    indices.push_back(face_indices[i]);
                    indices.push_back(face_indices[i+1]);
                }
            }
        }
        
        // Final Safety Check 2: Index buffer length
        if (indices.size() % 3 != 0) {
            // This should be logically impossible with the Fan Triangulation 
            std::cerr << "OBJFile Error: Index count " << indices.size() 
                      << " is not a multiple of 3!" << std::endl;
            
            // Strategy: Trim the excess to prevent out-of-bounds reads during render
            indices.resize(indices.size() - (indices.size() % 3));
        }
    }

private:
    // Internal helper to parse v/vt/vn strings
    std::tuple<int, int, int> parseFacePart(std::string part, const auto& p, const auto& t, const auto& n) {
        int v = 0, vt = 0, vn = 0;
        std::replace(part.begin(), part.end(), '/', ' ');
        std::istringstream vss(part);
        
        vss >> v;
        if (part.find("  ") != std::string::npos) { // Case v//vn
            vss >> vn;
        } else {
            if (!(vss >> vt)) vt = 0;
            if (!(vss >> vn)) vn = 0;
        }

        // Handle Relative Indexing (Negative values)
        if (v < 0) v = p.size() + v + 1;
        if (vt < 0) vt = t.size() + vt + 1;
        if (vn < 0) vn = n.size() + vn + 1;

        return {v, vt, vn};
    }
};

}
}
}