/* Goban v1.0
// Written by Anthony J. Garcia
// (c) Three Stone Solutions

// g++ -Wall `sdl-config --cflags` -c -o board.o board.cpp && gcc `sdl-config --libs` -lSDL -lGL -lSDL_ttf -lstdc++ -o board board.o
// g++46 -Wall `sdl-config --cflags` -c -o board.o board.cpp && gcc46 `sdl-config --libs` -lSDL -lGL -lSDL_ttf -lstdc++ -o board board.o


// Mapped out 8 bits (1 byte), will allow the program to flow smoothly.
// 0x00 0000 0000 - Empty Space
// 0x01 0000 0001 - Black Stone
// 0x02 0000 0010 - White Stone
// 0x04 0000 0100 - Checked flag.
// 0x05 0000 0101 - Checked Black stone.
// 0x06 0000 0110 - Checked White stone.
// 0x08 0000 1000 - Dead flag.
// 0x09 0000 1001 - Dead Black stone. A point for White.
// 0x0a 0000 1010 - Dead White stone. A point for Black.
// 0x10 0001 0000 - Black enclosed space. A point for Black.
// 0x20 0010 0000 - White enclosed space. A point for White.
// 0x40 0100 0000 - Marked flag, enabled when checking liberties.

/* TO FIX!!!
	- KO square in corners when not a KO (taking 2+ stones) - Look at KO rule - 1 lib, just took a stone - NOT IN CORNER - lib being check after removing stone?
	- Resign/Lose/Win/Timeout/Server timeout message screen.
	- Save premade board sizes, without player names. Easier to BLITZ!
	- Scoring function when game isn't fully over fucks up.
	- W time doesn't update until a event is triggered.
*/

#define fill_last(x, y) if(board[(y*y_lines)+x]) for(int a=0;a<=STONE_SIZE;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, (color==BLACK)?255:0);

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include "draw.h"

const int FRAMES_PER_SECOND = 20; // Set the fps.

unsigned int Check_Lib(short, short, int); // Check to see if the space has any liberties.
unsigned int Fix_Space(short, short, short); // Return a chunk of stones (from an undo)
int undo(); // Undo a move.
short enclosed(short, short);
void Mark_Stone(short, short); 
void Draw_Board(SDL_Surface *); // Display the board.
void Draw_Word(SDL_Surface *, int, int, char *); // Draw a word onto the screen at a given position (x, y).
void Map_Click(SDL_Surface *, int, int); // Board handler, maps out the users clicks.
int pass(); // User passes.
void score(); // Used for scoring the game.
void reset_board();

bool DBL_CLICK = false; // Double click enabled flag.

char *board; // One byte of data.
Linked_List moves; // The move list.
unsigned short done = 0; // Is the game over?
char w_name[NAME_LENGTH] = "", b_name[NAME_LENGTH] = "";
	
// Variables so calculations don't need to be repeated.
extern bool CORDS; // Cords to be displayed or not.
extern unsigned int sidebar_start;
extern unsigned int board_start;
extern unsigned int board_size_x;
extern unsigned int board_size_y;

