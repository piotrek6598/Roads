/** @file
 * Implementation of module finding best path.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 20.07.2020
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "road.h"
#include "path.h"

bool checkIfFirstPathBetter(path_t *path1, path_t *path2) {
    if (path1->total_len < path2->total_len)
        return true;
    if (path1->total_len > path2->total_len)
        return false;
    return path1->year > path2->year;
}

bool checkIfPathDefinedUnambiguously(Map *map, path_t *path, City *city1,
                                     City *city2) {
    City *curr_city;
    heap_t *heap;
    heap_node_t *heap_node;
    list_t *cities, *roads;
    list_t *tmp_node;

    unsigned new_len;
    int new_year;

    cities = mapGetValuesList(map->cities);
    if (cities == NULL)
        return false;

    heap = createHeap(map->cities_num);
    if (heap == NULL) {
        deleteList(&cities);
        return false;
    }

    tmp_node = cities;
    while (tmp_node->value != NULL)
        ((City *) tmp_node->value)->prev_city = NULL;

    fillHeapWithCitiesFromList(heap, &cities);
    deleteList(&cities);
    decreaseHeapKey(heap, city1->num_in_heap, 0, INT_MIN);
    heap_node = popHeap(heap);
    new_len = 0;
    new_year = INT_MIN;
    assert(heap_node != NULL);

    curr_city = city1;
    tmp_node = path->roads;
    while (strcmp(curr_city->name, city2->name) != 0) {
        Road *curr_road = (Road *) tmp_node->value;
        City *next_city = getNextCity(curr_city, curr_road);
        new_len += curr_road->length;
        new_year = new_year < curr_road->year ? new_year : curr_road->year;

        decreaseHeapKey(heap, next_city->num_in_heap, new_len, new_year);
        next_city->prev_city = curr_city;

        curr_city = next_city;
    }

    curr_city = city1;
    while (strcmp(curr_city->name, city2->name) != 0) {
        roads = mapGetValuesList(curr_city->connected_roads);
        if (roads == NULL) {
            deleteHeap(heap);
            return false;
        }
        assert(roads != NULL);

        tmp_node = roads;
        while (tmp_node->value != NULL) {
            Road *curr_road = (Road *) tmp_node->value;
            City *next_city = getNextCity(curr_city, curr_road);
            new_len = heap_node->total_len + curr_road->length;
            new_year = heap_node->year < curr_road->year ? heap_node->year
                                                         : curr_road->year;

            if (decreaseHeapKey(heap, next_city->num_in_heap, new_len,
                                new_year)) {
                if (next_city->prev_city != curr_city) {
                    deleteHeap(heap);
                    return false;
                }
            }

            tmp_node = tmp_node->next;
        }

        heap_node = popHeap(heap);
        if (heap_node == NULL) {
            deleteHeap(heap);
            return false;
        }
        assert(heap_node != NULL);
        curr_city = heap_node->city;
    }
    deleteHeap(heap);
    return true;
}

path_t *findBestPath(Map *map, City *city1, City *city2) {
    City *curr_city;
    heap_t *heap;
    heap_node_t *heap_node;
    list_t *cities, *roads;
    list_t *tmp_node;
    path_t *path = (path_t *) malloc(sizeof(path_t));

    unsigned new_len;
    int new_year;

    if (path == NULL)
        return NULL;

    cities = mapGetValuesList(map->cities);
    if (cities == NULL) {
        free(path);
        return NULL;
    }

    heap = createHeap(map->cities_num);
    if (heap == NULL) {
        deleteList(&cities);
        free(path);
        return NULL;
    }

    fillHeapWithCitiesFromList(heap, &cities);
    deleteList(&cities);
    decreaseHeapKey(heap, city1->num_in_heap, 0, INT_MIN);
    heap_node = popHeap(heap);
    assert(heap_node != NULL);

    curr_city = city1;
    while (strcmp(curr_city->name, city2->name) != 0) {
        roads = mapGetValuesList(curr_city->connected_roads);
        if (roads == NULL) {
            deleteHeap(heap);
            free(path);
            return NULL;
        }
        assert(roads != NULL);

        tmp_node = roads;
        while (tmp_node->value != NULL) {
            Road *curr_road = (Road *) tmp_node->value;
            City *next_city = getNextCity(curr_city, curr_road);
            new_len = heap_node->total_len + curr_road->length;
            new_year = heap_node->year < curr_road->year ? heap_node->year
                                                         : curr_road->year;

            if (decreaseHeapKey(heap, next_city->num_in_heap, new_len,
                                new_year)) {
                next_city->prev_city = curr_city;
            }

            tmp_node = tmp_node->next;
        }

        heap_node = popHeap(heap);
        if (heap_node == NULL) {
            deleteHeap(heap);
            free(path);
            return NULL;
        }
        assert(heap_node != NULL);
        curr_city = heap_node->city;
    }

    path->total_len = heap_node->total_len;
    path->year = heap_node->year;

    deleteHeap(heap);

    path->roads = newList();
    if (path->roads == NULL) {
        free(path);
        return NULL;
    }

    assert(curr_city == city2);
    while (strcmp(curr_city->name, city1->name) != 0) {
        Road *curr_road = (Road *) mapGet(curr_city->connected_roads,
                                          (void *) curr_city->prev_city->name);
        if (curr_road == NULL) {
            deleteList(&path->roads);
            free(path);
            return NULL;
        }
        if (!addList(&path->roads, (void *) curr_road)) {
            deleteList(&path->roads);
            free(path);
            return NULL;
        }

        curr_city = curr_city->prev_city;
    }
    return path;
}
