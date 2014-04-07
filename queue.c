struct Queue* newQueue() {
  struct Queue* q;
  q = (struct Queue*) malloc(sizeof(struct Queue));
  q->first = NULL;
  q->last = NULL;
  q->count = 0;
  
  return q;
}

struct QueueItem* newItem(char c) {
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
  q->count--;
  
  return element;
}

void append(struct Queue* q, struct QueueItem* item) {
  struct QueueItem* element;
  
  if (q->first == NULL) {
    q->first = item;
    q->last = item;
  } else {
    element = q->last;
    element->nextItem = item;
    q->last = item;
  }

  q->count = q->count + 1;

}