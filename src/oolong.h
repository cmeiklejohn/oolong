#include <msgpack.h>

#ifndef OOLONG_H
#define OOLONG_H

typedef struct oolong_task {
  char *description;
  bool completed;
  time_t due;
} oolong_task;

typedef struct oolong_task_grouping {
  int today_size;
  oolong_task **today;

  int next_size;
  oolong_task **next;

  int completed_size;
  oolong_task **completed;
} oolong_task_grouping;

msgpack_sbuffer *
oolong_serialize(msgpack_sbuffer *sbuf, oolong_task **tasks, int size);

oolong_task **
oolong_deserialize(oolong_task **tasks, msgpack_sbuffer *sbuf);

oolong_task **
oolong_group_individual(oolong_task **tasks, int *size, oolong_task *task);

oolong_task_grouping *
oolong_create_grouping(oolong_task_grouping *task_grouping);

oolong_task_grouping *
oolong_group(oolong_task_grouping *task_grouping, oolong_task **tasks, int size);

oolong_task **
oolong_ungroup(oolong_task **tasks, oolong_task_grouping *task_grouping);

oolong_task_grouping *
oolong_regroup(oolong_task_grouping *task_grouping);

oolong_task **
oolong_add_task(oolong_task **tasks, int *size, oolong_task *task);

msgpack_sbuffer *
oolong_save(msgpack_sbuffer *sbuf);

msgpack_sbuffer *
oolong_load(msgpack_sbuffer *sbuf);

#endif OOLONG_H
