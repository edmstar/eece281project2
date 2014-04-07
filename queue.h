struct QueueItem {
	char value;
	volatile struct QueueItem* nextItem;
};

struct Queue {
	volatile struct QueueItem* first;
	volatile struct QueueItem* last;
	int count;
};


struct Queue* newQueue();
struct QueueItem* newItem(char c);
struct QueueItem* pop(struct Queue* q);
void append(struct Queue* q, struct QueueItem* item);