// USE ACTION FLAG FOR POP RESIGNS, LEAVES, UNDO REQUESTS
int main(int argc, char *argv[]){
	int c;

	while((c=getopt(argc, argv, "c:s:b:w:k:t:y:h:o")) != -1) // Check the flags.
		switch(c){
			case 'b': // Black's name.
				strncpy(b_name, optarg, NAME_LENGTH-1);
				break;
			case 'c': 
				DBL_CLICK = true; // Enable double click // antislip mouse
				break;
			case 's':
				y_lines = atoi(optarg); // Check that a number was given?
				break;
			case 'w': // White's name.
				strncpy(w_name, optarg, NAME_LENGTH-1);
				break;
			case 'k': // Set the komi.
				w_score = atof(optarg); // Check a float was given?
				break;
			case 't': // Set the main time (in minutes).
				main_time = atoi(optarg)*60;
				break;
			case 'y': // Set byoyomi time (in minutes).
				byoyomi_time = atoi(optarg)*60; // Make sure time will be within an unsigned short
				break;
			case 'h': // Set the handicap.
				handicap = atoi(optarg);
				break;
			case 'o': // Disable coordinates. on the board
				CORDS = false;
				break;
			case '?':
				if(optopt == 's' || optopt == 'b' || optopt == 'w' || optopt == 'k' || optopt == 't' || optopt == 'y' || optopt == 'h') fprintf(stderr, "[!] -%c flag requires arguements.\n", optopt);
				else fprintf(stderr, "[!] Unknown option: -%c\n", optopt);
			default: usage(argv[0]); // Display usage.
		}
	if(!w_name[0] || !b_name[0] || !main_time || !byoyomi_time) usage(argv[0]); // Check that time and player names were given.
	if(y_lines != 9 && y_lines != 13 && y_lines > 19) usage(argv[0]); // Check board size.
	if(handicap < 0 || handicap > 9) usage(argv[0]); // Check max handicap size.

	char *buff = new char[(NAME_LENGTH<<1)+15]; // Allocate space for the names and ' (b) vs. (w) ' IF YOU MAKE NAME_LENGTH TOO BIG, O WELL!
	
	board = new char[y_lines*y_lines]; // Create space for the board.
	
	sidebar_start = (H_DIST*(y_lines-1)+(EDGE<<1)); // The beginning of the sidebar.
	
	board_size_x = (H_DIST*(y_lines-1)+(EDGE<<1))+SIDEBAR; // Size of the board.
	board_size_y = (V_DIST*(y_lines-1))+(EDGE<<1); // Size of the board.
	
	snprintf(buff, (NAME_LENGTH<<1)+14, "%s (b) vs. %s (w)", b_name, w_name); // Make the title.
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0) die(SDL_GetError()); // Die if SDL couldn't start.
	screen = SDL_SetVideoMode(board_size_x, board_size_y, 32, SDL_HWSURFACE|SDL_DOUBLEBUF); // Make the main surface to draw on.

	if(!screen) die(SDL_GetError()); // Error making the screen.
	if(TTF_Init() < 0) die(SDL_GetError()); // Try to enable ttf.
	if(!(font = TTF_OpenFont("Candice.ttf", 28))) die(SDL_GetError()); // Try to open the font.
  // open another font for map coords?

	p1 = TTF_RenderText_Shaded(font, b_name, textColor, bgcolor); // Set B players name.
	p2 = TTF_RenderText_Shaded(font, w_name, textColor, bgcolor); // Set W players name.

	if(!p1 || !p2) die(SDL_GetError()); // Die if the players names could not be drawn.

	for(int a=0;a<y_lines;a++)
		for(int b=0;b<y_lines;b++) board[b*y_lines+a] ^= board[b*y_lines+a]; // Clear off the board for the new game.

	SDL_WM_SetCaption(buff, "Go client v1.0");
	set_handicap(); // Set handicap before drawing the board.
	Draw_Board(screen); // Display the changes.

	b_stones = w_stones = BYOYOMI_STONES; // Set the number of stones each player must play before recieving more time.

	if(!handicap) b_time.start(); // Start blacks timer.
	else{ // There was a handicap set, so white plays first.
		color ^= 2;
		w_time.start();
	}
	while(!done){
		while(SDL_PollEvent(&event)){ // Wait for the user to do something.
			if(event.type == SDL_QUIT) done = 1; // If the user closes the board.
			// Ask the user "Are you sure you want to quit?" ???
			
			if(event.type == SDL_MOUSEBUTTONDOWN){ // The user clicked the board.
				if(event.button.button == SDL_BUTTON_LEFT){ // Left button click.
					Slock(screen); // Lock the screen for drawing.
					
					printf("x %.02d : y %.02d\n", event.button.x, event.button.y); // For debugging!
					
					if(scoring){ // If the game is being scored, remove the stones that got clicked on.
						if(event.button.x >= sidebar_start+35 &&
							event.button.x <= sidebar_start+130 &&
							event.button.y >= EDGE+201 && event.button.y <= EDGE+241){
							// If the user clicks "Done"
							// Calculate the score, and display the difference on the screen.
							done++; // Also wait for the server to send the done flag. (When playing online.)
						}
						// The game is being played.
						else if(event.button.x >= ((H_DIST*(y_lines-1))+(EDGE<<1))+35 && // "Reset" button was clicked.
								event.button.x <= ((H_DIST*(y_lines-1))+(EDGE<<1))+135 &&
								event.button.y >= EDGE+242 && event.button.y <= EDGE+272){
							reset_board(); // Reset the board
							// Adjust the score!
							Draw_Board(screen);
						}
						
						else if(event.button.x >= EDGE-(.5*H_DIST) &&  // The user clicked on the board.
							event.button.x <= (screen->w)-(EDGE<<1)-SIDEBAR+H_DIST &&
							event.button.y >= EDGE-(.5*V_DIST) &&
							event.button.y <= (screen->h)-(EDGE<<1)+V_DIST){
							// Round off x and y
							float linex, liney;
							int x, y;
							linex = (event.button.x-(.5*EDGE))/H_DIST; // Pick a x line.
							liney = (event.button.y-(.5*EDGE))/V_DIST; // Pick a y line.

							if(linex<1) linex = 0;
							else if(linex>=y_lines-1) linex = y_lines-1;
							x = (int)linex;

							if(liney<1) liney = 0;
							else if(liney>=y_lines-1) liney = y_lines-1;
							y = (int)liney;

							if(!(board[y*y_lines+x]^WHITE) || !(board[y*y_lines+x]^BLACK)){ // Make sure a stone was clicked on.
								Mark_Stone(x, y); // Check that a stone was clicked on, or a previously removed stone.
								// Check any liberties around the stones removed?
								score(); // Re-scan the board for changes.
							}
							else if(!(board[y*y_lines+x]^(BLACK|DEAD)) || !(board[y*y_lines+x]^(WHITE|DEAD))){
								/*
								 * If a group of stones were previously removed, display them back on the board.
								 * Also re-adjust the score, and correct territory.
								 */
								 Mark_Stone(x, y);
								 // Adjust the score!
								 Draw_Board(screen);
							}
							// Scan the board for empty spaces because dead stones were removed
							// Also account for dead groups, and unmark w or b spaces.
						}
					}

					else if(lost){
						if(event.button.x <= EDGE+(H_DIST*(y_lines-1))+(H_DIST*.5) && event.button.y <= (EDGE<<1)+(V_DIST*(y_lines-1))-(H_DIST*.5)){
							done = 1; // User clicked close board.
							Draw_Score(screen);
						}
					}
					else if(event.button.x >=EDGE-(.5*H_DIST) &&  // If the user clicked on the board.
						 event.button.x <= EDGE+(H_DIST*(y_lines-1))+(H_DIST*.5) && 
						 event.button.y >= EDGE-(.5*V_DIST) && 
						 event.button.y <= (EDGE<<1)+(V_DIST*(y_lines-1))-(H_DIST*.5)) Map_Click(screen, event.button.x, event.button.y);

					else if(event.button.x >= ((H_DIST*(y_lines-1))+(EDGE<<1))+35 && // If the user clicks the Pass button.
						event.button.x <= ((H_DIST*(y_lines-1))+(EDGE<<1))+130 &&
						event.button.y >= EDGE+201 && event.button.y <= EDGE+241){
						if(pass()) Draw_Board(screen);
					}
					else if(event.button.x >= ((H_DIST*(y_lines-1))+(EDGE<<1))+35&& // If the user clicks the Undo button.
						event.button.x <= ((H_DIST*(y_lines-1))+(EDGE<<1))+135&&
						event.button.y >= EDGE+242 && event.button.y <= EDGE+272){
						if(undo()){
							fill_last(kox, koy);
							Draw_Box(screen,EDGE+(kox*H_DIST),EDGE+(koy*V_DIST), 20, 20, color==BLACK?0:255); // Draw the box in the stone indicating a KO.
						}
						Draw_Board(screen);
					}
					else if(event.button.x >= ((H_DIST*(y_lines-1))+(EDGE<<1))+35 && // If the user clicks the Resign button.
						event.button.x <= ((H_DIST*(y_lines-1))+(EDGE<<1))+150 &&
						event.button.y >= EDGE+273 && event.button.y <= EDGE+303) resign(screen);
					Sulock(screen); // Unlock the screen.
					SDL_Flip(screen); // Display the changes.
				}
				// Also if editing, time = 0, no timers.
			}
			Draw_Time(screen, 0, 40, 0); // Display B's time.
			Draw_Time(screen, 0, 170, 1); // Display W's time.
		}
	}
	// The game is over, display the score.
	
	atexit(SDL_Quit); // Change to a "Are you sure?" question box?
	return 0;
}

