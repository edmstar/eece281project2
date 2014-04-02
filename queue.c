#include <stdio.h>
#include <stdlib.h>
#include <at89lp51rd2.h>

// ~C51~ 

#include "queue.h"

struct Queue* newQueue() {
  struct Queue* q;
  q = (struct Queue*) malloc(sizeof(struct Queue));
  q->first = NULL;
  q->count = 0;
  
  return q;
}

struct QueueItem* newItem(struct Queue* q, char c) {
  struct QueueItem* item;

  item = (struct QueueItem*) malloc(sizeof(struct QueueItem));
  
  item->value = c;
  item->nextItem = NULL;

  return item;
}

struct QueueItem* pop(struct Queue* q) {
  struct QueueItem* element;
  
  if (q->first == NULL) {
    return NULL;
  }

  element = q->first;
  q->first = element->nextItem;
  
  return element;
}

void append(struct Queue* q, struct QueueItem* item) {
  struct QueueItem* element;
  
  if (q->first == NULL) {
    q->first = item;
  } else {
    element = q->first;  
    while (element->nextItem != NULL) {
      element = element->nextItem;
    }
    element->nextItem = item;
  }

  q->count = q->count + 1;

}
