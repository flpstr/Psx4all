SDL_Surface *screen = NULL;

SDL_Surface* loadImage(char* filename)
{
    SDL_Surface* temp = IMG_Load(filename);
	if (temp == NULL)
		return NULL;

	SDL_Surface* ret = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    return ret;
}