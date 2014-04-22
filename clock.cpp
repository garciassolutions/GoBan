// g++ -Wall `sdl-config --cflags` -c -o clock.o clock.cpp && gcc `sdl-config --libs` -lSDL -lGL -lSDL_ttf -lstdc++ -o clock clock.o
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <string>

class Timer{
	private:
		int startTicks, pauseTicks;
		bool started, paused;
	public:
		Timer();
		int get_ticks();
		void start();
		void stop(); // Pause and stop do the same thing? Stop clears variables pause doesnt?
		void pause();
		void unpause();
		bool is_paused();
		bool is_started();
};

Timer::Timer(){
	startTicks = 0;
	pauseTicks = 0;
	started = paused = false;
}

bool Timer::is_paused(){
	return paused;
}

bool Timer::is_started(){
	return started;
}

void Timer::start(){
	started = true;
	paused = false;
	startTicks = SDL_GetTicks();
}

void Timer::stop(){
	started = paused = false;
}

void Timer::pause(){
	if(started && !paused){
		paused = true;
		pauseTicks = SDL_GetTicks() - startTicks;
	}
}

void Timer::unpause(){
	if(paused){
		paused = false;
		startTicks = SDL_GetTicks() - pauseTicks;
		pauseTicks = 0;
	}
}

int Timer::get_ticks(){
	return started?(paused?pauseTicks:SDL_GetTicks()-startTicks):0;
}

void apply_surface(int, int, SDL_Surface *, SDL_Surface *);

int main(int argc, char *argv[]){
	Timer black_time, white_time;
	int b_start, w_start;

	if(argc != 2) exit(1);
	SDL_Surface *screen, *message, *seconds;
	TTF_Font *font;
	SDL_Color textColor = {255, 255, 255};
	int done = 0;
	SDL_Event event;
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		fprintf(stderr, "Error starting sdl: %s\n", SDL_GetError());
		exit(1);
	}
	screen = SDL_SetVideoMode(500, 500, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
	if(!screen){
		fprintf(stderr, "Unable to create screen: %s\n", SDL_GetError());
		exit(1); 
	}
	if(TTF_Init() == -1){
		fprintf(stderr, "Unable to create screen: %s\n", SDL_GetError());
		exit(1);
	}
	font = TTF_OpenFont(argv[1], 28);
	if(font == NULL){
		fprintf(stderr, "Unable to create screen: %s\n", SDL_GetError());
		exit(1);
	}
	message = TTF_RenderText_Solid(font, "nue 8k* vs. Henry 7k*", textColor);
	if(message == NULL){
		fprintf(stderr, "Unable to create screen: %s\n", SDL_GetError());
		exit(1);
	}
	black_time.start();
	while(!done){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT) done = 1;
			// Check for arrow keys.
			// Left = Back one move
			// Right = Foward one move.
			// Up = Blank board / game start.
			// Down = Last move.
		}
		std::stringstream time;
		SDL_FillRect(screen, NULL, 0x000000);
		time << "Timer: " << black_time.get_ticks() / 1000.f;
		seconds = TTF_RenderText_Solid(font, time.str().c_str(), textColor);
		apply_surface(50, 50, seconds, screen);
		SDL_FreeSurface(seconds);
		SDL_Flip(screen);
	}
	TTF_CloseFont(font);
	atexit(SDL_Quit);
	return 0;
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination){ 
	SDL_Rect offset;
	offset.x = x; 
	offset.y = y; 
	SDL_BlitSurface(source, NULL, destination, &offset);
}
