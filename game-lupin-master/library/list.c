#include "list.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct list {
  void **data;
  size_t length;
  size_t capacity;
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);
  list->data = malloc(sizeof(void *) * initial_size);
  assert(list->data != NULL);
  list->length = 0;
  list->capacity = initial_size;
  list->freer = freer;

  return list;
}

void list_append(list_t *list1, list_t *list2) {
  for (size_t i = 0; i < list_size(list2); i++) {
    list_add(list1, list_get(list2, i));
  }
  free(list2->data);
  free(list2);
}

bool list_contains(list_t *list, void *val) {
  for (int i = 0; i < list_size(list); i++) {
    if (list_get(list, i) == val) {
      return true;
    }
  }
  return false;
}

void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (int i = 0; i < list_size(list); i++) {
      list->freer(list_get(list, i));
    }
  }
  free(list->data);
  free(list);
}

size_t list_size(list_t *list) { return list->length; }

void *list_get(list_t *list, size_t index) {
  assert(index < list_size(list));
  return list->data[index];
}

void *list_remove(list_t *list, size_t index) {
  assert(index < list_size(list));
  void *value = list_get(list, index);
  for (size_t i = index; i < list_size(list) - 1; i++) {
    list->data[i] = list->data[i + 1];
  }
  list->length--;
  return value;
}

void *list_remove_last(list_t *list) {
  assert(list_size(list) > 0);
  return list_remove(list, list_size(list) - 1);
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);
  // if the size is too small, add more
  if (list_size(list) == list->capacity) {
    size_t new_cap = (list->capacity * 3) / 2 + 1;
    list->data = realloc(list->data, sizeof(void *) * new_cap);
    assert(list != NULL); // asserts that the resize succeeded
    list->capacity = new_cap;
  }
  list->data[list->length] = value;
  list->length++;
}

list_t *deepcopy(list_t *og, copy_func_t copy) {
  list_t *res = list_init(list_size(og), og->freer);
  for (int i = 0; i < list_size(og); i++) {
    list_add(res, copy(list_get(og, i)));
  }
  return res;
}