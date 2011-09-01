/* (C) 2005 Luke Macpherson */

#define HEAP_DATA_T uint32_t

struct heap_node{
        uint32_t priority;
        HEAP_DATA_T data;
};

struct heap{
	uint32_t size;
	uint32_t entries;
	struct heap_node *n;
};

struct heap * heap_init(uint32_t size);

/* insert entry onto the heap */
int heap_insert(struct heap *heap, uint32_t priority, HEAP_DATA_T data);

/* return the lowest (integer) priority */
int heap_delete(struct heap *heap, uint32_t *priority, HEAP_DATA_T *data);
