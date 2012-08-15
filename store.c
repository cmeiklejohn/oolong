#include <msgpack.h>

/* Provides a series of serialization and deserialization methods for
 * tasks using the messagepack serialization format.
 *
 * Author: Christopher Meiklejohn (christopher.meiklejohn@gmail.com)
 */

/* Define the task stucture */
typedef struct task {
  char *description;
  int completed;
  int due;
} task;

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
  int len;
  bool success;
  size_t offset = 0;
  msgpack_object obj;
  msgpack_object_array arr;
  msgpack_unpacked msg;
  msgpack_unpacked_init(&msg);

  /* Every time we successfully unpack a task, realloc our tasks array
   * to hold the new member
   */
  while(success = msgpack_unpack_next(&msg, sbuf->data, sbuf->size, &offset)) {
    /* Unpack the task and malloc the required memory */
    if((tasks = realloc(tasks, sizeof(task *) * (i + 1))) != NULL) {
      if(tasks[i] = malloc(sizeof(task))) {
        obj = msg.data;
        arr = obj.via.array;

        len = arr.ptr[0].via.raw.size;
        tasks[i]->description = malloc(len);
        strncpy(tasks[i]->description, arr.ptr[0].via.raw.ptr, len);
        tasks[i]->completed = arr.ptr[1].via.i64;
        tasks[i]->due = arr.ptr[2].via.i64;
      }
    }

    i++;
  }

  return tasks;
}
