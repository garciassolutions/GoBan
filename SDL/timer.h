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
