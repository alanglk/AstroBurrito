#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"
#include "astro_test.hpp"

#define WIDTH 3
#define HEIGHT 3
using namespace astro::graphics;
using namespace astro::math;

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

int main(){
    bool allSuccess = run_all_tests();
    return !allSuccess;
}