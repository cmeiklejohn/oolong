#include <stdlib.h>
#include <check.h>
#include <time.h>
#include <msgpack.h>
#include "../src/oolong.h"

/* Generate seed task data. */
oolong_task **
seed_tasks(oolong_task **tasks, int size) {
  int i;

  if((tasks = malloc(sizeof(oolong_task *) * size))) {
    for(i = 0; i < size; i++) {
      if(tasks[i] = malloc(sizeof(oolong_task))) {
        tasks[i]->description = malloc(sizeof(char) * 6);
        strcpy(tasks[i]->description, "Hello");
        tasks[i]->completed = 1;
        tasks[i]->due = time(&tasks[i]->due + 86400);
      }
    }
  }

  for(i = 0; i < size; i++) {
    fail_unless(tasks[i]->completed == 1);
    fail_unless(tasks[i]->due > 0);
    fail_unless(strcmp(tasks[i]->description, "Hello") == 0);
  }

  return tasks;
}

/* Create a bunch of data, serialize and deserialize. */
START_TEST(test_group_and_ungroup) {
  int i, size = 10;
  oolong_task **tasks = NULL, **newtasks = NULL;
  oolong_task_grouping *task_grouping = NULL;
  msgpack_sbuffer *sbuf = msgpack_sbuffer_new();

  task_grouping = oolong_create_grouping(task_grouping);
  tasks = seed_tasks(tasks, size);
  task_grouping = oolong_group(task_grouping, tasks, size);

  fail_unless(task_grouping->completed_size == size);

  for(i = 0; i < size; i++) {
    fail_unless(tasks[i]->completed == task_grouping->completed[i]->completed);
    fail_unless(tasks[i]->due == task_grouping->completed[i]->due);
    fail_unless(strcmp(tasks[i]->description, task_grouping->completed[i]->description) == 0);
  }

  newtasks = oolong_ungroup(tasks, task_grouping);

  for(i = 0; i < size; i++) {
    fail_unless(tasks[i]->completed == newtasks[i]->completed);
    fail_unless(tasks[i]->due == newtasks[i]->due);
    fail_unless(strcmp(tasks[i]->description, newtasks[i]->description) == 0);
  }
} END_TEST

/* Create a bunch of data, serialize and deserialize. */
START_TEST(test_serialize_and_deserialize) {
  int i, size = 10;
  oolong_task **tasks = NULL, **newtasks = NULL;
  msgpack_sbuffer *sbuf = msgpack_sbuffer_new();

  tasks = seed_tasks(tasks, size);
  sbuf = oolong_serialize(sbuf, tasks, size);
  newtasks = oolong_deserialize(newtasks, sbuf);

  for(i = 0; i < size; i++) {
    fail_unless(tasks[i]->completed == newtasks[i]->completed);
    fail_unless(tasks[i]->due == newtasks[i]->due);
    fail_unless(strcmp(tasks[i]->description, newtasks[i]->description) == 0);
  }
} END_TEST

/* Test suite. */
Suite *
oolong_suite(void) {
  Suite *s = suite_create("Oolong");

  TCase *tc_serialization = tcase_create("Serialization");
  tcase_add_test(tc_serialization, test_serialize_and_deserialize);
  suite_add_tcase(s, tc_serialization);

  TCase *tc_grouping = tcase_create("Grouping");
  tcase_add_test(tc_grouping, test_group_and_ungroup);
  suite_add_tcase(s, tc_grouping);

  return s;
}

/* Run the test suite. */
int main(void) {
  int number_failed;
  Suite *s = oolong_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