void score(){
	int i, j, p;
	for(i=0;i<y_lines;i++){
		for(j=0;j<y_lines;j++){
			if(!board[(i*y_lines)+j]){
				if((p=enclosed(j, i)) > 0){
					//printf("x: %d y: %d, %d\n", j, i, p);
					if(p==BLACK) b_score+=Fix_Space(j, i, W_SPACE);
					else if(p==WHITE) w_score+=Fix_Space(j, i, B_SPACE); // Add the empty spaces to the score and fix them.
				}
			}
		}
	}
	// Change this into a print on the screen. Flash winners score and name???
	printf("w_score: %.2f b_score: %d\n", w_score, b_score);
}
short enclosed(short x, short y){
	// Needs to be optimized. If the space has both colors in it, all the empty spaces will be checked, over, and over, and over...	
	/*
	 * The enclosed() function takes an x and a y position (representing the index of a 2d matrix mapped into a 1d array)
	 * and checks if it is enclosed by the same color stone, if so the spaces are marked as owned by that color.
	 * Empty spaces are marked as CHECKED so they will not be re-scanned.
	 * When a stone is encountered, its color is returned.
	 * When a dead stone is encountered its owners color is returned.
	 * 
	 * Also, bounds checking is done within this function. So no need to worry about going out of range in memory.
	 *
  */
  if(board[(y*y_lines)+x]&B_SPACE || board[(y*y_lines)+x]&W_SPACE) return (board[(y*y_lines)+x]==B_SPACE)?BLACK:WHITE;
	else if(!(board[(y*y_lines)+x]^BLACK) || !(board[(y*y_lines)+x]^WHITE)){ // The XOR will return a 0 if it was just black or white.
		if(Check_Lib(x, y, 0) >= 2) return board[(y*y_lines)+x];
		return -1; // Else isn't needed.
	}
	else if(board[(y*y_lines)+x]&DEAD) return (board[(y*y_lines)+x]^DEAD)==BLACK?WHITE:BLACK; // A dead stone was found, reverse the color and return it.
	else if(!board[(y*y_lines)+x]){ // There was a blank space.
		short tmp = 0;
		board[(y*y_lines)+x] ^= CHECKED;
		if(y+1 <= y_lines-1 && !(board[((y+1)*y_lines)+x]&CHECKED)) tmp |= enclosed(x, y+1);
		if(y-1 >= 0 && !(board[((y-1)*y_lines)+x]&CHECKED)) tmp |= enclosed(x, y-1);
		if(x+1 <= y_lines-1 && !(board[(y*y_lines)+x+1]&CHECKED)) tmp |= enclosed(x+1, y);
		if(x-1 >= 0 && !(board[(y*y_lines)+x-1]&CHECKED)) tmp |= enclosed(x-1, y);
		board[(y*y_lines)+x] ^= CHECKED;
		return tmp; // By ORing all the values together it can be checked if they were all the same number, ie. the same stone color.
	}
	return 0; // The function never reaches here...
}

