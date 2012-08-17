#ifndef STORE_H
#define STORE_H

typedef struct task {
  char *description;
  time_t completed;
  int due;
} task;

typedef struct task_grouping {
  int today_size;
  task **today;

  int next_size;
  task **next;

  int completed_size;
  task **completed;
} task_grouping;

#endif STORE_H
