#include <stdio.h>
#include <sys/socket.h>

int sv [2];

int main ()
{
  socketpair (AF_UNIX,
    SOCK_STREAM, 0, sv);
  if (fork ()) { /* parent */
    char buf [MAX_INPUT]; 
    char sbuf [MAX_INPUT];
    FILE *f;

    f = fopen ("prodfile", "w");
    while (1) {
      if (!fgets (buf, MAX_INPUT, stdin))
        break;
      fputs (buf, f);
      fflush (f);
      send (sv [0], "read",
        MAX_INPUT, 0);
      while (recv (sv [0], sbuf, 
        MAX_INPUT, MSG_DONTWAIT) == -1)
        usleep (10000);
      if (strcmp (sbuf, "send")) break;
    }
  } else { /* child */
    char buf [MAX_INPUT];
    char sbuf [MAX_INPUT];
    FILE *f;

    f = fopen ("prodfile", "r");
    while (1) {
      while (recv (sv [1], sbuf, 
        MAX_INPUT, MSG_DONTWAIT) == -1)
        usleep (10000);
      if (strcmp (sbuf, "read")) break;
      if (!fgets (buf, MAX_INPUT, f))
        break;
      fputs (buf, stdout);
      send (sv [1], "send",
        MAX_INPUT, 0);
    }
  }
  return 0;
}

