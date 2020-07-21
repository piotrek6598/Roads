/** @file
 * Implementation of heap interface.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 21.07.2020
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "heap.h"
#include "utils.h"

/** @brief Swap value in two heap nodes.
 * Updates city num_in_heap field.
 * @param heap [in,out]    - pointer to heap,
 * @param node1 [in]       - number of first node,
 * @param node2 [in]       - number of second node.
 */
static void swapHeapNodes(heap_t *heap, unsigned node1, unsigned node2) {
    heap_node_t tmp_node;

    assert(node1 > 0);
    assert(node2 > 0);
    assert(node1 <= heap->size);
    assert(node2 <= heap->size);

    tmp_node = heap->heap_tab[node1];
    heap->heap_tab[node1] = heap->heap_tab[node2];
    heap->heap_tab[node2] = tmp_node;
    // Updating num_in_heap field only if it is not 0.
    if (heap->heap_tab[node1].city->num_in_heap != 0)
        heap->heap_tab[node1].city->num_in_heap = node1;
    if (heap->heap_tab[node2].city->num_in_heap != 0)
        heap->heap_tab[node2].city->num_in_heap = node2;
}

/** @brief Check if first node is better.
 * First node is better if total length is smaller or total lengths are equal
 * is both nodes and year in first node is not smaller.
 * @param heap [in,out]    - pointer to heap,
 * @param node1 [in]       - number of first node,
 * @param node2 [in]       - number of second node.
 * @return Value @p true if first node is better. Otherwise value @p false.
 */
static inline bool
checkIfFirstNodeBetter(heap_t *heap, unsigned node1, unsigned node2) {
    long long int diff =
            (long long) heap->heap_tab[node1].total_len -
            heap->heap_tab[node2].total_len;
    if (diff < 0)
        return true;
    if (diff > 0)
        return false;
    return heap->heap_tab[node1].year >= heap->heap_tab[node2].year;
}

/** @brief Creates empty heap.
 * New heap has size n - 1. Where n is smallest power of 2 greater than @p size.
 * @param size [in]        - minimal heap size.
 * @return Pointer to new heap or NULL if allocation error occurred.
 */
heap_t *createHeap(size_t size) {
    int heap_size = powerOf2Greater(size);
    heap_t *new_heap = (heap_t *) malloc(sizeof(heap_t));

    if (new_heap == NULL)
        return NULL;

    new_heap->heap_tab = (heap_node_t *) malloc(
            sizeof(heap_node_t) * (heap_size + 1));
    if (new_heap->heap_tab == NULL) {
        free(new_heap);
        return NULL;
    }

    new_heap->size = size;
    return new_heap;
}

/** @brief Destroys heap.
 * @param heap [in]        - pointer to heap.
 */
void deleteHeap(heap_t *heap) {
    if (heap == NULL)
        return;
    free(heap->heap_tab);
    free(heap);
}

/** @brief Removes heap node from top.
 * @param heap [in,out]    - pointer to heap.
 * @return Pointer to removed heap node or NULL if heap is empty or doesn't
 * exists.
 */
heap_node_t *popHeap(heap_t *heap) {
    unsigned curr_node = 1;
    unsigned poss_next_node;

    if (heap == NULL || heap->size == 0)
        return NULL;

    swapHeapNodes(heap, 1, heap->size);
    heap->size--;

    while (true) {
        if (2 * curr_node + 1 <= heap->size) {
            if (checkIfFirstNodeBetter(heap, 2 * curr_node, 2 * curr_node + 1))
                poss_next_node = 2 * curr_node;
            else
                poss_next_node = 2 * curr_node + 1;
        } else if (2 * curr_node <= heap->size) {
            poss_next_node = 2 * curr_node;
        } else {
            break;
        }

        if (!checkIfFirstNodeBetter(heap, curr_node, poss_next_node)) {
            swapHeapNodes(heap, curr_node, poss_next_node);
            curr_node = poss_next_node;
        } else {
            break;
        }
    }
    return &heap->heap_tab[heap->size + 1];
}

/** @brief Decreases key of specified node.
 * If heap with new values was worse, it'd do nothing and returns @p false.
 * Otherwise changes heap node key and returns @p true.
 * @param heap [in,out]    - pointer to heap,
 * @param node [in]        - node's number,
 * @param total_len [in]   - nrw total length,
 * @param year [in]        - new year.
 * @return Value @p true if new key is not worse than current. Otherwise
 * value @p false.
 */
bool
decreaseHeapKey(heap_t *heap, unsigned node, unsigned total_len, int year) {
    if (heap == NULL || node == 0 || node > heap->size)
        return false;
    if (heap->heap_tab[node].total_len < total_len)
        return false;
    if (heap->heap_tab[node].total_len == total_len &&
        heap->heap_tab[node].year > year)
        return false;

    heap->heap_tab[node].total_len = total_len;
    heap->heap_tab[node].year = year;

    while (node > 1) {
        if (!checkIfFirstNodeBetter(heap, node / 2, node)) {
            swapHeapNodes(heap, node / 2, node);
            node /= 2;
        } else {
            break;
        }
    }
    return true;
}

/** @brief Fill heap with cities from list.
 * All cities has default key (total length = INT_MAX, year = UINT_MAX).
 * Sets for every city its number in heap array.
 * Assumes that heap is large enough to store all cities.
 * @param heap [in,out]    - pointer to heap,
 * @param cities_list [in] - double pointer to list of cities.
 */
void fillHeapWithCitiesFromList(heap_t *heap, list_t **cities_list) {
    list_t *tmp_node = *cities_list;
    unsigned i = 1;

    while (tmp_node != NULL && tmp_node->value != NULL) {
        heap->heap_tab[i].city = (City *) tmp_node->value;
        heap->heap_tab[i].city->num_in_heap = i;
        heap->heap_tab[i].year = INT_MAX;
        heap->heap_tab[i].total_len = UINT_MAX;
        tmp_node = tmp_node->next;
        i++;
    }
}