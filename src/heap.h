/** @file
 * Interface of class representing heap.
 * Heap is used as priority queue in Dijkstra algorithm. Cities marked with
 * field num_in_heap = 0 are excluded from priority queue. Cities with that
 * value cannot be removed from top.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 21.07.2020
 */

#ifndef ROADS_HEAP_H
#define ROADS_HEAP_H

#include "roads_types.h"

heap_t *createHeap(size_t size);

void deleteHeap(heap_t *heap);

heap_node_t * popHeap(heap_t *heap);

bool decreaseHeapKey(heap_t *heap, unsigned node, unsigned total_len, int year);

void fillHeapWithCitiesFromList(heap_t *heap, list_t **cities_list);

#endif //ROADS_HEAP_H
