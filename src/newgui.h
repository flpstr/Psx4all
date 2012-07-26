#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

extern SDL_Surface* screen;

SDL_Surface* loadImage(char* filename);
void drawSurface(int x, int y, SDL_Surface* source, SDL_Surface* dest);
SDL_Surface* renderText(SDL_Surface* surface, TTF_Font* font, char* text, SDL_Color &color);
void fillRect(int x, int y, int w, int h, int color);

#endif
