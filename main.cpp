
#include <iostream>
#include <time.h>

#include "AnimatedPath.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

int main(int argc, char* argv[]){
	
	SDL_Surface* screenSurface = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Window * window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 600, SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Surface * windowSurface = SDL_GetWindowSurface( window );
	bool quit = false;
	SDL_Event e;
	int dx;
	int dy;

	SDL_Renderer* renderer = NULL;
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
	// Entity entity(0,0,400,400,renderer,"ship/ship");


	// SDL_Surface * proto = IMG_Load("box-proto3.png");
	// SDL_Texture * texture = SDL_CreateTextureFromSurface( renderer, proto ); 
	SDL_RenderClear( renderer );
	SDL_Rect rekt;
	rekt.x = 0;
	rekt.y = 0;
	rekt.h = 200;
	rekt.w = 200;
	std::cout << "CREATING ANIMATED PATH" << std::endl;
	AnimatedPath ap("tree3.svg","brush3.png",300,1200,600,0,0,renderer);
	std::cout << "FINISHED CREATING ANIMATED PATH" << std::endl;
	

	SDL_SetRenderTarget(renderer,NULL);
	dx = 0;
	dy = 0;
	while(!quit){
		
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym){
					case SDLK_w:
						dy=-10;
					break;
					case SDLK_s:
						dy=10;
					break;
					case SDLK_a:
						dx=-10;
					break;
					case SDLK_d:
						dx=10;
					break;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				switch(e.key.keysym.sym){
					case SDLK_w:
						dy=0;
					break;
					case SDLK_s:
						dy=0;
					break;
					case SDLK_a:
						dx=0;
					break;
					case SDLK_d:
						dx=0;
					break;
				}
			}
			
		}
		rekt.x+=dx;
		rekt.y+=dy;
		SDL_RenderClear( renderer );

		ap.render();

		SDL_RenderPresent( renderer );

		SDL_Delay(50);
	}

	SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );

	return 0;
}