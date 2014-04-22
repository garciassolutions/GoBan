#include <curses.h>
#include <string.h>
#include <panel.h>

unsigned short int x, y, x2, y2; // Some variables we're going to need.
unsigned short int r = 0; // Get rid of the first \n in print_win().

void print_win(WINDOW *, char *); // Function to print to a window.
char *get_string(WINDOW *); // Get a string from a window.

int main(int argc, char **argv){
	int i;
	initscr(); // Start ncurses.
	getmaxyx(stdscr, y, x); // Get window resolution.
	refresh(); // Needs it after getmaxyx()?
	WINDOW *win[3]; // The three windows we're going to need.	
	PANEL  *pan[3]; // Lets make 3 panels aswell.
	win[0] = newwin(1, x, 0, (int)(x/2)); // Title window.
	win[1] = newwin(y-2, x, 1, 0); // Main window.
	win[2] = newwin(1, x, y-1, 0); // Input window.
	pan[0] = new_panel(win[0]);
	pan[1] = new_panel(win[1]);
	pan[2] = new_panel(win[2]);
	wprintw(win[0], "Title...");
	wprintw(win[1], "Server bullshit...");
	wprintw(win[2], "Input: ");
	update_panels();
	doupdate();
	for(i=0;i<50;i++) print_win(win[1], "haxs\n");
	for(i=0;i<10;i++) print_win(win[1], get_string(win[2]));
	print_win(win[1], ":: Press any key to exit.\n");
	getch();
	endwin(); // End ncurses.
	return 0;
}

void print_win(WINDOW *my_win, char *str){
	getmaxyx(my_win, y, x);
	getyx(my_win, y2, x2);
	if(y2 == y-1 && r){ // Matches max y on the first run, moves a blank line up, and prints.
		wmove(my_win, 0, 0);
		wdeleteln(my_win);
		wmove(my_win, y2, 0);
	}
	if(y2==y-1) r++;
	wprintw(my_win, "%s", str);
	update_panels();
}

char *get_string(WINDOW *my_win){
	static char input[26] = { 0 };
	mvwscanw(my_win, 0, 7, "%25c", &input);
	wdeleteln(my_win);
	mvwprintw(my_win, 0, 0, "Input: ");
	wrefresh(my_win);
	return input;
}
