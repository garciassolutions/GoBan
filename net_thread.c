#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panel.h>
#include <unistd.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF 128

void DieWithError(char *, int);
void print_win(WINDOW *, char *);
void get_string(WINDOW *);

unsigned int x, y, x2, y2; // Some variables we're gunna need.
static char input[BUF], buf[BUF]; // Some storage buckets.
WINDOW *win[3]; // Create 3 windows, top, bottom, and middle.
PANEL *pan[3]; // Create 3 panels for our 3 windows.
int r = 0;

fd_set Go_fd;

int main(int argc, char **argv){
	int i, sock;
	
	initscr(); // Start curses mode.
	getmaxyx(stdscr, y, x); // Get max screen size.
	refresh(); // The refresh is just needed?
	win[0] = newwin(1, x, 0, (int)(x/2)); // Title window.
	win[1] = newwin(y-2, x, 1, 0); // Main window.
	win[2] = newwin(1, x, y-1, 0); // Input window.
	pan[0] = new_panel(win[0]);
	pan[1] = new_panel(win[1]);
	pan[2] = new_panel(win[2]);
	wprintw(win[0], "Title...");
	wprintw(win[1], "Server bullshit...\n");
	wprintw(win[2], "Input: ");
	update_panels(); // Put our panels in the right order.
	doupdate(); // Refresh them on the screen.
	
	struct sockaddr_in Sock_in;
	unsigned short port = 6969;
	char *servIP, send_buff[BUF], read_buff[BUF];
	servIP = "210.155.158.200";

	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithError("socket() failed.\n", 0);
	print_win(win[1], "socket() successful\n");
	
	memset(&Sock_in, 0, sizeof(Sock_in));
	Sock_in.sin_family = AF_INET;
	Sock_in.sin_addr.s_addr = inet_addr(servIP);
	Sock_in.sin_port = htons(port);
	
	if(connect(sock, (struct sockaddr *) &Sock_in, sizeof(Sock_in)) < 0)
		DieWithError("connect() failed.\n", sock);
  
  if(fork()){
    while(recv(sock, &buf, BUF-1, 0) > 0){
      print_win(win[1], buf);
    }
  }
  else{
    while(1){
      get_string(win[2]);
      sprintf(input, "%s\n", input);
      if(send(sock, &input, BUF, 0) < BUF)
        DieWithError("send() failed", sock);
      print_win(win[1], input);
    }
  }
  // Poor program. We never get here.
  close(sock);
  endwin();
  return 0;
}

void DieWithError(char *err, int sock){
	print_win(win[1], ":: Error\n");
	print_win(win[1], err);
	print_win(win[1], "\n:: Please press any key to continue.\n");
	if(sock) close(sock);
	getch();
	endwin();
	exit(1);
}

void print_win(WINDOW *my_win, char *str){
	getmaxyx(my_win, y, x);
	getyx(my_win, y2, x2);
	if(y2 == y-1 && r){ // Matches max y on the first run, moves a blank line up, and prints.
		wmove(my_win, 0, 0);
		wdeleteln(my_win);
		wmove(my_win, y2, 0);
	}
	else if(y2==y-1) r++;
	wprintw(my_win, "%s", str);
//	for(;*str;str++)
//		if(*str == '\n' || *str == '\r'){
//      if(y2 == y-1){
//		    wmove(my_win, 0, 0);
//		    wdeleteln(my_win);
//		    wmove(my_win, y2, 0);
//		    }
//      }
//    else waddch(my_win, *str);
	update_panels();
	doupdate();
}

void get_string(WINDOW *my_win){
	memset(&input, 0, BUF);
	mvwgetnstr(my_win, 0, 7, input, BUF-1);
	wdeleteln(my_win);
	mvwprintw(my_win, 0, 0, "Input: ");
	update_panels();
	doupdate();
}
