#include <msgpack.h>
#include <time.h>
#include "oolong.h"

/* Provides a series of serialization, deserialization and storage
 * methods for tasks using the messagepack serialization format.
 *
 * Author: Christopher Meiklejohn (christopher.meiklejohn@gmail.com)
 */

/* Given a series of tasks, serialize into a series of array objects
 * using messagepack and return pointer to the buffer containing the
 * serialized data
 */
msgpack_sbuffer *
oolong_serialize(msgpack_sbuffer *sbuf, oolong_task **tasks, int size) {
  int i;
  oolong_task *task;
  msgpack_packer *pk = msgpack_packer_new(sbuf, msgpack_sbuffer_write);

  for(i = 0; i < size; i++) {
    task = tasks[i];

    /* Begin array packing */
    msgpack_pack_array(pk, 3);

    /* Pack description */
    msgpack_pack_raw(pk, sizeof(task->description) + 1);
    msgpack_pack_raw_body(pk, task->description, sizeof(task->description) + 1);

    /* Pack completed */
    msgpack_pack_int(pk, task->completed);

    /* Pack due */
    msgpack_pack_int(pk, task->due);
  }

  msgpack_packer_free(pk);

  return sbuf;
}

/* Given a messagepack buffer, unpack task structs and return pointer
 * to the list of unpacked tasks.
 */
oolong_task **
oolong_deserialize(oolong_task **tasks, msgpack_sbuffer *sbuf) {
  int i = 0;
  bool success;
  size_t offset = 0;
  msgpack_object obj;
  msgpack_object_raw raw;
  msgpack_object_array arr;
  msgpack_unpacked msg;
  msgpack_unpacked_init(&msg);

  /* Every time we successfully unpack a task, realloc our tasks array
   * to hold the new member
   */
  while((success = msgpack_unpack_next(&msg, sbuf->data, sbuf->size, &offset))) {
    /* Unpack the task and malloc the required memory */
    if((tasks = realloc(tasks, sizeof(oolong_task *) * (i + 1))) != NULL) {
      if((tasks[i] = malloc(sizeof(oolong_task)))) {
        obj = msg.data;
        arr = obj.via.array;

        raw = arr.ptr[0].via.raw;
        tasks[i]->description = malloc(raw.size);
        strncpy(tasks[i]->description, raw.ptr, raw.size);
        tasks[i]->completed = arr.ptr[1].via.i64;
        tasks[i]->due = arr.ptr[2].via.i64;
      }
    } else {
      fprintf(stderr, "Cannot reallocate space.\n");
      exit(EXIT_FAILURE);
    }

    i++;
  }

  return tasks;
}

/* Given a list, and a task, add it and allocate appropriate memory. */
oolong_task **
oolong_group_individual(oolong_task **tasks, int *size, oolong_task *task) {
  if((tasks = realloc(tasks, sizeof(oolong_task *) * (*size + 1)))) {
    tasks[*size] = task;
    (*size)++;
  } else {
    fprintf(stderr, "Cannot reallocate space.\n");
    exit(EXIT_FAILURE);
  }

  return tasks;
}

/* Initialize a new task grouping. */
oolong_task_grouping *
oolong_create_grouping(oolong_task_grouping *task_grouping) {
  if((task_grouping = malloc(sizeof(oolong_task_grouping)))) {
    task_grouping->completed = NULL;
    task_grouping->today = NULL;
    task_grouping->next = NULL;

    task_grouping->completed_size = 0;
    task_grouping->today_size = 0;
    task_grouping->next_size = 0;
  } else {
    fprintf(stderr, "Cannot allocate space.\n");
    exit(EXIT_FAILURE);
  }

  return task_grouping;
}

/* Given an array of tasks, group into appropriate groupings
 * for display.  Returns a pointer to the grouping object.
 */
oolong_task_grouping *
oolong_group(oolong_task_grouping *task_grouping, oolong_task **tasks, int size) {
  int i;
  oolong_task *task;
  time_t curtime;

  time(&curtime);

  for(i = 0; i < size; i++) {
    task = tasks[i];

    if(task->completed) {
      /* If the task has been completed, dump into the completed list */
      task_grouping->completed = oolong_group_individual(task_grouping->completed, &task_grouping->completed_size, task);
    } else if(task->due < curtime + 86400){
      /* If the task is due within the next 86400 seconds, assume it's
       * due today or overdue.
       */
      task_grouping->today = oolong_group_individual(task_grouping->today, &task_grouping->today_size, task);
    } else {
      /* Else, assume it's due in the future */
      task_grouping->next = oolong_group_individual(task_grouping->next, &task_grouping->next_size, task);
    }
  }

  return task_grouping;
}

/* Given a task grouping, build a list of all of the tasks within
 * the grouping.
 */
oolong_task **
oolong_ungroup(oolong_task **tasks, oolong_task_grouping *task_grouping) {
  int i = 0, j = 0, size = 0;

  size += task_grouping->today_size;
  size += task_grouping->next_size;
  size += task_grouping->completed_size;

  /* Potential problem if we reallocate space containing pointers
   * and don't free the pointers.
   */
  if((tasks = realloc(tasks, sizeof(oolong_task *) * size))) {
    for(j = 0; j < task_grouping->today_size; j++, i++) {
      tasks[i] = task_grouping->today[j];
    }

    for(j = 0; j < task_grouping->next_size; j++, i++) {
      tasks[i] = task_grouping->next[j];
    }

    for(j = 0; j < task_grouping->completed_size; j++, i++) {
      tasks[i] = task_grouping->completed[j];
    }

    return tasks;
  } else {
    fprintf(stderr, "Cannot reallocate space.\n");
    exit(EXIT_FAILURE);
  }
}

/* Given a task grouping, regroup tasks.  Returns a pointer to the
 * grouping.
 */
oolong_task_grouping *
regroup(oolong_task_grouping *task_grouping) {
  oolong_task **tasks = NULL;

  if((tasks = oolong_ungroup(tasks, task_grouping))) {
    task_grouping = oolong_group(task_grouping, tasks, sizeof(tasks) / sizeof(oolong_task *));
  } else {
    fprintf(stderr, "Cannot regroup tasks.");
  }

  return task_grouping;
}

/* Given a messagepack sbuffer, write the contents to the disk.
 */
msgpack_sbuffer *
oolong_save(msgpack_sbuffer *sbuf);

/* Given a messagepack sbuffer, load the contents of the object on
 * disk into it.
 */
msgpack_sbuffer *
oolong_load(msgpack_sbuffer *sbuf);
