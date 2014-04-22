#ifndef DRAW_H
#define DRAW_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "clock.h"
#include "Linked_List.h"

#define BLACK 1
#define WHITE 2
#define CHECKED 4 // For checking empty spaces, and patches of stones.
#define DEAD 8 // Used for marking scored spaces.
#define B_SPACE 16
#define W_SPACE 32
#define MARKED 0x40

#define NAME_LENGTH 25 // The max length of the players name.
#define EDGE 65 // The edge spacing around the board. Plus Cord. space.
#define OUTLINE 15 // Dark outline around the board.
#define STONE_SIZE 20 // The size of the stones.
#define H_DIST 44.0 // Distance between horizontal lines.
#define V_DIST 44.6 // Distance between vertical lines.
#define SIDEBAR 150 // How large the sidebar will be.
#define BYOYOMI_STONES 20 // How many stones have to be played before the timer is reset.

#define Sulock(screen) if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
#define Draw_BLine(screen, x, R, G, B) for(int a=0;a<(screen->h);a++) DrawPix(screen, x, a, R, G, B); // Draw a big line.
#define Draw_VLine(screen, x, y, R, G, B) for(int a=y;a<=(screen->h)-EDGE;a++) DrawPix(screen, x, a, R, G, B);
#define Draw_HLine(screen, x, y, R, G, B) for(int a=x;a<=(screen->w)-EDGE-SIDEBAR;a++) DrawPix(screen, a, y, R, G, B);
#define Draw_Sideline(screen, y, R, G, B) for(int a=(screen->w)-SIDEBAR-1;a<(screen->w);a++) DrawPix(screen, a, y, R, G, B);

SDL_Rect clips[40]; // Clips for stones will be used to remove, and display new stones properly,

bool CORDS = true;

unsigned int sidebar_start = 0;
unsigned int board_start = 0;
unsigned int board_size_x = 0;
unsigned int board_size_y = 0;

extern char *board;
extern Linked_List moves;
extern char w_name[], b_name[];

const char translation[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'};

bool scoring = false; // Flag if the game is being scored.
bool saving = false; // Save filenames as: w_b_time.sgf
SDL_Color textColor = {255, 255, 255}, bgcolor={0xdc,0xb3,0x5c}, foreground={0x21, 0x5e, 0x21}, offsetcolor={0x45, 0xbd, 0x00}, buttoncolor={0xcd,0xcd,0xcd};
SDL_Event event;
SDL_Surface *p1, *p2, *screen, *seconds, *pss, *p_board;

Timer b_time, w_time; // Black and whites time.

TTF_Font *font; // The font that will be in use.

float w_score = 0.5, w_total = 0; // white's score, whites total score.
short int handicap = 0, y_lines, b_score, b_total=0, color = 1, b_stones, w_stones, byoyomi_time=0, main_time=0, passed = 0, ko_stone, kox=-1, koy=-1, lost = 0, lastx = -1, lasty = -1;

// Draw a pixel onto the screen
void DrawPix(SDL_Surface *display, int x, int y, Uint8 R, Uint8 G, Uint8 B){
	Uint32 col = SDL_MapRGB(display->format, R, G, B);
	switch(display->format->BytesPerPixel){
		case 1:{
			Uint8 *bufp;
			bufp = (Uint8 *)display->pixels+y*display->pitch+x;
			*bufp = col;
		}
		break;
		case 2:{
			Uint16 *bufp;
			bufp = (Uint16 *)display->pixels+y*display->pitch/2+x;
			*bufp = col;
		}
		break;
		case 3:{
			Uint8 *bufp;
			bufp = (Uint8 *)display->pixels+y*display->pitch+x * 3;
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
			bufp = (Uint32 *)display->pixels+y*display->pitch/4+x;
			*bufp = col;
		}
		break;
	}
}

// Apply the new surface to the screen.
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination){ 
	SDL_Rect offset;
	offset.x = x; 
	offset.y = y; 
	SDL_BlitSurface(source, NULL, destination, &offset);
}

// Draw a word onto the screen at a given position.
void Draw_Word(SDL_Surface *screen, int x, int y, char *letters){
	pss = TTF_RenderText_Shaded(font, letters, textColor, bgcolor);
	apply_surface(x, y, pss, screen);
	SDL_FreeSurface(pss);
}

