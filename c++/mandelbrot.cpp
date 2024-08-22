#include <SDL2/SDL.h>
#include <iostream>
#include <complex>

const int WIDTH = 800;
const int HEIGHT = 800;
const int MAX_ITER = 1000;

Uint32 mandelbrot(double real, double imag) {
    std::complex<double> c(real, imag);
    std::complex<double> z(0, 0);
    int iter;

    for (iter = 0; iter < MAX_ITER; ++iter) {
        if (std::norm(z) > 4.0) break;
        z = z * z + c;
    }

    // Map iteration count to color
    Uint8 color = static_cast<Uint8>(iter % 256);
    return SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGB888), color, color, color);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Mandelbrot Set",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    Uint32* pixels = new Uint32[WIDTH * HEIGHT];

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;
            pixels[y * WIDTH + x] = mandelbrot(real, imag);
        }
    }

    SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(5000); // Display the window for 5 seconds

    // Clean up
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
