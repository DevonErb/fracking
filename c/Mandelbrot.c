#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

// Function to map a point on the complex plane to the screen coordinates
void mandelbrot(double real, double imag, int *r, int *g, int *b) {
    double c_real = real;
    double c_imag = imag;
    double x = 0.0;
    double y = 0.0;
    int iter;
    
    for (iter = 0; iter < MAX_ITER; iter++) {
        double x2 = x * x;
        double y2 = y * y;
        if (x2 + y2 > 4.0) break;
        y = 2 * x * y + c_imag;
        x = x2 - y2 + c_real;
    }
    
    // Map iteration count to color
    *r = (iter % 256);
    *g = (iter % 256);
    *b = (iter % 256);
}

int main() {
    FILE *fp = fopen("mandelbrot.ppm", "wb");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }
    
    fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;
            int r, g, b;
            mandelbrot(real, imag, &r, &g, &b);
            fputc(r, fp);
            fputc(g, fp);
            fputc(b, fp);
        }
    }

    fclose(fp);
    printf("Mandelbrot set image saved to 'mandelbrot.ppm'\n");
    return 0;
}
