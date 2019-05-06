
#include <iostream>
#include <time.h>

#include "AnimatedPath.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

// int main(int argc, char* argv[]){
// 	SDL_Surface* screenSurface = NULL;
// 	SDL_Init(SDL_INIT_EVERYTHING);
// 	SDL_Window * window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 600, SDL_WINDOW_ALLOW_HIGHDPI);
// 	SDL_Renderer * renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
// 	SDL_Surface * windowSurface = SDL_GetWindowSurface( window );
// 	SDL_Texture * texture = IMG_LoadTexture(renderer,"brush2.png");
// 	SDL_Texture * texture2 = IMG_LoadTexture(renderer,"brush2.png");
// 	SDL_SetTextureColorMod(texture,255,0,0);
	
// 	// SDL_Texture * texture = SDL_CreateTextureFromSurface( renderer, surface);
// 	SDL_Rect rekt;
// 	rekt.y = 300;
// 	rekt.h = 50;
// 	rekt.w = 50;
// 	// 0) RESET RENDERER IF NEEDED.
// 	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
// 	SDL_RenderClear( renderer );
// 	// 1) CREATE SUM TEXTURE OR CANVAS
// 	SDL_Texture* image_sum = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1200, 600);
// 	// 2) REASSIGN RENDER TARGET
// 	SDL_SetRenderTarget(renderer, image_sum);
// 	// PROCEDURE:
// 	// 3) CALL RENDERCOPY(RENDERER,STAMP,SOURCERECT,DESTRECT)
// 	// 4) RESET RENDER TARGET
// 	SDL_RenderCopy(renderer, texture, NULL, NULL);
// 	SDL_SetRenderTarget(renderer, NULL);
// 	SDL_Event e;
// 	SDL_RenderCopy( renderer, image_sum, NULL,NULL );
// 	SDL_RenderPresent( renderer );
// 	bool quit = false;
// 	while(!quit){
// 		while( SDL_PollEvent( &e ) != 0 )
// 		{
// 			//User requests quit
// 			if( e.type == SDL_QUIT )
// 			{
// 				quit = true;
// 			}
// 		}
// 	}
// 	SDL_DestroyRenderer( renderer );
//     SDL_DestroyWindow( window );
// }


int main(int argc, char* argv[]){
	// Timer

	// clock_t start;

	SDL_Surface* screenSurface = NULL;
	// SDL_Surface* sprite = SDL_DisplayFormat(SDL_LoadBMP("blight.bmp"));
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Window * window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 600, SDL_WINDOW_ALLOW_HIGHDPI);
	// SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface * windowSurface = SDL_GetWindowSurface( window );
	// SDL_Delay(3000);
	std::cout << "hello world" << std::endl;
	bool quit = false;
	SDL_Event e;
	int dx;
	int dy;
	// test 1: IMG Loading and rendering
	// SDL_Surface * proto =IMG_Load("box-proto2.png");
	// SDL_Rect r;
	// r.x=0;
	// r.y=0;
	// r.h=100;
	// r.w=100;
	// SDL_BlitSurface(proto,NULL,windowSurface,&r);
	// r.x=100;
	// r.y=0;
	// r.h=100;
	// r.w=100;
	// SDL_BlitSurface(proto,NULL,windowSurface,&r);
	// r.x=50;
	// r.y=25;
	// r.h=100;
	// r.w=100;
	// SDL_BlitSurface(proto,NULL,windowSurface,&r);
	// End test 1
	// Background background(1200,600,200,200,windowSurface,"box-proto3.png");
	std::cout << "Background loaded" << std::endl;
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
	// SDL_RenderCopy( renderer, texture, NULL, &rekt );
	// SDL_RenderPresent( renderer );

	// SDL_Surface * shipSurfaces[8];
	// SDL_Texture * shipTextures[8];
	// std::string fn = "ship/ship";
	// for(int i =0;i<8;i++)
	// {
	// 	fn = "ship/ship";
	// 	fn +=char(int('0')+i);
	// 	fn +=".png";
	// 	// open a file in the format: ship/ship[NUMBER].png
	// 	shipSurfaces[i] = IMG_Load(fn.c_str());
	// 	shipTextures[i] = SDL_CreateTextureFromSurface( renderer, shipSurfaces[i] ); 
	// }
	// int iter = 0;

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
		// entity.move(dx,dy);
		// entity.render();
		ap.render();
		// SDL_RenderCopy( renderer, shipTextures[iter], NULL, &rekt );
		// iter = (iter+1)%8;
		SDL_RenderPresent( renderer );
		// SDL_UpdateWindowSurface( window );
		// std::cout << "Rendering Background" << std::endl;
		// background.render();
		// std::cout << "Rendered Background" << std::endl;
		// SDL_BlitSurface( proto, NULL, windowSurface, NULL );
		SDL_Delay(50);
	}
	// std::cout << count << std::endl;
	//std::cin >> x;
	SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
	// SDL_DestroyTexture( texture );
	return 0;
}