
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "astro/math/math.hpp"


namespace astro {
namespace core {
namespace io {
    
// Parse .obj file into array of points
class OBJFile {
public:
    struct TriangleIDX { int v1, v2, v3;};

    std::vector<math::Vec3f> points;
    std::vector<TriangleIDX> triangle_idxs;
    
    
    OBJFile() = default;
    explicit OBJFile(const std::string& filepath) {
        loadFromFile(filepath);
    }
    
    void loadFromFile(const std::string& filepath) {
        std::string line;
        std::ifstream fs(filepath);
        while (std::getline(fs, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream ss(line);
            std::string type;
            ss >> type;

            if (type == "v") {
                float x, y, z;
                ss >> x >> y >> z;
                points.emplace_back(x, y, z);
            } 
            else if (type == "f") {
                int v1, v2, v3;
                
                if (line.find('/') != std::string::npos) {
                    // Case: f v/vt/vn or f v/vt or f v//vn
                    std::istringstream fss(line.substr(line.find(' ') + 1)); // start after 'f'
                    auto read_group = [](std::istringstream& stream) -> int {
                        int v_idx;
                        char slash;
                        
                        if (!(stream >> v_idx)) return 0; 
                        if (!(stream >> slash)) return v_idx; 
                        
                        int temp_idx;
                        stream >> temp_idx; 
                        if (stream.peek() == '/') {
                            stream >> slash; 
                            stream >> temp_idx; 
                        }
                        return v_idx;
                    };

                    v1 = read_group(fss);
                    v2 = read_group(fss);
                    v3 = read_group(fss);

                } else {
                    // Case: f v1 v2 v3 (only vertex indices)
                    ss >> v1 >> v2 >> v3;
                }
                triangle_idxs.emplace_back(v1 - 1, v2 - 1, v3 - 1);
            }
        }
        fs.close();
    }
    
    
private:


};

}
}
}