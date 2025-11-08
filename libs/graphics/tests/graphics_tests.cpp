
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
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
    struct TriangleIDX { int v1, v2, v3 = 0;};
    std::vector<Vec4f> points;
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
            points.emplace_back(x, y, z, 1.0); // Homogeneous coord
        } 
        else if (type == "f") {
            int v1, v2, v3;
            char slash; // to skip '/' if needed
            if (line.find('/') != std::string::npos) {
                // handles f like "1/2/3 4/5/6 7/8/9"
                std::replace(line.begin(), line.end(), '/', ' ');
                std::istringstream fss(line);
                fss >> type >> v1 >> v2 >> v3; // read only vertex indices
            } else {
                ss >> v1 >> v2 >> v3;
            }
            triangle_idxs.emplace_back(v1 - 1, v2 - 1, v3 - 1);
        }
    }
    fs.close();
    
    // Projection matrices
    float fx = WIDTH / 10.0; // Ortographic projection
    float fy = WIDTH / 10.0; 
    float cx = WIDTH /2.0;
    float cy = HEIGHT /2.0;
    
    Mat3f K = {
        fx, 0, cx,
        0, fy, cy,
        0, 0, 1
    };
    Matrix<float, 3, 4> Rt = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0
    }; 
    Matrix<float, 3, 4> projection_mat = K * Rt;



    // Main loop
    int i = 0;
    const auto init_t = std::chrono::steady_clock::now();
    while(true){
        clearCanvas(canvas, gray);
        
        // Render wireframe
        for(const auto& idx: triangle_idxs) {
            
            // Get triangle vertices
            const Vec4f v1 = points[idx.v1];
            const Vec4f v2 = points[idx.v2];
            const Vec4f v3 = points[idx.v3];
            
            // Project vertices to clip-space
            Vec3f v1_proj = projection_mat * v1;
            Vec3f v2_proj = projection_mat * v2;
            Vec3f v3_proj = projection_mat * v3;
            
            v1_proj.x /= v1_proj.z;
            v1_proj.y /= v1_proj.z;

            v2_proj.x /= v2_proj.z;
            v2_proj.y /= v2_proj.z;

            v3_proj.x /= v3_proj.z;
            v3_proj.y /= v3_proj.z;
            
            if(!isInCanvasBounds(canvas, v1_proj.x, v1_proj.y )) continue;
            if(!isInCanvasBounds(canvas, v2_proj.x, v2_proj.y )) continue;
            if(!isInCanvasBounds(canvas, v3_proj.x, v3_proj.y )) continue;
            
            // Draw lines
            drawLine(canvas, v1_proj.x, v1_proj.y, v2_proj.x, v2_proj.y, red);
            drawLine(canvas, v2_proj.x, v2_proj.y, v3_proj.x, v3_proj.y, green);
            drawLine(canvas, v3_proj.x, v3_proj.y, v1_proj.x, v1_proj.y, blue);

            // Draw vertices
            putPixel(canvas, v1_proj.x, v1_proj.y, white);
            putPixel(canvas, v2_proj.x, v2_proj.y, white);
            putPixel(canvas, v3_proj.x, v3_proj.y, white);
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
    
    return true;

}


int main(){
    return run_test("wireframeDrawing");

    bool allSuccess = run_all_tests();
    return !allSuccess;
}