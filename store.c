#include <msgpack.h>
#include "store.h"

/* Provides a series of serialization, deserialization and storage
 * methods for tasks using the messagepack serialization format.
 *
 * Author: Christopher Meiklejohn (christopher.meiklejohn@gmail.com)
 */

/* Given a series of tasks, serialize into a series of array objects
 * using messagepack and return pointer to the buffer containing the
 * serialized data
 */
msgpack_sbuffer *serialize(msgpack_sbuffer *sbuf, task **tasks, int size) {
  int i;
  task *task;
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
task **deserialize(task **tasks, msgpack_sbuffer *sbuf) {
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
    if((tasks = realloc(tasks, sizeof(task *) * (i + 1))) != NULL) {
      if((tasks[i] = malloc(sizeof(task)))) {
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

/* Given an array of tasks, group into appropriate groupings
 * for display.  Returns a pointer to the grouping object.
 */
task_grouping *group(task_grouping *task_grouping, task **tasks) {
  return task_grouping;
}

/* Given a task grouping, build a list of all of the tasks within
 * the grouping.
 */
task **ungroup(task **tasks, task_grouping *task_grouping) {
  int i = 0, j = 0, size = 0;

  size += task_grouping->today_size;
  size += task_grouping->next_size;
  size += task_grouping->completed_size;

  /* Potential problem if we reallocate space containing pointers
   * and don't free the pointers.
   */
  if((tasks = realloc(tasks, sizeof(task) * size))) {
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
task_grouping *regroup(task_grouping *task_grouping) {
  task **tasks = NULL;

  if(ungroup(tasks, task_grouping)) {
    group(task_grouping, tasks);
  } else {
    fprintf(stderr, "Failed to reorganize tasks.\n");
    exit(EXIT_FAILURE);
  }

  return task_grouping;
}

/* Given a messagepack sbuffer, write the contents to the disk.
 */
msgpack_sbuffer *save(msgpack_sbuffer *sbuf);

/* Given a messagepack sbuffer, load the contents of the object on
 * disk into it.
 */
msgpack_sbuffer *load(msgpack_sbuffer *sbuf);
