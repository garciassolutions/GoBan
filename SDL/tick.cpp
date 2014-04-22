#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

class Timer{
	private:
		int startTicks, pauseTicks;
		bool started, paused;
	public:
		Time_init();
		void Timer_start();
		void Timer_stop(); // Pause and stop do the same thing? Stop clears variables pause doesnt?
		void Timer_pause();
		void Timer_unpause();
		int Timer_ticks();
		bool Timer_started();
		bool Timer_paused();
}

Timer::Timer_init(){
	startTicks = 0;
	pauseTicks = 0;
	started = paused = false;
}

Timer::Timer_start(){
	started = true;
	paused = false;
	startTicks = SDL_GetTicks();
}

Timer::Timer_stop(){
	started = paused = false;
}

Timer::Timer_pause(){
	if(started && !paused){
		paused = true;
		pauseTicks = SDL_GetTicks() - startTicks;
	}
}

Timer::Timer_unpause(){
	if(paused){
		paused = false;
		startTicks = SDL_GetTicks() - pausedTicks;
		pausedTicks = 0;
	}
}

Timer::Timer_ticks(){
	return started?paused?pausedTicks:SDL_GetTicks()-startTicks:0;
}

int main(int argc, char *argv[]){
	int done = 0;
	SDL_Surface *screen;
	SDL_Event *event;
	while(!done){
		while(SDL_PollEvent(&event)){
			
		}
	}
	return 0;
}