int pass(){
	// Display "[W|B] Passed" above the board.
	long long last_move = moves.get_last();
	if(last_move != -2 && last_move != -1){ // Check that the last move wasn't a pass, or the first move to fill it up.
		lastx = last_move>>8;
		lasty = last_move^(lastx<<8);
		fill_last(lastx, lasty);
	}
	moves.add(-2, -2); // Add the move to the linked list of moves.
	if(color==3){
		w_time.pause();
		w_stones--;
		b_time.unpause();
	}
	else{
		b_time.pause();
		b_stones--;
		w_time.unpause();
	}
	color^=2;
	passed++;
	if(passed == 3){ // 3 passes back to back indicate the game is finished.
		scoring = true;
		b_time.stop();
		w_time.stop();
		puts("Calling score()");
		score(); // Scan the board for empty spaces.
		Draw_Score(screen);
		return 1;
	}
	return 0;
}

int undo(){
	long long last_move = moves.get_last();
	int tmp_color = color==1?BLACK:WHITE;
	
	printf("returned: %llu\n", last_move);
	if(last_move == -1) b_stones = BYOYOMI_STONES; // Reset time also?
	else if(last_move >= 0){ // If last_move == -1 there are no more moves.

		unsigned char lost_stones = moves.removed();
		unsigned char stones_removed = 0;
		moves.rem(); // Remove the stone from the list of moves.
		lastx = last_move>>8;
		lasty = last_move^(lastx<<8);
		
		if(lost_stones&1) // 1 from below
			stones_removed += Fix_Space(lastx, lasty+1, tmp_color)+1;
		if(lost_stones&2) // 2 from above
			stones_removed += Fix_Space(lastx, lasty-1, tmp_color)+1;
		if(lost_stones&4) // 4 from the left
			stones_removed += Fix_Space(lastx-1, lasty, tmp_color)+1;
		if(lost_stones&8) // 8 from the right
			stones_removed += Fix_Space(lastx+1, lasty, tmp_color)+1;
		
		if(color==3){ // It was whites turn, black requested an undo.
			w_time.pause();
			if(moves.get_last() == -1) b_time.start(); // Black had no prior moves.
			else b_time.unpause();
			if(moves.get_last() != -1) b_stones++; // Increase the stones black has to play again.
			b_score -= stones_removed; // Adjust black's score if he had taken any stones.
		}
		else{ // It was blacks turn, white requested an undo.
			// Check that this was not whites first turn!!!
			b_time.pause();
			// Check that it wasn't whites first move.
			w_time.unpause();
			w_score -= stones_removed;
			w_stones++;
		}

		color ^= 2;
		board[(lasty*y_lines)+lastx] ^= board[(lasty*y_lines)+lastx]; // Clear the place on the board.			

		last_move = moves.get_last();
		if(last_move!=-1){
			lastx = last_move>>8;
			lasty = last_move^(lastx<<8);
			lost_stones = moves.removed();
		}
		else if(last_move == -1) b_stones = BYOYOMI_STONES;
		else if(last_move == -2) lastx = lasty = kox = koy = -1; // Check this.
		else lastx=lasty=-1;
		
		if(lost_stones&16){ // 16 means KO stone
			kox = lastx;
			koy = lasty;
			return 1;
		}
		else kox=koy=-1;
	}
	else if(last_move == -2){ // Player had passed.
		moves.rem();
		last_move = moves.get_last();
		if(last_move!=-1){
			lastx = last_move>>8;
			lasty = last_move^(lastx<<8);
		}
		else lastx=lasty=-1;
		
		if(color==3){
			w_time.pause();
			b_stones++;
			b_time.unpause();
		}
		else{
			b_time.pause();
			w_time.unpause();
			w_stones++;
		}
		if(passed) passed--;
		color ^= 2;
	}
	return 0;
}

