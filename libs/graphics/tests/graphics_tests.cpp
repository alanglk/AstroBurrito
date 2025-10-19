#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"
#include "astro_test.hpp"


#define WIDTH 800
#define HEIGHT 600
using namespace astro::graphics;
using namespace astro::math;

TEST(clearCanvas){
    Matrix<int, WIDTH, HEIGHT> cero_matrix(0);
    Matrix<int, WIDTH, HEIGHT> wht_matrix(0xFFFFFFFF);

    // Clear to black
    Matrix<int, WIDTH, HEIGHT> canv_matrix(0xFF000000);
    AstroCanvas canvas = {canv_matrix.data.data(), canv_matrix.rows, canv_matrix.cols};
    Color color(0);
    clearCanvas(canvas, color);
    ASSERT_EQ(canv_matrix, cero_matrix);
    
    
    // Clear to white
    color.data[0] = 0xFF; // R
    color.data[1] = 0xFF; // G
    color.data[2] = 0xFF; // B
    color.data[3] = 0xFF; // A
    clearCanvas(canvas, color);
    ASSERT_EQ(canv_matrix, wht_matrix);

    return true;
}

int main(){
    bool allSuccess = run_all_tests();
    return 0;
}