// Draw the quit diagram.
void Draw_quit(SDL_Surface *screen, int x, int y){ // Set a QUIT flag for Map_Click()?
	// Display a discolored box with [Yes] [No] also in discolored boxs.
	pss = TTF_RenderText_Shaded(font, "Are you sure you want to quit?", textColor, bgcolor);
	SDL_FreeSurface(pss);
	pss = TTF_RenderText_Shaded(font, "Yes", textColor, bgcolor);
	SDL_FreeSurface(pss);
	pss = TTF_RenderText_Shaded(font, "No", textColor, bgcolor);
	SDL_Flip(screen);
	// After this function is called, it should be put in a loop to wait for the user to click within the dialog box and select an answer.
}

// Draw a box onto the screen.
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

// Lock the screen so it can't be drawn and swapped.
void Slock(SDL_Surface *screen){
	if(SDL_MUSTLOCK(screen))
		if(SDL_LockSurface(screen) < 0)
			return;
}

// Draw a circle onto the screen.
void Circle(SDL_Surface *screen, int centerx, int centery, int radius, Uint8 c){
	float d, y, x;
	float step = .5;
	d = 3 - (2 * radius);
	x = 0;
	y = radius;
	while(y >= x && centery-y > 0 && centery+y < screen->h){
		DrawPix(screen, centerx+x,centery+y, c, c, c);
		DrawPix(screen, centerx+y,centery+x, c, c, c);
		DrawPix(screen, centerx - x,centery+y, c, c, c);
		DrawPix(screen, centerx - y,centery+x, c, c, c);
		DrawPix(screen, centerx+x,centery - y, c, c, c);
		DrawPix(screen, centerx+y,centery - x, c, c, c);
		DrawPix(screen, centerx - x,centery - y, c, c, c);
		DrawPix(screen, centerx - y,centery - x, c, c, c);
		if(d < 0){
			d = d+(4 * x)+6;
		}
		else{
			d = d +4 * (x - y)+10;
			y-=step;
		}
		x+=step;
	}
}

