#include <stdio.h>

using namespace std;
/*
 * A doublely linked list consists of four parts:
 * 1. Data (Object).
 * 2. A "next" pointer.
 * 3. A "previous" pointer.
 * 4. A "current" pointer (to keep track).
 */
class Linked_List{ // Make a new class.
  private:
    struct node{ // Make a data structure (object) in the class.
      char rem;
      int posx, posy; // x and a y position of a stone on the board.
      int time; // The time the stone was played at.
      node *next; // The next stone played after this one.
      node *prev; // The stone played before this one.
    } *curr;
  public:
    Linked_List();
    char removed(); // Return the direction stones were removed in.
    void add(int, int); // Add at the end, no stones removed.
    void add(int, int, int); // Add at the end.
    void rem(); // Remove the last element.
    void display(); // Display the elements.
    long long get_last();
    bool add_rem(int, int); // Add a removed stone to the list. Kept for score keeping.
    int num();
    int get_time();
};

Linked_List::Linked_List(){ // Make an empty list when the class is created.
  curr = NULL;
}

void Linked_List::add(int x, int y){ // Add a stone to the list.
  if(!curr){ // If there is no list already.
    curr = new node; // Create a new list.
    curr->posx = x; // Set the x location of the stone.
    curr->posy = y; // Set the y location of the stone.
    curr->next = NULL; // The next move isn't known, so make it nothing.
    curr->prev = NULL; // There was no move before this.
    curr->rem = 0; // No stones were removed this turn.
  }
  else{ // This is not the first item in the list.
    node *t1 = new node; // Make a new item for the list.
    t1->posx = x; // Set the x position of the stone.
    t1->posy = y; // Set the y position of the stone.
    t1->next = NULL; // Next move isn't defined, so set it to nothing.
    t1->rem = 0; // No stones were removed.
    curr->next = t1; // Add this stone to the list and move the current pointer.
    t1->prev = curr; // Set the previous pointer. (For item removal.)
    curr = t1;
  }
}

void Linked_List::add(int x, int y, int rem){ // The same as above, but stones were removed.
  if(!curr){ // If this is the first element.
    curr = new node;
    curr->posx = x;
    curr->posy = y;
    curr->next = NULL;
    curr->prev = NULL;
    curr->rem = rem;
  }
  else{ // There's a list already, just add to it.
    node *t1 = new node; 
    t1->posx = x;
    t1->posy = y;
    t1->next = NULL;
    t1->rem = rem;
    curr->next = t1;
    t1->prev = curr;
    curr = t1;
  }
}
void Linked_List::rem(){ // Remove the last item from the list. (Undo)
  if(!curr->prev){
   curr = NULL;
  }
  else{
	node *t1;
  	t1 = curr->prev;
  	delete curr;
  	curr = t1;
  	curr->next = NULL; // Don't point to something that's not there.
  }
}

int Linked_List::num(){ // Count how many items are in the list.
  int x = 0;
  node *t;
  for(t=curr;t!=NULL;t=t->prev, x++);
  return x;
}

void Linked_List::display(){ 
  node *q;
  for(q=curr;q!=NULL;q=q->prev) cout<<endl<<q->posx << " " << q->posy <<endl;
}

long long Linked_List::get_last(){ // Long is too short to display [1-19][1-19]
  return curr?(curr->posx<<8)^curr->posy:-1; // Return -1 on an empty list
}

int Linked_List::get_time(){
  return curr->time;
}

char Linked_List::removed(){ // Return the amount of removed stones.
  return curr->rem;
}
