#ifndef SEMNOME_INDEXEDHEAP_H
#define SEMNOME_INDEXEDHEAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t ID;
    double value;
} HeapElement;

typedef struct {
    uint64_t capacity;
    uint64_t size;
    HeapElement *elements;
    int64_t *positions;
} IndexedHeap;

IndexedHeap* createIndexedHeap(uint64_t capacity);
void freeIndexedHeap(IndexedHeap *heap);
void insertIndexedHeap(IndexedHeap *heap, HeapElement element);
void maxHeapify(IndexedHeap *heap, uint64_t index);
HeapElement extractMax(IndexedHeap *heap);
void update(IndexedHeap *heap, uint64_t id, double new_value);
bool isEmpty(const IndexedHeap *heap);
void removeIndexedHeap(IndexedHeap *heap, uint64_t id);
HeapElement findMax(const IndexedHeap *heap);

#endif //SEMNOME_INDEXEDHEAP_H
