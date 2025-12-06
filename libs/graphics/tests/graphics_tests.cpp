
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"
#include "astro_test.hpp"

using namespace astro::graphics;
using namespace astro::math;

#define WIDTH 800
#define HEIGHT 600
#define COLOR_DEPTH 24
#define MAX_TEST_DURATION 5 // seconds

#define OBJ_DIABLO_PATH "/home/alanglk/AstroBurrito/assets/test/diablo3_pose.obj"

// Define colors
Color black (0, 0, 0); 
Color gray  (25, 25, 25); 
Color white (255, 255, 255);
Color red   (255, 0, 0);
Color green (0, 255, 0);
Color blue  (0, 0, 255);

// --- Helper functions ---
#include "astro/core/platform/IPlatformLayer.hpp"
#include "astro/core/platform/PlatformFactory.hpp"
#include "astro/core/io/PPMImage.hpp"
using namespace astro::core::platform;

class TestWindow {
public:
    
    TestWindow(const std::string& window_name, int width, int height) {
        console = PlatformFactory::getPlatform();
        const LayerConfig layerConfig = {
            window_name,
            WIDTH,
            HEIGHT,
            COLOR_DEPTH,
            LayerEventType::EvtNone
        };
        console->initialize(layerConfig);
    }
    void showCanvas(AstroCanvas& canvas){
        console->render(canvas);
    }
private:
    std::unique_ptr<IPlatformLayer> console;
};


// --- Testing ---
TEST(clearCanvas){
    Matrix<Color, WIDTH, HEIGHT> cero_matrix(Color(0));
    Matrix<Color, WIDTH, HEIGHT> wht_matrix(Color(255));

    // Clear to black
    AstroCanvas canvas(WIDTH, HEIGHT);
    Matrix_View<Color, WIDTH, HEIGHT> canv_matrix(canvas.data.data()); // Init with random value
    Color color(0);
    clearCanvas(canvas, color);
    ASSERT_EQ(canv_matrix, cero_matrix);
    
    
    // Clear to white
    color.data[0] = 255; // R
    color.data[1] = 255; // G
    color.data[2] = 255; // B
    clearCanvas(canvas, color);
    ASSERT_EQ(canv_matrix, wht_matrix);

    return true;
}

TEST(lineDrawing){
    // Create canvas
    AstroCanvas canvas(WIDTH, HEIGHT);
    
    // Create test window
    TestWindow window("lineDrawing", WIDTH, HEIGHT);



    // Set triangle verts
    Vec2i A = {WIDTH/2 -200, HEIGHT/2 + 100};
    Vec2i B = {WIDTH/2 -150, HEIGHT/2 - 100};
    Vec2i C = {WIDTH/2 +200, HEIGHT/2 - 200};

    // Main loop
    int i = 0;
    const auto init_t = std::chrono::steady_clock::now();
    while(true){
        clearCanvas(canvas,  gray);

        // Update triangle
        int delta_y = 5*sin(0.05* i);
        A.y += delta_y;
        B.y += delta_y;
        C.y += delta_y;
        drawLine(canvas, A.x, A.y, B.x, B.y, red);
        drawLine(canvas, B.x, B.y, C.x, C.y, green);
        drawLine(canvas, C.x, C.y, A.x, A.y, blue);
        
        // Render on window
        window.showCanvas(canvas);

        // Check for ellapsed time
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - init_t).count();
        if (elapsed >= MAX_TEST_DURATION) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        i++;
    }
    
    return true;
}


TEST(wireframeDrawing){
    // Create canvas
    AstroCanvas canvas(WIDTH, HEIGHT);
    
    // Create test window
    TestWindow window("wireframeDrawing", WIDTH, HEIGHT);

    
    // Parse .obj file into array of pointsç
    struct TriangleIDX { int v1, v2, v3;};
    std::vector<Vec3f> points;
    std::vector<TriangleIDX> triangle_idxs;

    
    std::filesystem::path wf_path = OBJ_DIABLO_PATH;
    ASSERT_TRUE(std::filesystem::exists(wf_path));
    ASSERT_TRUE(std::filesystem::is_regular_file(wf_path));
    ASSERT_EQ(wf_path.extension(), ".obj");
    
    std::string line;
    std::ifstream fs(wf_path);
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
    
    // Just a Viewport transform. The model is already in the NDC space
    const std::function<Vec2i(Vec3f)> project = [](Vec3f P) {
        int x = static_cast<int>((P.x + 1.0) * WIDTH / 2.0f);
        int y = static_cast<int>(-1.0f * (P.y + 1.0) * (HEIGHT / 2.0f) + HEIGHT );
        
        return Vec2i(x, y);
    };
    
    // Main loop
    int i = 0;
    const auto init_t = std::chrono::steady_clock::now();
    while(true){
        clearCanvas(canvas, gray);
        
        // Render wireframe
        for(const auto& idx: triangle_idxs) {
            
            // Get triangle vertices
            const Vec3f v1 = points[idx.v1];
            const Vec3f v2 = points[idx.v2];
            const Vec3f v3 = points[idx.v3];
            
            // Project vertices to clip-space
            Vec2i v1_proj = project(v1);
            Vec2i v2_proj = project(v2);
            Vec2i v3_proj = project(v3);
            
            if(!isInCanvasBounds(canvas, v1_proj.x, v1_proj.y )) continue;
            if(!isInCanvasBounds(canvas, v2_proj.x, v2_proj.y )) continue;
            if(!isInCanvasBounds(canvas, v3_proj.x, v3_proj.y )) continue;
            
            drawLine(canvas, v1_proj.x, v1_proj.y, v2_proj.x, v2_proj.y, red);
            drawLine(canvas, v2_proj.x, v2_proj.y, v3_proj.x, v3_proj.y, green);
            drawLine(canvas, v3_proj.x, v3_proj.y, v1_proj.x, v1_proj.y, blue);
        }
        
        // Render vertices
        for(const auto P : points){
            const Vec2i p = project(P);
            if(!isInCanvasBounds(canvas, p.x, p.y )) continue;
            putPixel(canvas, p.x, p.y, white);
        }

        // Show on window
        window.showCanvas(canvas);

        // Check for ellapsed time
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - init_t).count();
        if (elapsed >= MAX_TEST_DURATION) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        i++;
    }
    
    astro::core::io::PPMImage::writeImage("./graphics_tests.ppm", canvas);
    
    return true;

}


int main(){
    return run_test("wireframeDrawing");

    bool allSuccess = run_all_tests();
    return !allSuccess;
}