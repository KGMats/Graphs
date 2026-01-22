#include <stdlib.h>
#include <math.h> // For fabs
#include <float.h> // For DBL_EPSILON
#include "IndexedHeap.h"

static void swap(IndexedHeap *heap, uint64_t i, uint64_t j) {
    heap->positions[heap->elements[i].ID] = j;
    heap->positions[heap->elements[j].ID] = i;

    HeapElement temp = heap->elements[i];
    heap->elements[i] = heap->elements[j];
    heap->elements[j] = temp;
}

IndexedHeap* createIndexedHeap(uint64_t capacity) {
    IndexedHeap *heap = (IndexedHeap*)malloc(sizeof(IndexedHeap));
    heap->capacity = capacity;
    heap->size = 0;
    heap->elements = (HeapElement*)malloc(sizeof(HeapElement) * capacity);
    heap->positions = (int64_t*)malloc(sizeof(int64_t) * capacity);
    for (uint64_t i = 0; i < capacity; ++i) {
        heap->positions[i] = -1;
    }
    return heap;
}

void freeIndexedHeap(IndexedHeap *heap) {
    if (heap) {
        free(heap->elements);
        free(heap->positions);
        free(heap);
    }
}

void insertIndexedHeap(IndexedHeap *heap, HeapElement element) {
    if (heap->size == heap->capacity) {
        // Heap is full
        return;
    }

    uint64_t i = heap->size++;
    heap->elements[i] = element;
    heap->positions[element.ID] = i;

    while (i != 0 && (heap->elements[(i - 1) / 2].value < heap->elements[i].value || (fabs(heap->elements[(i - 1) / 2].value - heap->elements[i].value) < DBL_EPSILON && heap->elements[(i - 1) / 2].ID > heap->elements[i].ID))) {
        swap(heap, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}

void maxHeapify(IndexedHeap *heap, uint64_t index) {
    uint64_t largest = index;
    uint64_t left = 2 * index + 1;
    uint64_t right = 2 * index + 2;

    if (left < heap->size && (heap->elements[left].value > heap->elements[largest].value || (fabs(heap->elements[left].value - heap->elements[largest].value) < DBL_EPSILON && heap->elements[left].ID < heap->elements[largest].ID))) {
        largest = left;
    }

    if (right < heap->size && (heap->elements[right].value > heap->elements[largest].value || (fabs(heap->elements[right].value - heap->elements[largest].value) < DBL_EPSILON && heap->elements[right].ID < heap->elements[largest].ID))) {
        largest = right;
    }

    if (largest != index) {
        swap(heap, index, largest);
        maxHeapify(heap, largest);
    }
}

HeapElement extractMax(IndexedHeap *heap) {
    if (heap->size == 0) {
        return (HeapElement){ .ID = (uint64_t)-1, .value = -1.0 };
    }

    HeapElement max_element = heap->elements[0];
    heap->positions[max_element.ID] = -1;

    if (heap->size > 1) {
        heap->elements[0] = heap->elements[heap->size - 1];
        heap->positions[heap->elements[0].ID] = 0;
        heap->size--;
        maxHeapify(heap, 0);
    } else {
        heap->size--;
    }


    return max_element;
}


void update(IndexedHeap *heap, uint64_t id, double new_value) {
    int64_t pos = heap->positions[id];
    if (pos == -1) return;

    heap->elements[pos].value = new_value;

    if (pos > 0 && (heap->elements[(pos - 1) / 2].value < heap->elements[pos].value || (fabs(heap->elements[(pos - 1) / 2].value - heap->elements[pos].value) < DBL_EPSILON && heap->elements[(pos - 1) / 2].ID > heap->elements[pos].ID))) {
        // bubble up
        while (pos != 0 && (heap->elements[(pos - 1) / 2].value < heap->elements[pos].value || (fabs(heap->elements[(pos - 1) / 2].value - heap->elements[pos].value) < DBL_EPSILON && heap->elements[(pos - 1) / 2].ID > heap->elements[pos].ID))) {
            swap(heap, pos, (pos - 1) / 2);
            pos = (pos - 1) / 2;
        }
    } else {
        // bubble down
        maxHeapify(heap, pos);
    }
}

void removeIndexedHeap(IndexedHeap *heap, uint64_t id) {
    int64_t pos = heap->positions[id];
    if (pos == -1) return; // Not in heap

    // Move the last element to the position of the element to be removed
    heap->elements[pos] = heap->elements[heap->size - 1];
    heap->positions[heap->elements[pos].ID] = pos;
    heap->positions[id] = -1; // Mark as removed

    heap->size--;

        if (pos < heap->size) {
            if (pos > 0 && (heap->elements[(pos - 1) / 2].value < heap->elements[pos].value || (fabs(heap->elements[(pos - 1) / 2].value - heap->elements[pos].value) < DBL_EPSILON && heap->elements[(pos - 1) / 2].ID > heap->elements[pos].ID))) {
                // Bubble up
                while (pos != 0 && (heap->elements[(pos - 1) / 2].value < heap->elements[pos].value || (fabs(heap->elements[(pos - 1) / 2].value - heap->elements[pos].value) < DBL_EPSILON && heap->elements[(pos - 1) / 2].ID > heap->elements[pos].ID))) {
                    swap(heap, pos, (pos - 1) / 2);
                    pos = (pos - 1) / 2;
                }
            } else {
                // Bubble down
                maxHeapify(heap, pos);
            }
        }}

bool isEmpty(const IndexedHeap *heap) {
    return heap->size == 0;
}

HeapElement findMax(const IndexedHeap *heap) {
    if (heap->size == 0)
    {
        return (HeapElement) { .ID = (uint64_t)-1, .value = -1.0 } ;
    }
    
    return heap->elements[0];
}

