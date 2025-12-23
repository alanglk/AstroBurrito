
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <random>
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
#include "astro/core/io/OBJFile.hpp"
#include "astro/core/camera/PerspectiveCamera.hpp"
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
    Matrix<Color, WIDTH, HEIGHT> black_matrix(black);
    Matrix<Color, WIDTH, HEIGHT> wht_matrix(white);

    // Clear to black
    AstroCanvas canvas(WIDTH, HEIGHT);
    Matrix_View<Color, WIDTH, HEIGHT> canv_matrix(canvas.data.data()); // Init with random value
    clearCanvas(canvas, black);
    ASSERT_EQ(canv_matrix, black_matrix);
    
    
    // Clear to white
    clearCanvas(canvas, white);
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
        draw2dLine(canvas, A.x, A.y, B.x, B.y, red);
        draw2dLine(canvas, B.x, B.y, C.x, C.y, green);
        draw2dLine(canvas, C.x, C.y, A.x, A.y, blue);
        
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

    
    // Parse .obj file into array of points
    std::filesystem::path wf_path = OBJ_DIABLO_PATH;
    ASSERT_TRUE(std::filesystem::exists(wf_path));
    ASSERT_TRUE(std::filesystem::is_regular_file(wf_path));
    ASSERT_EQ(wf_path.extension(), ".obj");
    astro::core::io::OBJFile wf_obj(wf_path);

    
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
        for(const auto& idx: wf_obj.triangle_idxs) {
            
            // Get triangle vertices
            const Vec3f v1 = wf_obj.points[idx.v1];
            const Vec3f v2 = wf_obj.points[idx.v2];
            const Vec3f v3 = wf_obj.points[idx.v3];
            
            // Project vertices to clip-space
            Vec2i v1_proj = project(v1);
            Vec2i v2_proj = project(v2);
            Vec2i v3_proj = project(v3);
            
            if(!isInCanvasBounds(canvas, v1_proj.x, v1_proj.y )) continue;
            if(!isInCanvasBounds(canvas, v2_proj.x, v2_proj.y )) continue;
            if(!isInCanvasBounds(canvas, v3_proj.x, v3_proj.y )) continue;
            
            draw2dLine(canvas, v1_proj.x, v1_proj.y, v2_proj.x, v2_proj.y, red);
            draw2dLine(canvas, v2_proj.x, v2_proj.y, v3_proj.x, v3_proj.y, green);
            draw2dLine(canvas, v3_proj.x, v3_proj.y, v1_proj.x, v1_proj.y, blue);
        }
        
        // Render vertices
        for(const auto P : wf_obj.points){
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
    
    astro::core::io::PPMImage::writeImage("./graphics_wireframe_test.ppm", canvas);
    
    return true;

}


TEST(perspectiveCamera) {
    // Create canvas
    AstroCanvas canvas(WIDTH, HEIGHT);
    
    // Create test window
    TestWindow window("perspectiveCamera", WIDTH, HEIGHT);

    // Parse .obj file into array of points
    std::filesystem::path wf_path = OBJ_DIABLO_PATH;
    ASSERT_TRUE(std::filesystem::exists(wf_path));
    ASSERT_TRUE(std::filesystem::is_regular_file(wf_path));
    ASSERT_EQ(wf_path.extension(), ".obj");
    astro::core::io::OBJFile wf_obj(wf_path);

    // Create Camera
    astro::core::camera::PerspectiveCamera camera(WIDTH, HEIGHT, 60.);
    const float orbitRadius = 2.;
    const float orbitDeltaAlpha = 0.05;
    float orbitAlpha = 0.0;
    
    // Create shader
    BasicShader shader;
    shader.projectionMatrix = camera.getProjectionMatrix();
    shader.modelMatrix = Mat4f::Identity();
    
    // Random color generator
    std::random_device r_dev;
    std::mt19937 rng(r_dev());
    std::uniform_int_distribution<uint8_t> dist(0, 255); // [0, 255] integer distribution
    std::vector<Color> colorList;
    colorList.reserve(wf_obj.triangle_idxs.size());
    for(int i = 0; i < wf_obj.triangle_idxs.size(); i++)
        colorList.emplace_back(dist(rng), dist(rng), dist(rng), 255);
    
    // Main loop
    int i = 0;
    const auto init_t = std::chrono::steady_clock::now();
    while(true){
        clearCanvas(canvas, gray);
        float camX = orbitRadius*std::cos(orbitAlpha);
        float camZ = orbitRadius*std::sin(orbitAlpha);
        camera.lookAt({camX, 0., camZ}, {0., 0., 0.}, {0., 1., 0.});
        orbitAlpha+=orbitDeltaAlpha;
        
        // Update shader matrices
        shader.viewMatrix = camera.getViewMatrix();
        shader.updateMVP();

        // Render wireframe
        for(int i = 0; i < wf_obj.triangle_idxs.size(); i++) {
            
            // Get triangle vertices
            const auto& idx = wf_obj.triangle_idxs[i];
            const Vec3f v1 = wf_obj.points[idx.v1];
            const Vec3f v2 = wf_obj.points[idx.v2];
            const Vec3f v3 = wf_obj.points[idx.v3];
            
            // Render triangle
            shader.color = colorList[i];
            TDRenderer::renderTriangle(canvas, {Vec4f(v1.x, v1.y, v1.z, 1.0f), Vec4f(v2.x, v2.y, v2.z, 1.0f), Vec4f(v3.x, v3.y, v3.z, 1.0f)}, shader);
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
    
    astro::core::io::PPMImage::writeImage("./graphics_camera_test.ppm", canvas);
    AstroCanvas zBufferImage = canvas.zbuffer2Image();
    astro::core::io::PPMImage::writeImage("./graphics_camera_depth_test.ppm", zBufferImage);
    return true;
}

int main(){
    // return run_test("perspectiveCamera");

    bool allSuccess = run_all_tests();
    return !allSuccess;
}