void Mark_Stone(short x, short y){ // Can pointer magik be used?
	/*
	 * Mark_Stone() marks a group of stones positioned at x, y (in the 2d matrix of the board) dead.
	 * This function will check if the stone in any of four directions is the same (or if any of those directions can be checked based on the dimensions of the board).
	 * If it's the same stone, call the function on that position and mark it as dead.
	*/
	char tmp_stone = board[(y*y_lines)+x]; // Will this calculation sit in cache?
	board[(y*y_lines)+x] ^= DEAD; // Mark the space on the board as dead.
	if(y+1 <= y_lines-1 && !(board[((y+1)*y_lines)+x]^tmp_stone)) // Make sure the stone above and the current stone are equal.
		Mark_Stone(x, y+1);
	if(y-1 >= 0 && !(board[((y-1)*y_lines)+x]^tmp_stone)) // Check down
		Mark_Stone(x, y-1);
	if(x+1 <= y_lines-1 && !(board[(y*y_lines)+x+1]^tmp_stone)) // Check right
		Mark_Stone(x+1, y);
	if(x-1 >= 0 && !(board[(y*y_lines)+x-1]^tmp_stone)) // Check left
		Mark_Stone(x-1, y);
}

unsigned int Fix_Space(short x, short y, short swap){
	// This function redraws removed stones, and returns the number of stones reset.
	// This function is also used for scoring (because its so s3xE)
	board[(y*y_lines)+x] ^= swap;
	unsigned int c = 0;
	if(y+1 <= y_lines-1 && !board[((y+1)*y_lines)+x]) // Check up
		c += Fix_Space(x, y+1, swap)+1; // Increase the count, and return how many spaces were fixed above.
	if(y-1 >= 0 && !board[((y-1)*y_lines)+x]) // Check down
		c += Fix_Space(x, y-1, swap)+1; // Increase the count, and return how many spaces were fixed below.
	if(x+1 <= y_lines-1 && !board[(y*y_lines)+x+1]) // Check right
		c += Fix_Space(x+1, y, swap)+1; // Increase the count, and return how many spaces were fixed to the right.
	if(x-1 >= 0 && !board[(y*y_lines)+x-1]) // Check left
		c += Fix_Space(x-1, y, swap)+1; // Increase the count, and return how many spaces were fixed to the left.
	return c;
}

