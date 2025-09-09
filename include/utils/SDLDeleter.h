#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

struct SDLDeleter {
    void operator()(SDL_Texture* texture) const {
        if (texture) SDL_DestroyTexture(texture);
    }
    
    void operator()(SDL_Surface* surface) const {
        if (surface) SDL_DestroySurface(surface);
    }
    
    void operator()(SDL_Renderer* renderer) const {
        if (renderer) SDL_DestroyRenderer(renderer);
    }
    
    void operator()(SDL_Window* window) const {
        if (window) SDL_DestroyWindow(window);
    }
    
    void operator()(TTF_Font* font) const {
        if (font) TTF_CloseFont(font);
    }
};

// Type aliases for smart pointers with custom deleters
using unique_texture = std::unique_ptr<SDL_Texture, SDLDeleter>;
using unique_surface = std::unique_ptr<SDL_Surface, SDLDeleter>;
using unique_renderer = std::unique_ptr<SDL_Renderer, SDLDeleter>;
using unique_window = std::unique_ptr<SDL_Window, SDLDeleter>;
using unique_font = std::unique_ptr<TTF_Font, SDLDeleter>;

using shared_texture = std::shared_ptr<SDL_Texture>;
using shared_surface = std::shared_ptr<SDL_Surface>;
using shared_font = std::shared_ptr<TTF_Font>;
