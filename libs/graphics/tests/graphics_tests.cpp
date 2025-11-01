

#include <X11/X.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>

#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"
#include "astro_test.hpp"

using namespace astro::graphics;
using namespace astro::math;

#define WIDTH 800
#define HEIGHT 600
#define COLOR_DEPTH 24
#define MAX_TEST_DURATION 5 // seconds

// --- Helper functions ---
#include "astro/core/platform/IPlatformLayer.hpp"
#include "astro/core/platform/PlatformFactory.hpp"
using namespace astro::core::platform;
class TestWindow {
public:
    
    TestWindow(const std::string& window_name, int width, int height) {
        console = PlatformFactory::getPlatform();
        const LayerConfig layerConfig = {
            "AstroBurrito",
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
    color.data[3] = 255; // A
    clearCanvas(canvas, color);
    ASSERT_EQ(canv_matrix, wht_matrix);

    return true;
}

TEST(triangleDrawing){
    // Create canvas
    AstroCanvas canvas(WIDTH, HEIGHT);
    
    // Create test window
    TestWindow window("triangleDrawing", WIDTH, HEIGHT);

    // Define colors
    Color clearColor = {10, 10, 10, 255}; 
    Color red   = {255, 0, 0, 255};
    Color green = {0, 255, 0, 255};
    Color blue  = {0, 0, 255, 255};


    // Set triangle verts
    Vec2i A = {WIDTH/2 -200, HEIGHT/2 + 100};
    Vec2i B = {WIDTH/2 -150, HEIGHT/2 - 100};
    Vec2i C = {WIDTH/2 +200, HEIGHT/2 - 200};

    // Main loop
    int i = 0;
    const auto init_t = std::chrono::steady_clock::now();
    while(true){
        clearCanvas(canvas,  clearColor);

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
    
    std::cout << "a";

    return true;
}

int main(){
    bool allSuccess = run_all_tests();
    return !allSuccess;
}