void Map_Click(SDL_Surface *screen, int x, int y){
	int p = 0; // Check if any stones were removed.
	int a, r=0;
	char removed = 0;
	long long last_move = moves.get_last();
	
	float linex, liney;
	linex = (x-EDGE)/H_DIST; // Pick a x line.
	liney = (y-EDGE)/V_DIST; // Pick a y line.

  if(linex>=y_lines-1) linex = y_lines-1;
	x = (int)(linex+.4);

  if(liney>=y_lines-1) liney = y_lines-1;
	y = (int)(liney+.4);

//	printf("%f %f %i %i\n", linex, liney, x, y);

	if(!board[(y*y_lines)+x]){
		//printf("Played: %c%d\n", translate[x], y+1);
		board[(y*y_lines)+x] = color==3?WHITE:BLACK; // Set the spot played to black, or white.
		if(Check_Lib(x, y, 0)){ // Returns 1 if the stone can be played.
			// Check that the 4 spaces around the stone played don't need to be removed.
			if(x+1 <= y_lines-1 && board[y*y_lines+x+1] && board[y*y_lines+x] ^ board[y*y_lines+x+1] && !Check_Lib(x+1, y, 0)){ // DONT CALL CHECK_LIB TWICE!
				r += Check_Lib(x+1, y, 1);
				removed ^= 8;
				p++;
			}
			if(x-1 >= 0 && board[y*y_lines+x] ^ board[y*y_lines+x-1] && board[y*y_lines+x-1] && !Check_Lib(x-1, y, 0)){
				r += Check_Lib(x-1, y, 1);
				removed ^= 4;
				p++;
			}
			if(y+1 <= y_lines-1 && board[(y+1)*y_lines+x] && board[y*y_lines+x] ^ board[(y+1)*y_lines+x] && !Check_Lib(x, y+1, 0)){
				r += Check_Lib(x, y+1, 1);
				removed ^= 1;
				p++;
			}
			if(y-1 >= 0 && board[(y-1)*y_lines+x] && board[y*y_lines+x] ^ board[(y-1)*y_lines+x] && !Check_Lib(x, y-1, 0)){
				r += Check_Lib(x, y-1, 1);
				removed ^= 2;
				p++;
			}
			p++;
			kox = koy = -1;
		}
		else{
			// Maybe the spot was an eye that was going to be filled.
			// Check and see if the other stones need to be removed.
			if(x+1 <= y_lines-1 && board[(y*y_lines)+x+1] && board[(y*y_lines)+x] ^ board[(y*y_lines)+x+1] && !Check_Lib(x+1, y, 0)){
				if(kox != x+1 && koy != y){
					r += Check_Lib(x+1, y, 1);
					removed ^= 8;
					p=2;
					kox = koy = -1;
				}
			}
			if(x-1 >= 0 && board[(y*y_lines)+x] ^ board[(y*y_lines)+x-1] && board[(y*y_lines)+x-1] && !Check_Lib(x-1, y, 0)){
				if(kox != x-1 && koy != y){
					r += Check_Lib(x-1, y, 1);
					removed ^= 4;
					p=2;
					kox = koy = -1;
				}
			}
			if(y+1 <= y_lines-1 && board[((y+1)*y_lines)+x] && board[(y*y_lines)+x] ^ board[(y+1)*y_lines+x] && !Check_Lib(x, y+1, 0)){
				if(kox != x && koy != y+1){
					r += Check_Lib(x, y+1, 1);
					removed ^= 1;
					p=2;
					kox = koy = -1;
				}
			}
			if(y-1 >= 0 && board[((y-1)*y_lines)+x] && board[(y*y_lines)+x] ^ board[((y-1)*y_lines)+x] && !Check_Lib(x, y-1, 0)){
				if(kox != x && koy != y-1){
					r += Check_Lib(x, y-1, 1);
					removed ^= 2;
					p=2;
					kox = koy = -1;
				}
			}
		}
		
		if(!p) board[y*y_lines+x] = 0; // The stone could not be played, clear it and continue.
		else if(!(p^1)){ // If p==1 no stones were removed, so don't redraw the board.
			if(last_move >= 0){ // The last move wasn't a pass, fill it up.
				lastx = last_move>>8;
				lasty = last_move^(lastx<<8);
				fill_last(lastx, lasty);
			}
			if(!(color^BLACK)){ // Check for black
				b_time.pause();
				for(a=1;a<=STONE_SIZE;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, 0); // Draw the stone.
				for(a=9;a<=11;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, 255);
				b_stones--;
				if(!b_stones) b_stones = BYOYOMI_STONES; // Unless main time hasnt finished.
				w_time.unpause();
			}
			else{ // Check for white
				w_time.pause();
				for(a=1;a<=STONE_SIZE;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, 255); // Draw the stone.
				for(a=9;a<=11;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, 0);
				w_stones--;
				if(!w_stones) w_stones = BYOYOMI_STONES; // Unless main time hasnt finished.
				b_time.unpause();
			}
			moves.add(x, y); // Add the move to the linked list.
			color^=2;
			kox=koy=-1;
			passed ^= passed; // Reset the passed flag.
		}
		else{ // FIX THIS
			if(Check_Lib(x, y, 0) == 1 && r == 1){ // We took a stone, and only have 1 lib. Let's call that a KO.
				removed ^= 16; // We did remove some stones, didnt we?
				moves.add(x, y, removed); // Add the move to the linked list. Also tell it what stones were removed.
				Draw_Board(screen); // Don't need to fill last when this is here. Or to draw the new stone because this will do it already.
				if(color == BLACK){
					b_time.pause();
					b_stones--;
					if(!b_stones) b_stones = BYOYOMI_STONES;
					w_time.unpause();
				}
				else{
					w_time.pause();
					w_stones--;
					if(!w_stones) w_stones = BYOYOMI_STONES;
					b_time.unpause();
				}
				kox = x;
				koy = y;
				Draw_Box(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), 20, 20, color==BLACK?255:0); // Draw the box in the stone indicating a KO.
			}

			else{ // More then one stone
				moves.add(x, y, removed); // Add the move to the linked list. Also tell it what stones we removed.
				Draw_Board(screen); // Don't need to fill last when this is here. Or to draw the new stone because this will do it already.
				if(color == BLACK){
					b_time.pause();
					b_stones--;
					for(a=9;a<=11;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, 255);
					if(!b_stones) b_stones = BYOYOMI_STONES;
					w_time.unpause();
				}
				else{
					w_time.pause();
					w_stones--;
					for(a=9;a<=11;a++) Circle(screen,EDGE+(x*H_DIST),EDGE+(y*V_DIST), a, 0);
					if(!w_stones) w_stones = BYOYOMI_STONES;
					b_time.unpause();
				}
			}
			passed ^= passed;
			color^=2;
		}
	}
}

