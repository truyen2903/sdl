#include "include/SDL.h"
#include "include/SDL_ttf.h"
#include <iostream>
#undef main

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Tạo cửa sổ SDL
    SDL_Window* window = SDL_CreateWindow("SDL Web Blog Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu nền trắng
        SDL_RenderClear(renderer);

        // Vẽ nút
        SDL_Rect buttonRect = { 300, 250, 200, 50 }; // Vị trí và kích thước của nút
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255); // Màu xanh dương
        SDL_RenderFillRect(renderer, &buttonRect);

        // Vẽ chữ lên nút
        SDL_Color textColor = { 255, 255, 255 }; // Màu trắng
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Click me!", textColor); // Tạo surface văn bản
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); // Tạo texture từ surface
        SDL_Rect textRect = { 330, 265, 140, 20 }; // Vị trí và kích thước văn bản
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect); // Vẽ văn bản lên renderer
        SDL_FreeSurface(textSurface); // Giải phóng surface
        SDL_DestroyTexture(textTexture); // Giải phóng texture

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // Giải phóng tài nguyên
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}