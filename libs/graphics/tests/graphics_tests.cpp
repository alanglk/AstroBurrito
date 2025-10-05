#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"
#include "astro_test.hpp"


using namespace astro::graphics;
using namespace astro::math;

TEST(clearCanvas){
    // Clear to black
    AstroCanvas canvas;
    Color color(0);
    clearCanvas(canvas, color);
    Matrix<int, WIDTH, HEIGHT> cero_matrix(0);
    Matrix<int, WIDTH, HEIGHT> canv_matrix(canvas.data, canvas.width, canvas.height);
    ASSERT_EQ(canv_matrix, cero_matrix);
    
    
    // Clear to white
    color.data[0] = 0xFF; // R
    color.data[1] = 0xFF; // G
    color.data[2] = 0xFF; // B
    color.data[3] = 0xFF; // A
    clearCanvas(canvas, color);
    Matrix<int, WIDTH, HEIGHT> wht_matrix(0xFFFFFFFF);
    ASSERT_EQ(canv_matrix, wht_matrix);

    return true;
}

int main(){
    bool allSuccess = run_all_tests();
    return 0;
}