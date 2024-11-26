#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <random>
#include <iostream>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 960;

// Function to calculate the midpoint (used for fractal generation)
SDL_Point getDistance(SDL_Point point1, SDL_Point point2) {
    SDL_Point midpoint;
    midpoint.x = (point1.x + point2.x) / 2;
    midpoint.y = (point1.y + point2.y) / 2;
    return midpoint;
}

// Function to render the points inside the fractal
void renderInside(SDL_Renderer* renderer, SDL_Point points[4], float scale, SDL_Rect viewport) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 3);
    SDL_Point initial = points[1];

    // Scale the number of points generated based on the zoom level
    int numPoints = (scale * 100000);
    
    // Adjust the number of points based on zoom level to save resources
    if (scale < 1.0f) {
        numPoints /= 2; // For zooming out, generate fewer points
    }

    // Generate fractal points and only render those that are within the viewport
    for (int i = 0; i < numPoints; i++) {
        int randomnum = dis(gen);
        SDL_Point corner = points[randomnum];
        initial = getDistance(initial, corner);

        // Check if the point is within the visible viewport
        if (initial.x >= viewport.x && initial.x <= viewport.x + viewport.w &&
            initial.y >= viewport.y && initial.y <= viewport.y + viewport.h) {
            
            if(randomnum == 1)
            {
              SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // White color
            }else if (randomnum == 2) {
              SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // White color
            }else if (randomnum == 3) {
              SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // White color
            }




            SDL_RenderDrawPoint(renderer, initial.x, initial.y);
        }
    }
}

// Function to handle zoom and pan and adjust the vertices of the triangle
void renderZoom(SDL_Renderer* renderer, float scale, SDL_Point offset) {
    // Define the triangle vertices (equilateral triangle)
    SDL_Point points[4] = {
        { 640, 110 },  // Top vertex
        { 320, 850 },  // Bottom-left vertex
        { 960, 850 },  // Bottom-right vertex
        { 640, 110 }   // Close the triangle by connecting back to the first point
    };

    // Scale and translate (apply zoom and offset)
    for (int i = 0; i < 4; ++i) {
        points[i].x = (points[i].x - 640) * scale + 640 + offset.x;
        points[i].y = (points[i].y - 480) * scale + 480 + offset.y;
    }

    // Define the visible area (viewport) based on scale and offset
    int viewportWidth = (int)(WINDOW_WIDTH * scale);
    int viewportHeight = (int)(WINDOW_HEIGHT * scale);

    // Ensure the viewport is never smaller than the window size (for zooming out)
    viewportWidth = std::max(viewportWidth, WINDOW_WIDTH);
    viewportHeight = std::max(viewportHeight, WINDOW_HEIGHT);

    SDL_Rect viewport = { offset.x, offset.y, viewportWidth, viewportHeight };

    // Set render draw color for the triangle (white color)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    renderInside(renderer, points, scale, viewport);
    
    // Draw the triangle outline (now including the last point to close it)
    SDL_RenderDrawLines(renderer, points, 4);
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Sierpinski Triangle",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    float scale = 1.0f;  // Initial zoom level (1.0 is normal size)
    SDL_Point offset = { 0, 0 };  // Offset for panning

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } 
            else if (e.type == SDL_MOUSEWHEEL) {
                if (e.wheel.y > 0) {
                    // Zoom in
                    scale *= 1.1f;
                }
                else if (e.wheel.y < 0) {
                    // Zoom out
                    scale /= 1.1f;
                }
            }
        }

        // Clear the screen once at the start of each frame
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Render the triangle and apply zoom
        renderZoom(renderer, scale, offset);

        // Present the renderer (this updates the window with what we've drawn)
        SDL_RenderPresent(renderer);

        // Wait for a short time to simulate FPS (for smooth rendering)
        SDL_Delay(16);  // Roughly 60 FPS
    }

    // Clean up and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

