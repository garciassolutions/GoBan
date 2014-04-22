#ifndef CLOCK_H
#define CLOCK_H
class Timer{
	private:
		int startTicks, pauseTicks, byoyomi;
		bool started, paused;
	public:
		Timer();
		int get_ticks();
		int get_byoyomi();
		void start();
		void stop(); // Pause and stop do the same thing? Stop clears variables pause doesnt?
		int pause();
		void unpause();
		bool is_paused();
		bool is_started();
		void set_byoyomi();
};

Timer::Timer(){
	startTicks = 0;
	pauseTicks = 0;
	byoyomi = 0;
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

int Timer::pause(){ // Return the time to be stored.
	if(started && !paused){
		pauseTicks = SDL_GetTicks() - startTicks; // How long we've been running, minus when we started. = How long between pauses.
		paused = true;
	}
	return pauseTicks;
}

void Timer::unpause(){
	if(!started && !paused){
		started = true;
		startTicks = SDL_GetTicks();
	}
	else if(paused){
		paused = false;
		startTicks = SDL_GetTicks() - pauseTicks;
		pauseTicks = 0;
	}
}

int Timer::get_ticks(){
	return started?(paused?pauseTicks:SDL_GetTicks()-startTicks):0;
}

void Timer::set_byoyomi(){
	byoyomi = 1;
}

int Timer::get_byoyomi(){
	return byoyomi;
}

#endif
