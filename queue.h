
struct QueueItem {
	char value;
	struct QueueItem* nextItem;
};

struct Queue {
	struct QueueItem* first;
	int count;
};


struct Queue* newQueue();
struct QueueItem* newItem(struct Queue* q, char c);
struct QueueItem* pop(struct Queue* q);
void append(struct Queue* q, struct QueueItem* item);
