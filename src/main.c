#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include "store.h"

#define WIDTH 77
#define HEIGHT 23

const char TEST_DESC[] = "Get beers";

void seed_test_data(task_grouping *task_grouping) {
  int i;
  task_grouping->today_size = 7;
  task_grouping->today = malloc(sizeof(task *) * task_grouping->today_size);
  for (i = 0; i < task_grouping->today_size; i++) {
    task_grouping->today[i] = malloc(sizeof(task));
    task_grouping->today[i]->description = malloc(sizeof(char) * 255);
    strncpy(task_grouping->today[i]->description, TEST_DESC, sizeof(TEST_DESC));
  }
}

int main(int argc, char *argv[]) {
  WINDOW *window;
  int i, c, quit = 0, startx = 5, starty = 3, highlight = 0;
  task_grouping task_grouping;

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

  while(1) {
    mvwprintw(window, starty, startx, "navigate: jk   quit: q");
    wclrtoeol(window);

    for(i = 0; i < task_grouping.today_size; i++) {
      task *task = task_grouping.today[i];

      if(highlight == i) {
        wattron(window, A_REVERSE);
        mvwprintw(window, starty + (i + 2), startx, "%d. %s", i + 1, task->description);
        wattroff(window, A_REVERSE);
      } else {
        mvwprintw(window, starty + (i + 2), startx, "%d. %s", i + 1, task->description);
      }

      wclrtoeol(window);
    }

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
