/* binary heap implementation
 * (c) 2005 Luke Macpherson
 * O(log(n)) insert and delete
 * Refer to the following URL for a description:
 * http://en.wikipedia.org/wiki/Binary_heap
 */

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "heap.h"

#define PARENT(n) ((n-1)/2) /* parent of n */
#define   LEFT(n) ((2*n)+1) /* left child of n */
#define  RIGHT(n) ((2*n)+2) /* right child of n */

static inline void
swap(struct heap *heap, uint32_t x, uint32_t y){
	struct heap_node tmp;
	tmp = heap->n[x];
	heap->n[x] = heap->n[y];
	heap->n[y] = tmp;
}

struct heap *
heap_init(uint32_t size){
	struct heap *heap;

	heap = malloc(sizeof(struct heap) +
	              (sizeof(struct heap_node)*size));
	if(heap==NULL){
		return NULL;
	}
	heap->size = size;
	heap->entries = 0;
	heap->n = (struct heap_node*)&heap[1];

	return heap;
}

/* insert entry onto the heap */
/* returns 0 on success, -1 on error */
int
heap_insert(struct heap *heap, uint32_t priority, HEAP_DATA_T data){
	uint32_t index, parent;

	if(heap->entries>=heap->size){
		return -1;
	}

	index = heap->entries;

	/* insert in last position */
	heap->n[index].priority = priority;
	heap->n[index].data = data;

	/* re-order the heap */
	while(index>0){
		parent = PARENT(index);
		if(heap->n[index].priority < heap->n[parent].priority){
			swap(heap, index, parent);
			index = parent;
		}else{
			break;
		}
	}

	heap->entries++;

	return 0;
}

/* get the lowest priority off heap */
/* returns 0 on success, -1 on error */
int
heap_delete(struct heap *heap, uint32_t *priority, HEAP_DATA_T *data){
	uint32_t index, left, right, min;

	if(heap->entries==0){
		return -1;
	}

	*priority = heap->n[0].priority;
	*data = heap->n[0].data;
	heap->entries--;

	/* move tail to root */
	index = 0;
	heap->n[index] = heap->n[heap->entries];

	/* reorder the tree */
	while(1){
		left = LEFT(index);
		right = RIGHT(index);

		if(left>=heap->entries && right>=heap->entries)
			break;

		if(left<heap->entries && right<heap->entries){
			if(heap->n[left].priority<heap->n[right].priority){
				min = left;
			}else{
				min = right;
			}
		}else{
			if(left<heap->entries){
				min = left;
			}else{
				min = right;
			}
		}

		if(heap->n[index].priority>heap->n[min].priority){
			swap(heap, index, min);
			index = min;
		}else{
			break;
		}
	}

	return 0;
}
