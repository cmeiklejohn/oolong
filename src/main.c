#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include "oolong.h"

#define WIDTH 77
#define HEIGHT 23

#define MAX_WIDTH 80
#define MAX_HEIGHT 24
#define MARGIN 2

char *TODAY_LABEL = "TODAY";
char *NEXT_LABEL = "NEXT";
char *COMPLETED_LABEL = "COMPLETED";
const char TEST_DESC[] = "Get beers";

void seed_test_data(oolong_task_grouping *task_grouping) {
  int i;
  task_grouping->today_size = 7;
  task_grouping->today = malloc(sizeof(oolong_task *) * task_grouping->today_size);
  for (i = 0; i < task_grouping->today_size; i++) {
    task_grouping->today[i] = malloc(sizeof(oolong_task));
    task_grouping->today[i]->description = malloc(sizeof(char) * 255);
    strncpy(task_grouping->today[i]->description, TEST_DESC, sizeof(TEST_DESC));
  }
}

/* print tasks and groups */

int print_task(WINDOW *window, int y, int x, char *description, int i) {
  //wattron(window, A_REVERSE);
  //mvwprintw(window, y, x, "%d. %s", i + 1, description);
  //wattroff(window, A_REVERSE);
   
  mvwprintw(window, y, x, "%d. %s", i + 1, description);

  wclrtoeol(window);

  return 0;
}

int print_task_grouping(WINDOW *window, int y, int x, oolong_task **tasks, int tasks_size, char *label) {
  int i;
  
  mvwprintw(window, y, x, label);
  wclrtoeol(window);
  for(i = 0; i < tasks_size; i++) {
    oolong_task *task = tasks[i];
    print_task(window, y + (i + 2), x, task->description, i);
  }

  return 0;
}

int print_task_groupings(WINDOW *window, int y, int x, oolong_task_grouping *grouping) {
  print_task_grouping(window, y, x, grouping->today, grouping->today_size, TODAY_LABEL);
  print_task_grouping(window, y + grouping->today_size + MARGIN + 1, x, grouping->completed, grouping->completed_size, COMPLETED_LABEL);
  print_task_grouping(window, y, x + 40, grouping->next, grouping->next_size, NEXT_LABEL);
}

int main(int argc, char *argv[]) {
  WINDOW *window;
  int i, c, quit = 0, startx = 5, starty = 3, highlight = 0;
  oolong_task_grouping task_grouping;

  seed_test_data(&task_grouping);

  initscr();
  clear();
  raw();
  noecho();
  cbreak();
  curs_set(0);

  startx = (80 - WIDTH);
  starty = (24 - HEIGHT);
  window = newwin(HEIGHT, WIDTH, starty, startx);
  keypad(window, TRUE);

  // run loop
  while(1) {
    print_task_groupings(window, starty, startx, &task_grouping);

    mvwprintw(window, starty + 21, startx, "navigate: jk   quit: q");
    wclrtoeol(window);
    wclrtobot(window);
    wrefresh(window);

    c = wgetch(window);
    switch(c) {
    case KEY_UP:
    case 'k':
      if(highlight == 0) {
        highlight = task_grouping.today_size - 1;
      } else {
        --highlight;
      }
      break;
    case KEY_DOWN:
    case 'j':
      if(highlight == task_grouping.today_size - 1) {
        highlight = 0;
      } else {
        ++highlight;
      }
      break;
    case 'q':
      quit = 1;
    }

    if (quit) {
      break;
    }
  }

  curs_set(1);
  refresh();
  endwin();

  return 0;
}
