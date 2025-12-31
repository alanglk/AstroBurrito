
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
        
        // Compute Tangent vectors
        for (size_t i = 0; i < indices.size(); i += 3) {
            uint32_t i0 = indices[i];
            uint32_t i1 = indices[i + 1];
            uint32_t i2 = indices[i + 2];

            graphics::VertexAttributes& v0 = vertices[i0];
            graphics::VertexAttributes& v1 = vertices[i1];
            graphics::VertexAttributes& v2 = vertices[i2];

            math::Vec3f edge1 = (v1.pos - v0.pos).xyz;
            math::Vec3f edge2 = (v2.pos - v0.pos).xyz;

            math::Vec2f deltaUV1 = v1.uv - v0.uv;
            math::Vec2f deltaUV2 = v2.uv - v0.uv;

            float det = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            math::Vec3f tangent;

            if (std::abs(det) < 1e-6f) {
                // Degenerate UVs, use a default tangent based on the first edge
                tangent = math::normalize(edge1);
            } else {
                float f = 1.0f / det;
                tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                tangent = math::normalize(tangent);
            }

            // Accumulate into all three vertices of the triangle (for smoothing)
            v0.tangent = v0.tangent + tangent;
            v1.tangent = v1.tangent + tangent;
            v2.tangent = v2.tangent + tangent;
        }

        // Re-normalize and Orthonormalize (Gram-Schmidt)
        for (auto& v : vertices) {
            if (math::len(v.tangent) > 0.0001f) {
                v.tangent = math::normalize(v.tangent);
                
                // Gram-Schmidt: Ensure tangent is perfectly orthogonal to the normal
                // T = normalize(T - N * dot(N, T))
                v.tangent = math::normalize(v.tangent - v.normal * math::dot(v.normal, v.tangent));
            } else {
                // Fallback if tangent is zero (e.g. no UVs provided)
                v.tangent = math::Vec3f(1, 0, 0); 
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