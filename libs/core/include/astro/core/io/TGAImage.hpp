#pragma once

#include <fstream>
#include <ios>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <vector>

#include "astro/graphics/graphics.hpp"

namespace astro {
namespace core {
namespace io {

class TGAImage {
public:
    static constexpr int TGA_HEADER_SIZE = 18;
    static constexpr int TGA_TYPE_UNCOMPRESSED_RGB = 2;
    static constexpr int TGA_TYPE_RLE_RGB = 10; // New support

    static inline graphics::AstroCanvas readImage(const std::string& path) {
        std::filesystem::path in_path(path);
        
        if (!std::filesystem::exists(in_path)) {
            throw std::runtime_error("TGA Error: File not found: " + path);
        }

        std::ifstream fs(in_path, std::ios::binary);
        if (!fs.is_open()) {
            throw std::runtime_error("TGA Error: Could not open file: " + path);
        }

        // --- 1. Read Header ---
        uint8_t header[TGA_HEADER_SIZE];
        if (!fs.read(reinterpret_cast<char*>(header), TGA_HEADER_SIZE)) {
            throw std::runtime_error("TGA Error: Could not read header.");
        }

        const uint8_t idLength     = header[0];
        const uint8_t colorMapType = header[1];
        const uint8_t imageType    = header[2];

        // Validate Image Type (Support 2 and 10)
        if (imageType != TGA_TYPE_UNCOMPRESSED_RGB && imageType != TGA_TYPE_RLE_RGB) {
            throw std::runtime_error("TGA Error: Unsupported image type (" + std::to_string(imageType) + 
                                     "). Only Type 2 (RGB) and Type 10 (RLE RGB) are supported.");
        }

        const int width  = header[12] | (header[13] << 8);
        const int height = header[14] | (header[15] << 8);
        const int bpp    = header[16];
        const int descriptor = header[17];

        if (width <= 0 || height <= 0) {
            throw std::runtime_error("TGA Error: Invalid dimensions.");
        }
        if (bpp != 24 && bpp != 32) {
            throw std::runtime_error("TGA Error: Unsupported bit depth: " + std::to_string(bpp));
        }

        // --- 2. Skip Metadata ---
        int colorMapLen = header[5] | (header[6] << 8);
        int colorMapEntrySize = header[7];
        int skipSize = idLength + (colorMapType == 1 ? (colorMapLen * (colorMapEntrySize / 8)) : 0);
        
        if (skipSize > 0) {
            fs.seekg(skipSize, std::ios::cur);
        }

        // --- 3. Read Pixel Data ---
        size_t bytesPerPixel = bpp / 8;
        size_t totalPixels = static_cast<size_t>(width) * height;
        std::vector<uint8_t> pixelData;
        pixelData.reserve(totalPixels * bytesPerPixel);

        if (imageType == TGA_TYPE_UNCOMPRESSED_RGB) {
            // TYPE 2: Bulk Read
            pixelData.resize(totalPixels * bytesPerPixel);
            if (!fs.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size())) {
                throw std::runtime_error("TGA Error: Failed to read uncompressed data.");
            }
        } 
        else if (imageType == TGA_TYPE_RLE_RGB) {
            // TYPE 10: RLE Decode
            // RLE Encoding logic:
            // 1. Read packet header (1 byte).
            // 2. If bit 7 is 0: It's a RAW packet. Read (header + 1) pixels directly.
            // 3. If bit 7 is 1: It's an RLE packet. Read 1 pixel, repeat it (header - 127) times.
            
            size_t currentPixels = 0;
            std::vector<uint8_t> pixelBuffer(bytesPerPixel); // Temp storage for one pixel

            while (currentPixels < totalPixels) {
                if (fs.peek() == EOF) break;

                uint8_t chunkHeader = fs.get();
                
                if (chunkHeader < 128) {
                    // RAW Packet
                    int count = chunkHeader + 1;
                    for (int i = 0; i < count; ++i) {
                        if (!fs.read(reinterpret_cast<char*>(pixelBuffer.data()), bytesPerPixel)) {
                            throw std::runtime_error("TGA Error: Unexpected EOF in RLE Raw packet.");
                        }
                        pixelData.insert(pixelData.end(), pixelBuffer.begin(), pixelBuffer.end());
                    }
                    currentPixels += count;
                } else {
                    // RLE Packet
                    int count = chunkHeader - 127;
                    if (!fs.read(reinterpret_cast<char*>(pixelBuffer.data()), bytesPerPixel)) {
                        throw std::runtime_error("TGA Error: Unexpected EOF in RLE Repeat packet.");
                    }
                    // Repeat this pixel 'count' times
                    for (int i = 0; i < count; ++i) {
                        pixelData.insert(pixelData.end(), pixelBuffer.begin(), pixelBuffer.end());
                    }
                    currentPixels += count;
                }
            }
        }
        
        fs.close();

        // --- 4. Orientation & Mapping ---
        graphics::AstroCanvas image(width, height);
        bool isTopLeft = (descriptor & 0x20); 

        for (int y = 0; y < height; ++y) {
            int targetY = isTopLeft ? y : (height - 1 - y);

            for (int x = 0; x < width; ++x) {
                size_t idx = (y * width + x) * bytesPerPixel;
                
                // Safety check to ensure we don't go out of bounds on malformed RLE
                if (idx + bytesPerPixel > pixelData.size()) continue;

                uint8_t b = pixelData[idx];
                uint8_t g = pixelData[idx + 1];
                uint8_t r = pixelData[idx + 2];
                uint8_t a = (bytesPerPixel == 4) ? pixelData[idx + 3] : 255;

                graphics::Color col = { r, g, b, a };
                graphics::putPixel(image, x, targetY, col);
            }
        }

        return image;
    }

    // writeImage remains the same as the optimized version previously provided
    static inline void writeImage(const std::string& path, const graphics::AstroCanvas& image) {
        std::ofstream fs(path, std::ios::binary);
        if (!fs.is_open()) throw std::runtime_error("Cannot create: " + path);

        int w = image.width, h = image.height;
        uint8_t header[18] = {0};
        header[2] = 2; header[12] = w & 0xFF; header[13] = (w >> 8) & 0xFF;
        header[14] = h & 0xFF; header[15] = (h >> 8) & 0xFF;
        header[16] = 32; header[17] = 0x20; 

        fs.write((char*)header, 18);
        std::vector<uint8_t> buf; buf.reserve(w * h * 4);
        for(int y=0; y<h; ++y) {
            for(int x=0; x<w; ++x) {
                auto p = graphics::getPixel(image, x, y);
                buf.insert(buf.end(), {p.b, p.g, p.r, p.a});
            }
        }
        fs.write((char*)buf.data(), buf.size());
    }
};

} // namespace io
} // namespace core
} // namespace astro