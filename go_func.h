#include <panel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#ifndef GO_FUNC_H
#define GO_FUNC_H

unsigned int x, y, y2, x2;

void DieWithError(char *);
void print_win(WINDOW *, char *);
void get_string(WINDOW *);

void DieWithError(char *err){
	print_win(win[1], ":: Error\n");
	print_win(win[1], err);
	print_win(win[1], "\n:: Please press any key to continue.\n");
	getch();
	endwin();
	exit(1);
}

void print_win(WINDOW *my_win, char *str){
	getmaxyx(my_win, y, x);
	getyx(my_win, y2, x2);
	if(y2 == y-1){ // Matches max y on the first run, moves a blank line up, and prints.
		wmove(my_win, 0, 0);
		wdeleteln(my_win);
		wmove(my_win, y2, 0);
	}
	wprintw(my_win, "%s", str);
	update_panels();
	doupdate();
}

void get_string(WINDOW *my_win){
	memset(&input, 0, sizeof(input));
	mvwscanw(my_win, 0, 7, "%25c", &input);
	wdeleteln(my_win);
	mvwprintw(my_win, 0, 0, "Input: ");
	update_panels();
}

#endif
