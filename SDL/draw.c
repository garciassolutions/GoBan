#include <SDL/SDL.h>

void DrawPix(SDL_Surface *display, int x, int y, Uint8 R, Uint8 G, Uint8 B){
	Uint32 col = SDL_MapRGB(display->format, R, G, B);
	switch(display->format->BytesPerPixel){
		case 1:{
			Uint8 *bufp;
			bufp = (Uint8 *)display->pixels + y*display->pitch + x;
			*bufp = col;
		}
		break;
		case 2:{
			Uint16 *bufp;
			bufp = (Uint16 *)display->pixels + y*display->pitch/2 + x;
			*bufp = col;
		}
		break;
		case 3:{
			Uint8 *bufp;
			bufp = (Uint8 *)display->pixels + y*display->pitch + x * 3;
			if(SDL_BYTEORDER == SDL_LIL_ENDIAN){
				bufp[0] = col;
				bufp[1] = col >> 8;
				bufp[2] = col >> 16;
			}
			else{
				bufp[2] = col;
				bufp[1] = col >> 8;
				bufp[0] = col >> 16;
			}
		}
		break;
		case 4:{
			Uint32 *bufp;
			bufp = (Uint32 *)display->pixels + y*display->pitch/4 + x;
			*bufp = col;
		}
		break;
	}
}

void Draw_HLine(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B){
	for(int a=x;a<=(screen->w)-EDGE-SIDEBAR;a++) DrawPix(screen, a, y, R, G, B);
}

void Draw_VLine(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B){
	for(int a=y;a<=(screen->h)-EDGE;a++) DrawPix(screen, x, a, R, G, B);
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination){ 
	SDL_Rect offset;
	offset.x = x; 
	offset.y = y; 
	SDL_BlitSurface(source, NULL, destination, &offset);
}

void Draw_Box(SDL_Surface *screen, int x, int y, int length, int width, int c){
	int a, b;
	for(a=0;a<=width/2;a++){
		for(b=0;b<3;b++){
			DrawPix(screen, x+a, y+(length/2)+b, c, c, c);
			DrawPix(screen, x+a, y-(length/2)-b, c, c, c);
			DrawPix(screen, x-a, y+(length/2)+b, c, c, c);
			DrawPix(screen, x-a, y-(length/2)-b, c, c, c);
		}
	}
	for(a=0;a<=length/2;a++){
		for(b=0;b<3;b++){
			DrawPix(screen, x+(width/2)+b, y+a, c, c, c);
			DrawPix(screen, x-(width/2)-b, y+a, c, c, c);
			DrawPix(screen, x+(width/2)+b, y-a, c, c, c);
			DrawPix(screen, x-(width/2)-b, y-a, c, c, c);
		}
	}
}

void Slock(SDL_Surface *screen){
	if(SDL_MUSTLOCK(screen))
		if(SDL_LockSurface(screen) < 0)
			return;
}

void Sulock(SDL_Surface *screen){
	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
}

void Circle(SDL_Surface *screen, int centerx, int centery, int radius, Uint8 c){
	float d, y, x;
	float step = .5;
	d = 3 - (2 * radius);
	x = 0;
	y = radius;
	while (y >= x && centery-y > 0 && centery+y < screen->h){
		DrawPix(screen, centerx + x,centery + y, c, c, c);
		DrawPix(screen, centerx + y,centery + x, c, c, c);
		DrawPix(screen, centerx - x,centery + y, c, c, c);
		DrawPix(screen, centerx - y,centery + x, c, c, c);
		DrawPix(screen, centerx + x,centery - y, c, c, c);
		DrawPix(screen, centerx + y,centery - x, c, c, c);
		DrawPix(screen, centerx - x,centery - y, c, c, c);
		DrawPix(screen, centerx - y,centery - x, c, c, c);
		if (d < 0) {
			d = d + (4 * x) + 6;
		}
		else {
			d = d + 4 * (x - y) + 10;
			y-=step;
		}
		x+=step;
	}
}

void star_points(int size, SDL_Surface *screen){ // Take the size of the board and the screen to draw on.
	if(size == 9){
		for(int r=0;++r<4;){
			Circle(screen, (H_DIST*2)+EDGE, EDGE+(V_DIST*2), r, 1);
			Circle(screen, (H_DIST*2)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*2), r, 1);
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*4)+EDGE, EDGE+(V_DIST*4), r, 1);
		}
	}
	else if(size == 13){
		for(int r=0;++r<4;){
			Circle(screen, (H_DIST*3)+EDGE, EDGE+(V_DIST*3), r, 1);
			Circle(screen, (H_DIST*3)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*3)+EDGE, EDGE+(V_DIST*9), r, 1);	
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*3), r, 1);
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*9), r, 1);
			Circle(screen, (H_DIST*9)+EDGE, EDGE+(V_DIST*3), r, 1);
			Circle(screen, (H_DIST*9)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*9)+EDGE, EDGE+(V_DIST*9), r, 1);
		}
	}
	else if(size == 19){
		for(int r=0;++r<4;){
			Circle(screen, (H_DIST*3)+EDGE, EDGE+(V_DIST*3), r, 1);
			Circle(screen, (H_DIST*3)+EDGE, EDGE+(V_DIST*9), r, 1);
			Circle(screen, (H_DIST*3)+EDGE, EDGE+(V_DIST*15), r, 1);	
			Circle(screen, (H_DIST*9)+EDGE, EDGE+(V_DIST*3), r, 1);
			Circle(screen, (H_DIST*9)+EDGE, EDGE+(V_DIST*9), r, 1);
			Circle(screen, (H_DIST*9)+EDGE, EDGE+(V_DIST*15), r, 1);
			Circle(screen, (H_DIST*15)+EDGE, EDGE+(V_DIST*3), r, 1);
			Circle(screen, (H_DIST*15)+EDGE, EDGE+(V_DIST*9), r, 1);
			Circle(screen, (H_DIST*15)+EDGE, EDGE+(V_DIST*15), r, 1);	
		}
	}
}


void usage(char *nombre){
	fputs("(~) Three Stone Solutions (c)\n(~) http://threestonesolutions.com\n(~) Written by Anthony J. Garcia\n(~) Go client v1.0\n\n", stderr);
	fprintf(stderr, "Usage is: %s -s 9-19 -b black_name -w white_name -k komi -t main_time -y byoyomi_time [-h handicap] [-a 1]\n", nombre);
	fputs("[!] -s board_size can have a minimum value of nine and a maximum value of nineteen.\n", stderr);
	fputs("[!] -k komi is whites starting score.\n", stderr);
	fputs("[!] -t main_time is the amount of time a player has to play.\n", stderr);
	fputs("[!] -y byoyomi_time is the amount of time a player will recieve after the main time, and every time the byoyomi stone count is played.\n", stderr);
	fputs("[!] -h handicap is the amount of stones black will get too start at the beginning of the game.\n", stderr);
	fputs("[!] the minimum handicap black can recieve is 2 stones.\n", stderr);
	fputs("[!] -w white players name.\n", stderr);
	fputs("[!] -b black players name.\n", stderr);
	fputs("[!] -a autosave, 1 for on, otherwise off.\n", stderr);
	exit(1);
}

void die(char *err){ // Close any open filehandles and sockets?
	fprintf(stderr, "Error: %s\n", err);
	exit(1);
}