unsigned int Check_Lib(short x, short y, int rem){
	/*
	 * This function has three uses:
	 * 1. When liberties are being checked, it will return 0-n liberties for the stone(s) that are connected at (x, y)
	 * 2. When stones are being removed, it will remove the stone blob and adjust the score.
	 * 3, When stones were previously removed and were re-clicked on the board, they are unmarked and the score is adjusted.
	 */
	unsigned int c = 0;
	if(x+1 <= y_lines-1){
		if(!board[(y*y_lines)+x+1] || !(board[(y*y_lines)+x+1]^CHECKED) || board[(y*y_lines)+x+1]&B_SPACE || board[(y*y_lines)+x+1]&W_SPACE) c++; // Theres an empty space, increase the libertie count. OR enclosed() was called and the free spaces might be marked.
		else if(!(board[(y*y_lines)+x]^board[(y*y_lines)+x+1])){ // The stone to the right was the same color.
			board[(y*y_lines)+x] ^= CHECKED;
			c += Check_Lib(x+1, y, rem);
			board[(y*y_lines)+x] ^= CHECKED;
		}
	}
	if(x-1 >= 0){
		if(!board[(y*y_lines)+x-1] || !(board[(y*y_lines)+x-1]^CHECKED) || board[(y*y_lines)+x-1]&B_SPACE || board[(y*y_lines)+x-1]&W_SPACE) c++; // A free libertie to the left.
		else if(!(board[(y*y_lines)+x]^board[(y*y_lines)+x-1])){ // The same stone to the left.
			board[(y*y_lines)+x] ^= CHECKED;
			c += Check_Lib(x-1, y, rem);
			board[(y*y_lines)+x] ^= CHECKED;
		}
	}
	if(y+1 <= y_lines-1){
		if(!board[(y+1)*y_lines+x] || !(board[((y+1)*y_lines)+x]^CHECKED) || board[((y+1)*y_lines)+x]&B_SPACE || board[((y+1)*y_lines)+x]&W_SPACE) c++; // A free libertie above.
		else if(!(board[(y*y_lines)+x]^board[((y+1)*y_lines)+x])){ // The same stone is above the current space.
			board[(y*y_lines)+x] ^= CHECKED;
			c += Check_Lib(x, y+1, rem);
			board[(y*y_lines)+x] ^= CHECKED;
		}
	}
	if(y-1 >= 0){
		if(!board[((y-1)*y_lines)+x] || !(board[((y-1)*y_lines)+x]^CHECKED) || board[((y-1)*y_lines)+x]&B_SPACE || board[((y-1)*y_lines)+x]&W_SPACE) c++;
		else if(!(board[(y*y_lines)+x]^board[((y-1)*y_lines)+x])){
			board[(y*y_lines)+x] ^= CHECKED;
			c += Check_Lib(x, y-1, rem);
			board[(y*y_lines)+x] ^= CHECKED;
		}
	}
	if(rem){
		board[(y*y_lines)+x]==WHITE?b_score++:w_score++; // Increase white or blacks score depending what color stone was removed.
		board[(y*y_lines)+x]^=board[(y*y_lines)+x];
		return 1;
	}
	return c;
}

void reset_board(){ // "Reset" button scans the board[] for & 0x08 and clears it when found.
	// Also fix the score!
	int q;
	for(q=0;q<y_lines*y_lines;q++)
		if(board[q]&W_SPACE || board[q]&B_SPACE){
			if(board[q]==W_SPACE) w_score--;
			else if(board[q]==B_SPACE) b_score--;
			board[q] ^= board[q]; // If was a marked empty space.
		}
		else if(board[q]&DEAD){
			if(board[q]==(DEAD|WHITE)) b_score--;
			else if(board[q]==(DEAD|BLACK)) w_score--;
			board[q] ^= DEAD; // If the spot was marked as a removed stone.
		}
}

