#include <SDL/SDL.h>
#include <cairo.h>
#include <iostream>

#include "Exception.h"
#include "Game.h"

//#define SLOW (0.05)
#define SLOW (1.0)

#define SHOW_FPS

static const size_t FPS = 30;

static bool g_pause = true;
static bool g_iterate = true;

/******************************************************************************
 * handle_sdl_event                                                           *
 ******************************************************************************/
void handle_sdl_event(SDL_Event &ev) {
	switch(ev.type) {
		case SDL_KEYDOWN:
			// Quit when pressing (or releasing) escape key
			if(ev.key.keysym.sym == SDLK_ESCAPE) {
				ev.type = SDL_QUIT;
			} else if(ev.key.keysym.sym == SDLK_i) {
				g_iterate = true;
			} else if(ev.key.keysym.sym == SDLK_SPACE) {
				g_pause = !g_pause;
			}
			break;
	}
}

/******************************************************************************
 * main_loop                                                                  *
 ******************************************************************************/
void main_loop(SDL_Surface *screen, cairo_t *cr, Game &game) {
	SDL_Event ev;
	Uint32 lastTicks = 0;
	const double period = 1.0 / FPS;
	double elapsedTime = 0.0;
	
#ifdef SHOW_FPS
	struct {
		Uint32 nb_frames;
		double time_elapsed;
	} stats;
	stats.nb_frames = 0;
	stats.time_elapsed = 0.0;
#endif
	
	do {
		Uint32 now = SDL_GetTicks();
		
		// Increase elapsedTime
		if(lastTicks != 0) {
			double dt = (now - lastTicks) * 0.001;
			elapsedTime += dt;
#ifdef SHOW_FPS
			stats.time_elapsed += dt;
#endif
		} else {
			elapsedTime = period;
#ifdef SHOW_FPS
			stats.time_elapsed = 0;
#endif
		}
		lastTicks = now;

#ifdef SHOW_FPS
		// If it is time to show FPS, show it
		if(stats.time_elapsed >= 10.0) {
			std::cout << "FPS on last 10s: " << (stats.nb_frames / 10.0) << " (target: " << FPS << ")" << std::endl;
			stats.time_elapsed -= 10.0;
			stats.nb_frames = 0;
		}
#endif
		
		// Handle SDL events
		while(SDL_PollEvent(&ev) == 1) {
			handle_sdl_event(ev);
		}
		
		// If it is time to show the frame, show it
		if(elapsedTime >= period) {
			// Draw the things
			game.draw(cr);
			
			// Show them to screen
			SDL_Flip(screen);
#ifdef SHOW_FPS
			++ stats.nb_frames;
#endif
			
			// And advance to the next period
			if(g_iterate || !g_pause) {
				g_iterate = false;
				game.advance(period * SLOW);
			}
			
			elapsedTime -= period;
		} else {
			// Wait for next time to show the frame
			int ms_to_sleep(static_cast<int>(1000.0 * (period - elapsedTime)));
			SDL_Delay(ms_to_sleep);
		}
	} while(ev.type != SDL_QUIT);
}

/******************************************************************************
 * main                                                                       *
 ******************************************************************************/
int main(int argc, char **argv) {
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		std::cerr << "SDL initialization error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	SDL_Surface *screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if(screen == NULL) {
		std::cerr << "Error while setting SDL video mode: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
	cairo_surface_t *surface = cairo_image_surface_create_for_data(static_cast<unsigned char *>(screen->pixels), CAIRO_FORMAT_ARGB32, 640, 480, 4 * 640);
	cairo_t *cr = cairo_create(surface);
	
	try {
		Game game;
		
		main_loop(screen, cr, game);
	} catch(const Exception &e) {
		std::cout << e.what() << std::endl;
	}
	
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	SDL_Quit();
	
	return 0;
}
// vim: ts=2 sw=2 noet
