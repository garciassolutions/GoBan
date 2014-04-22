#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <panel.h>
#include <unistd.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF 1024

unsigned int x, y, x2, y2; // Some variables we're gunna need.
unsigned short int r = 0; // The first upline.
static char input[BUF], buf[BUF]; // Some storage buckets.
WINDOW *win[3]; // Create 3 windows, top, bottom, and middle.
PANEL *pan[3]; // Create 3 panels for our 3 windows.

void DieWithError(char *);
void print_win(WINDOW *, char *);
void get_string(WINDOW *);

fd_set window_fd; // The windows file descriptor set. The socket and the terminal.

int main(int argc, char *argv[]){
  int sock; // Socket file descriptor.
  struct sockaddr_in PandaNet; // Server structure.

  initscr(); // Start curses mode.
  getmaxyx(stdscr, y, x); // Get max screen size.
  refresh(); // The refresh is just needed?
  
  win[0] = newwin(1, x, 0, (int)(x/2)); // Title window.
  win[1] = newwin(y-2, x, 1, 0); // Main window.
  win[2] = newwin(1, x, y-1, 0); // Input window.
  
  pan[0] = new_panel(win[0]); // Make our panels the right order, top to bottom.
  pan[1] = new_panel(win[1]);
  pan[2] = new_panel(win[2]);
  
  wprintw(win[0], "Server: Not connected.");
  wprintw(win[1], "[-] Connecting to server.\n");
  wprintw(win[2], "Input: ");
  
  update_panels(); // Put our panels in the right order.
  doupdate(); // Refresh them on the screen.
  
  if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError("socket() failed.\n");

  memset(&PandaNet, 0, sizeof(PandaNet));
  PandaNet.sin_family = AF_INET;
  PandaNet.sin_addr.s_addr = inet_addr("210.155.158.200");
  PandaNet.sin_port = htons(6969);

  if(connect(sock, (struct sockaddr *) &PandaNet, sizeof(PandaNet)) < 0)
    DieWithError("[!] A connection could not be made to PandaNet.\n");
  
  wdeleteln(win[0]);
  mvwprintw(win[0], 0, 0, "Server: Connected.");
  print_win(win[1], "[-] Connection established.");
  memset(&input, 0, sizeof(input));
  
	while(1){
	  int stdIN = fileno(stdin);
	  FD_ZERO(&window_fd); // Clear and set the file descriptors.
	  FD_SET(stdIN, &window_fd);
	  FD_SET(sock, &window_fd);
	  
		if(select(sock+1, &window_fd, NULL, NULL, NULL) < 0) DieWithError("select() failed.\n");

		if(FD_ISSET(stdIN, &window_fd)){
			get_string(win[2]);
			sprintf(input, "%s\n", input);
      print_win(win[1], input);
			if(send(sock, &input, sizeof(input), 0) < sizeof(input)) DieWithError("send() failed");
		}

		else if(FD_ISSET(sock, &window_fd))
			if(recv(sock, &buf, sizeof(buf)-1, 0) > 0) print_win(win[1], buf);
	}
	close(sock);
	free(buf);
	endwin(); // Stop curses.
  return 0;
}

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
	if(y2 == y-1 && r){ // Matches max y on the first run, moves a blank line up, and prints.
		wmove(my_win, 0, 0);
		wdeleteln(my_win);
		wmove(my_win, y2, 0);
	}
	if(y2==y-1) r++;
	wprintw(my_win, "%s", str);
	update_panels();
	doupdate();
}

void get_string(WINDOW *my_win){
  mvwgetnstr(my_win, 0, 7, input, sizeof(input)-2);
	wdeleteln(my_win);
	mvwprintw(my_win, 0, 0, "Input: ");
	update_panels();
	doupdate();
}
