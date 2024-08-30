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

    Uint8 color = static_cast<Uint8>(iter % 256);
    return SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGB888), color, color, color);
}

void drawMandelbrot(SDL_Renderer* renderer, double x_min, double x_max, double y_min, double y_max) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double real = x_min + (x / (WIDTH - 1.0)) * (x_max - x_min);
            double imag = y_min + (y / (HEIGHT - 1.0)) * (y_max - y_min);
            Uint32 color = mandelbrot(real, imag);
            SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Mandelbrot Zoom",
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

    double x_min = -2.0, x_max = 1.0;
    double y_min = -1.5, y_max = 1.5;
    double zoom_factor = 1.0;

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        y_min += 0.1 * zoom_factor;
                        y_max += 0.1 * zoom_factor;
                        break;
                    case SDLK_DOWN:
                        y_min -= 0.1 * zoom_factor;
                        y_max -= 0.1 * zoom_factor;
                        break;
                    case SDLK_LEFT:
                        x_min -= 0.1 * zoom_factor;
                        x_max -= 0.1 * zoom_factor;
                        break;
                    case SDLK_RIGHT:
                        x_min += 0.1 * zoom_factor;
                        x_max += 0.1 * zoom_factor;
                        break;
                    case SDLK_PLUS:
                        zoom_factor *= 1.1;
                        break;
                    case SDLK_MINUS:
                        zoom_factor /= 1.1;
                        break;
                }
            }
        }

        SDL_RenderClear(renderer);
        drawMandelbrot(renderer, x_min, x_max, y_min, y_max);
        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Add a small delay to limit rendering speed
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
