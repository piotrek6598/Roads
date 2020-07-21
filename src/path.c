/** @file
 * Implementation of module finding best path.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 21.07.2020
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "road.h"
#include "path.h"

/** @brief Makes cities invisible for Dijkstra algorithm.
 * Sets num_in_heap field to 0 for every city that is end of any road from list,
 * but not @p city1 and @p city2.
 * @param roads [in,out]     - pointer to roads list,
 * @param city1 [in,out]     - pointer to first city,
 * @param city2 [in,out]     - pointer to last city.
 */
static void
excludeCitiesFromRoadLists(list_t **roads, City *city1, City *city2) {
    list_t *tmp_node = *roads;
    Road *curr_road;
    while (tmp_node != NULL && tmp_node->value != NULL) {
        curr_road = (Road *) tmp_node->value;
        if (curr_road->city1 != city1 && curr_road->city1 != city2)
            curr_road->city1->num_in_heap = 0;
        if (curr_road->city2 != city1 && curr_road->city2 != city2)
            curr_road->city2->num_in_heap = 0;
        tmp_node = tmp_node->next;
    }
}

/** @brief Checks if first path is better.
 * First path is better if is shorter all has equal length and oldest road
 * is newer.
 * @param path1 [in,out]     - pointer to first path,
 * @param path2 [in,out]     - pointer to second path.
 * @return Value @p true if first path is better. Otherwise value @p false.
 */
bool checkIfFirstPathBetter(path_t *path1, path_t *path2) {
    if (path1->total_len < path2->total_len)
        return true;
    if (path1->total_len > path2->total_len)
        return false;
    return path1->year > path2->year;
}

/** @brief Checks if path was selected unambiguously.
 * Assumes that @p map, @p city1, @p city2, @p exclude_roads, @p direct are
 * the same as in @ref findBestPath function which returned @p path.
 * Works like Dijkstra algorithm, but first sets distance for cities from path
 * and only for this city, previous city is set. Then if Dijkstra algorithm
 * enters city with previous city set from city without previous city set and
 * path is just so good (can't be better by definition of path), means that
 * path is ambiguous.
 * @param map [in,out]       - pointer to map,
 * @param path [in,out]      - pointer to path,
 * @param city1 [in,out]     - pointer to first city,
 * @param city2 [in,out]     - pointer to last city,
 * @param exclude_roads [in] - list of excluded roads and cities,
 * @param direct [in]        - flag indicating if direct road can be used.
 * @return Value @p true if path was selected unambiguously.
 * Otherwise value @p false.
 */
bool checkIfPathDefinedUnambiguously(Map *map, path_t *path, City *city1,
                                     City *city2, list_t **exclude_roads,
                                     bool direct) {
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
    while (tmp_node != NULL && tmp_node->value != NULL) {
        ((City *) tmp_node->value)->prev_city = NULL;
        tmp_node = tmp_node->next;
    }

    fillHeapWithCitiesFromList(heap, &cities);
    deleteList(&cities);
    if (exclude_roads != NULL)
        excludeCitiesFromRoadLists(exclude_roads, city1, city2);
    decreaseHeapKey(heap, city1->num_in_heap, 0, INT_MAX);
    heap_node = popHeap(heap);
    new_len = 0;
    new_year = INT_MAX;
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
        tmp_node = tmp_node->next;
    }

    curr_city = city1;
    while (strcmp(curr_city->name, city2->name) != 0) {
        if ((heap_node->total_len == UINT_MAX && heap_node->year == INT_MAX) ||
            curr_city->num_in_heap == 0) {
            deleteHeap(heap);
            return false;
        }
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

            if (!((curr_city == city1 && next_city == city2) ||
                  (curr_city == city2 && next_city == city1)) || direct) {
                if (next_city->num_in_heap != 0) {
                    if (decreaseHeapKey(heap, next_city->num_in_heap, new_len,
                                        new_year)) {
                        if (next_city->prev_city != NULL &&
                            next_city->prev_city != curr_city) {
                            deleteList(&roads);
                            deleteHeap(heap);
                            return false;
                        }
                    }
                }
            }

            tmp_node = tmp_node->next;
        }
        deleteList(&roads);
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

/** @brief Finds best path between two cities.
 * Find best path from @p city1 to @p city2. All cities other than @p city1
 * and @p city2 that are end of any road in @p exclude_roads, can't be in path.
 * If flag @p direct is set to false, direct road from @p city1 to @p city2
 * can't be used. Works using Dijkstra algorithm.
 * @param map [in,out]       - pointer to map,
 * @param city1 [in,out]     - pointer to first city,
 * @param city2 [in,out]     - pointer to last city,
 * @param exclude_roads [in] - list of excluded roads and cities,
 * @param direct [in]        - flag indicating if direct road can be used.
 * @return Pointer to path or NULL if allocation error occurred or @p city2
 * is not reachable from @p city1.
 */
path_t *findBestPath(Map *map, City *city1, City *city2, list_t **exclude_roads,
                     bool direct) {
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
    if (exclude_roads != NULL)
        excludeCitiesFromRoadLists(exclude_roads, city1, city2);
    decreaseHeapKey(heap, city1->num_in_heap, 0, INT_MAX);
    heap_node = popHeap(heap);
    assert(heap_node != NULL);

    curr_city = city1;
    while (strcmp(curr_city->name, city2->name) != 0) {
        if ((heap_node->total_len == UINT_MAX && heap_node->year == INT_MAX) ||
            curr_city->num_in_heap == 0) {
            deleteHeap(heap);
            free(path);
            return false;
        }
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

            if (!((curr_city == city1 && next_city == city2) ||
                  (curr_city == city2 && next_city == city1)) || direct) {
                if (next_city->num_in_heap != 0) {
                    if (decreaseHeapKey(heap, next_city->num_in_heap, new_len,
                                        new_year)) {
                        next_city->prev_city = curr_city;
                    }
                }
            }

            tmp_node = tmp_node->next;
        }

        deleteList(&roads);
        heap_node = popHeap(heap);
        if (heap_node == NULL) {
            deleteHeap(heap);
            free(path);
            return NULL;
        }
        assert(heap_node != NULL);
        curr_city = heap_node->city;
    }

    if ((heap_node->total_len == UINT_MAX && heap_node->year == INT_MAX) ||
        curr_city->num_in_heap == 0) {
        deleteHeap(heap);
        free(path);
        return false;
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