// Draw the starpoints onto the board.
void star_points(int size, SDL_Surface *screen){ // Take the size of the board and the screen to draw on.
	if(size == 9){ // If it's a 9x9 board.
		for(int r=0;++r<4;){
			Circle(screen, (H_DIST*2)+EDGE, EDGE+(V_DIST*2), r, 1);
			Circle(screen, (H_DIST*2)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*2), r, 1);
			Circle(screen, (H_DIST*6)+EDGE, EDGE+(V_DIST*6), r, 1);
			Circle(screen, (H_DIST*4)+EDGE, EDGE+(V_DIST*4), r, 1);
		}
	}
	else if(size == 13){ // If it's a 13x13 board.
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
	else if(size == 19){ // If it's a 19x19 board.
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

// Display the usage if arguements are incorrect or not given properly.
void usage(char *nombre){
	fputs("(~) Three Stone Solutions (c)\n(~) http://threestonesolutions.com\n(~) Written by Anthony J. Garcia\n(~) Go client v1.0\n\n", stderr);
	fprintf(stderr, "Usage is: %s -s 2-19 -b black_name -w white_name -k komi -t main_time -y byoyomi_time [-h handicap] [-a 1]\n", nombre);
	fputs("[!] -s board_size can have a minimum value of nine and a maximum value of nineteen.\n", stderr);
	fputs("[!] -k komi is whites starting score.\n", stderr);
	fputs("[!] -t main_time is the amount of time a player has to play.\n", stderr);
	fputs("[!] -y byoyomi_time is the amount of time a player will recieve after the main time, and every time the byoyomi stone count is played.\n", stderr);
	fputs("[!] -h handicap is the amount of stones black will get too start at the beginning of the game.\n", stderr);
	fputs("[!] the minimum handicap black can recieve is 2 stones.\n", stderr);
	fputs("[!] -w white players name.\n", stderr);
	fputs("[!] -b black players name.\n", stderr);
	fputs("[!] -o disable coordinates from being displayed.\n", stderr);
	exit(1);
}

// Display the error and clean up the program.
void die(char *err){
	fprintf(stderr, "Error: %s\n", err);
	// Clean up???
	exit(1);
}

// Display a losing message.
void Lost(SDL_Surface *screen){ 
	// Check time and Lost() if <= 0 after byoyomi or == byoyomi_stones and <= 0 time
	// Apply a surface and wait for user to click in it to close main window.
	SDL_Color tmp = {0xff, 0xff, 0xff};
	SDL_Color tmp2 = {0x00, 0x00, 0x00};
	
	lost = 1;
	if(color^0x3){
		b_time.pause();
	}
	else{
		w_time.pause();
	}
	seconds = TTF_RenderText_Shaded(font, "You have ran out of time and lost.", tmp2, tmp);
	apply_surface(0, 0, seconds, screen);
	seconds = TTF_RenderText_Shaded(font, "Please click here to close this board.", tmp2, tmp);
	apply_surface(0, 30, seconds, screen);
	SDL_FreeSurface(seconds);
	SDL_Flip(screen);
}

// Display players time onto the screen.
void Draw_Time(SDL_Surface *screen, int x, int y, int c){
	std::stringstream clock;
	char tmp[15];
	int secs, minutes;
	float time;
	if(!c){ // If we're drawing blacks time.
		time = b_time.get_ticks()/1000.f;
		if(!b_time.get_byoyomi()){
			secs = int(main_time - int(time))%60;
			minutes = int((main_time - int(time))-secs)/60;
			// if((main_time - int(time) <= 0 && b_stones == BYOYOMI_STONES) Lost(screen); # fix this.
			if(main_time - int(time) <= 0){
				b_time.set_byoyomi();
				b_time.start();
			}
		}
		else{
			secs = int(byoyomi_time - time)%60;
			minutes = int(byoyomi_time - time - secs)/60;
			if(byoyomi_time - time <= 0) Lost(screen); // Check that time hasn't expired.
		}
		sprintf(tmp, " %02d:%02d : %02d ", minutes, secs, b_stones);
	}
	else{ // Else we're drawing whites time.
		time = w_time.get_ticks()/1000.f;
		if(!w_time.get_byoyomi()){
			secs = (main_time - int(time))%60;
			minutes = int(main_time-int(time)-secs)/60;
			//if((main_time - int(time)) <= 0) Lost(screen); // Also check that stones have been played.
			if((main_time - time) <= 0){
				w_time.set_byoyomi();
				w_time.start();
			}
		}
		else{
			secs = int(byoyomi_time - time)%60;
			minutes = int((byoyomi_time - time - secs)/60);
			if(byoyomi_time - time <= 0) Lost(screen); // Check that the time hasn't expired.
		}
		sprintf(tmp, "%02d:%02d : %02d", minutes, secs, w_stones); // Put the time in a number of spaces.

	}
	clock << tmp;
	seconds = TTF_RenderText_Shaded(font, clock.str().c_str(), textColor, bgcolor);
	apply_surface(((H_DIST*(y_lines-1))+(EDGE<<1))+x, EDGE+y, seconds, screen);
	SDL_FreeSurface(seconds);
	SDL_Flip(screen);
}

// If either player resigns display a message onto the screen.
void resign(SDL_Surface *screen){
	SDL_Color tmp = {0xff, 0xff, 0xff};
	SDL_Color tmp2 = {0xdc, 0xb3, 0x5c};
	lost = 1;
	if(color == WHITE){
		seconds = TTF_RenderText_Shaded(font, "       White has resigned and lost.    ", tmp2, tmp);
		w_time.stop();
	}
	else{
		seconds = TTF_RenderText_Shaded(font, "       Black has resigned and lost.    ", tmp2, tmp);
		b_time.stop();
	}
	int midx, midy;
	midx = int((screen->w-(EDGE<<1))/2);
	midy = int((screen->h-(EDGE<<1))/2);
	apply_surface(midx-200, midy-15, seconds, screen); // PUT THIS IN THE MIDDLE OF THE SCREEN
	seconds = TTF_RenderText_Shaded(font, "Please click anywhere close this board", tmp2, tmp);
	apply_surface(midx-200, midy+15, seconds, screen);
	SDL_FreeSurface(seconds);
	SDL_Flip(screen);
}

void Draw_Score(SDL_Surface *screen){
	SDL_Color tmp = {0xff, 0xff, 0xff};
	SDL_Color tmp2 = {0xdc, 0xb3, 0x5c};
	int midx = int((screen->w-(EDGE<<1))/2);
	int midy = int((screen->h-(EDGE<<1))/2);
	char total[128] = {0}; // So big? Dream big i guess...?
	snprintf(total, sizeof(total)-1, "%s : %.02f | %s : %.02f", w_name, w_score, b_name, b_score);
	seconds = TTF_RenderText_Shaded(font, total, tmp2, tmp);
	apply_surface(midx-200, midy-15, seconds, screen); // PUT THIS IN THE MIDDLE OF THE SCREEN
	seconds = TTF_RenderText_Shaded(font, b_score>w_score?"Black has won!":"White has won!", tmp2, tmp);
	apply_surface(midx-200, midy+15, seconds, screen); // PUT THIS IN THE MIDDLE OF THE SCREEN
	SDL_FreeSurface(seconds);
	SDL_Flip(screen);
}

// Draw the cords. around the board
void Draw_Cords(SDL_Surface *screen, int size){ // Draw the cords. onto the board.
	SDL_Color tmp = {0x00, 0x00, 0x00};
	int r, tx=EDGE-10, ty=EDGE-10;
	char small[4] = {0};
	for(r=0;r<size;r++){
		snprintf(small, sizeof(small)-1, "%d", r+1);
		apply_surface(tx, 15, TTF_RenderText_Solid(font, small, tmp), screen); // Draw the numbers.
		tx += H_DIST;
		snprintf(small, sizeof(small)-1, "%c", translation[r]);
		apply_surface(7, ty, TTF_RenderText_Solid(font, small, tmp), screen); // Draw the letters.
		ty += V_DIST;
	}
}

// Draw the board.
void Draw_Board(SDL_Surface *screen){ // This function displays the board, up to the last stone played.
	char x[6];
	int r;
	SDL_FillRect(screen, NULL, 0xdcb35c); // Draw the background color.
	
	// Display the turn and move.
	// Display:
	// Move 0 : Black to play.
	// Move XX: W/B played XXXX B/W to play
	
	for(r=0;r<y_lines;r++){ // ADD ALL THIS TO A TMP SURFACE
		Draw_HLine(screen, EDGE, EDGE+(r*V_DIST), 0, 0, 0); // Draw the horizontal lines.
		Draw_VLine(screen, EDGE+(r*H_DIST),EDGE, 0, 0, 0); // Draw the verticle lines.
	}
	Draw_BLine(screen,((H_DIST*(y_lines-1))+(EDGE<<1))-2, 0, 0, 0); // Draw the sidebar.

	if(CORDS) Draw_Cords(screen, y_lines); // Draw the cords on the board.
	
	// Draw black stones above blacks name.
	for(r=1;r<=STONE_SIZE;r++) Circle(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+(SIDEBAR>>1), EDGE-25, r, 0);
	apply_surface(((H_DIST*(y_lines-1))+(EDGE<<1))+15, EDGE, p1, screen); // B players name.
	snprintf(x, sizeof(x)-1, "%4d.0", b_score); 
	apply_surface(((H_DIST*(y_lines-1))+(EDGE<<1))+75, EDGE, TTF_RenderText_Solid(font, x, textColor), screen); // Add B score.
	Draw_Sideline(screen, EDGE+80, 0, 0, 0); // Draw the line under b's name

	// Draw white stones above whites name.
	for(r=1;r<=STONE_SIZE;r++) Circle(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+(SIDEBAR>>1), EDGE+105, r, 255);
	apply_surface(((H_DIST*(y_lines-1))+(EDGE<<1))+15, EDGE+130, p2, screen); // W players name.
	snprintf(x, sizeof(x)-1, "%4.01f", w_score);
	apply_surface(((H_DIST*(y_lines-1))+(EDGE<<1))+75, EDGE+130, TTF_RenderText_Solid(font, x, textColor), screen); // Add W score
	Draw_Sideline(screen, EDGE+210, 0, 0, 0); // Draw the line under w's name

// MAKE THE ABOVE A TEMPLATE, AND JUST DRAW ON IT!
// use SDL_BlitSurface() when removing, or adding stones.
// much easier then redrawing circles over, and over, and over again...

	if(scoring){ // If the board is being scored, display "Done" in place of "Pass". Also change "Undo" to "Reset"
		Draw_Word(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+35, EDGE+215, "Done");
		Draw_Word(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+35, EDGE+245, "Reset");
	}
	else{
		Draw_Word(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+35, EDGE+215, "Pass");
		Draw_Word(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+35, EDGE+245, "Undo");
	}
	Draw_Word(screen, ((H_DIST*(y_lines-1))+(EDGE<<1))+35, EDGE+275, "Resign");
	Draw_Sideline(screen, EDGE+242, 0, 0, 0);
	Draw_Sideline(screen, EDGE+272, 0, 0, 0);
	Draw_Sideline(screen, EDGE+304, 0, 0, 0);
	star_points(y_lines, screen);

	for(int a=0;a<y_lines;a++) // Redraw the stones when needed.
		for(int b=0;b<y_lines;b++)
			if(board[b*y_lines+a] == BLACK || board[b*y_lines+a] == WHITE)
				for(r=1;r<=STONE_SIZE;r++) Circle(screen, EDGE+(a*H_DIST), EDGE+(b*V_DIST), r, board[b*y_lines+a]==BLACK?0:255);
				
			else if(scoring && board[b*y_lines+a]){ // Only need to check this when scoring, maybe its a captured stone
				if(board[b*y_lines+a] == W_SPACE || board[b*y_lines+a] == B_SPACE)
					for(r=1;r<=STONE_SIZE/3;r++)
						Circle(screen, EDGE+(a*H_DIST), EDGE+(b*V_DIST), r, board[b*y_lines+a]==B_SPACE?255:0);
				else if(board[b*y_lines+a]&DEAD){ // The stone was killed
					for(r=1;r<=STONE_SIZE;r+=2) Circle(screen, EDGE+(a*H_DIST), EDGE+(b*V_DIST), r, board[b*y_lines+a]==(WHITE|DEAD)?0:255);
				}
			}
	long long last_move = moves.get_last(); // 0xLSTXLSTY
	lastx = last_move>>8; // Shift over the long long to get the top 4 bytes.
	lasty = last_move^(lastx<<8); // XOR the value of lastx which are the high bytes to the number to retrieve the lasty.
	if(last_move != -2 || last_move != -1)	for(r=9;r<=11;r++) Circle(screen, EDGE+(lastx*H_DIST), EDGE+(lasty*V_DIST), r, color==BLACK?0:255);
	SDL_Flip(screen); // Apply to the screen.
}

void set_handicap(){
	/*
	 * set_handicap() will set the correct handicap onto the board.
	 * It does this simply by mapping the board out, and setting the stones in the correct position. (On the star points.)
	 */
	if(y_lines == 9){
		if(handicap == 2){
			board[(y_lines*2)+2] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;
		}
		else if(handicap == 3){
			board[(y_lines*2)+2] ^= BLACK;
			board[(y_lines*2)+6] ^= BLACK;
			board[(y_lines*6)+2] ^= BLACK;
		}
		else if(handicap == 4){
			board[(y_lines*2)+2] ^= BLACK;
			board[(y_lines*2)+6] ^= BLACK;
			board[(y_lines*6)+2] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;
		}
		else if(handicap == 5){
			board[(y_lines*2)+2] ^= BLACK;
			board[(y_lines*2)+6] ^= BLACK;
			board[(y_lines*4)+4] ^= BLACK;
			board[(y_lines*6)+2] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;			
		}
	}
	else if(y_lines == 13){
		if(handicap == 2){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
		}
		else if(handicap == 3){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
		}
		else if(handicap == 4){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
		}
		else if(handicap == 5){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;			
		}
		else if(handicap == 6){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*6)+3] ^= BLACK;
			board[(y_lines*6)+9] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;					
		}
		else if(handicap == 7){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*6)+3] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;
			board[(y_lines*6)+9] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;							
		}
		else if(handicap == 8){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+6] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*6)+3] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;
			board[(y_lines*6)+9] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+6] ^= BLACK;
		}
		else if(handicap == 9){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+6] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*6)+3] ^= BLACK;
			board[(y_lines*6)+6] ^= BLACK;
			board[(y_lines*6)+9] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+6] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
		}	
	}
	else if(y_lines == 19){
		if(handicap == 2){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*15)+15] ^= BLACK;
		}
		else if(handicap == 3){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
		}
		else if(handicap == 4){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
			board[(y_lines*15)+15] ^= BLACK;
		}
		else if(handicap == 5){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
			board[(y_lines*15)+15] ^= BLACK;
		}
		else if(handicap == 6){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+15] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
			board[(y_lines*15)+15] ^= BLACK;
		}
		else if(handicap == 7){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
			board[(y_lines*9)+15] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
			board[(y_lines*15)+15] ^= BLACK;							
		}
		else if(handicap == 8){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
			board[(y_lines*9)+15] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
			board[(y_lines*15)+9] ^= BLACK;
		}
		else if(handicap == 9){
			board[(y_lines*3)+3] ^= BLACK;
			board[(y_lines*3)+9] ^= BLACK;
			board[(y_lines*3)+15] ^= BLACK;
			board[(y_lines*9)+3] ^= BLACK;
			board[(y_lines*9)+9] ^= BLACK;
			board[(y_lines*9)+15] ^= BLACK;
			board[(y_lines*15)+3] ^= BLACK;
			board[(y_lines*15)+9] ^= BLACK;
			board[(y_lines*15)+15] ^= BLACK;
		}
	}
}

#endif
