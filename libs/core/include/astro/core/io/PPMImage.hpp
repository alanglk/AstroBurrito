#pragma once

#include <fstream>
#include <ios>
#include <iostream>
#include <filesystem>
#include <stdexcept>

#include "astro/graphics/graphics.hpp"

using namespace astro::graphics;

namespace astro {
namespace core {
namespace io {
    
class PPMImage {
public:
    static inline void writeImage(const std::string& path, AstroCanvas& image){
        std::filesystem::path out_path = path;
        
        if (out_path.extension().compare(".ppm") != 0) {
            std::string error = "PPMImage extension should be '.ppm', not '" + out_path.extension().string() + "'.";
            throw std::runtime_error( error );
        }
        if (!std::filesystem::exists(out_path.parent_path())) {
            std::string error = "PPMImage output path does not exist: '" + out_path.parent_path().string() + "'.";
            throw std::runtime_error( error );
        }
        if (std::filesystem::exists(out_path))
            std::cout << "PPMImage Warning: overriding existing PPM Image.\n";
        
        // Open for truncating
        std::ofstream fs(out_path, std::ios_base::trunc | std::ios_base::binary);

        // Actual Writing

        fs << magic_number << ' ' << image.width << ' ' << image.height << ' ';
        fs << 255 << '\n'; // Max colo val
        
        for (int j = 0; j < image.height; j++){
            for (int i = 0; i < image.width; i++){
                Color col = getPixel(image, i, j);
                fs << col.r << col.g << col.b;
            }
        }
        
        fs.close();
    }

    // static inline AstroCanvas readImage(const std::string& path){}

private:
    static constexpr const char magic_number[3] = "P6"; // 2 + null_terminating

};

}
}
}

