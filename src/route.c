/** @file
 * Implementation of class representing route.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "road.h"
#include "route.h"
#include "utils.h"

/** @brief Creates new route with specified number.
 * Creates route with number @p routeId from @p city1 to @p city2 made by
 * specified roads (@p roads).
 * @param routeId [in]    - route number,
 * @param city1 [in]      - pointer to first city,
 * @param city2 [in]      - pointer to last city,
 * @param roads [in]      - list of roads which make this route.
 * @return Pointer to new route or NULL if allocation error occurred.
 */
Route *createRoute(unsigned routeId, City *city1, City *city2, list_t *roads) {
    Route *new_route = (Route *) malloc(sizeof(Route));

    if (new_route == NULL)
        return NULL;

    new_route->routeId = routeId;
    new_route->firstCity = city1;
    new_route->lastCity = city2;
    new_route->roads = roads;
    return new_route;
}

/** @brief Deletes specified route.
 * Doesn't delete cities or roads.
 * @param route [in]      - pointer to route.
 */
void deleteRoute(Route *route) {
    if (route == NULL)
        return;

    deleteList(&route->roads);
    free(route);
}

/** @brief Replaces road by list of other roads.
 * Replaces specified road in specified route with list of other roads.
 * Assumes that after replacing route will be coherent.
 * @param route [in,out]  – pointer to route,
 * @param road [in]       - pointer to road,
 * @param roads [in]      - list of substituting roads.
 * @return Value @p true if road was replaced. Otherwise value @p false.
 */
bool replaceRoad(Route *route, Road *road, list_t **roads) {
    list_t *tmp_node = route->roads;
    list_t *tmp_node2;
    list_t *prev = NULL;

    while (tmp_node != NULL) {
        if (tmp_node->value == (void *) road) {
            tmp_node2 = tmp_node->next;
            free(tmp_node);
            if (prev != NULL) {
                prev->next = *roads;
            } else {
                route->roads = *roads;
            }
            tmp_node = *roads;
            while (tmp_node->next != NULL && tmp_node->next->value != NULL)
                tmp_node = tmp_node->next;
            free(tmp_node->next);
            tmp_node->next = tmp_node2;
            return true;
        }
        prev = tmp_node;
        tmp_node = tmp_node->next;
    }
    return false;
}

/** @brief Extends route in specified direction.
 * Updates first or last city.
 * @param route [in,out]  – pointer to route,
 * @param roads [in]      – list of extending roads,
 * @param from_last [in]  – flag indicating if route is extended from last city.
 * @return Value @p true if route was extended. Otherwise value @p false.
 */
bool extendRouteInDirection(Route *route, list_t **roads, City *new_city,
                            bool from_last) {
    list_t *tmp_node;
    if (from_last) {
        tmp_node = route->roads;
        while (tmp_node->next != NULL && tmp_node->next->value != NULL)
            tmp_node = tmp_node->next;
        free(tmp_node->next);
        tmp_node->next = *roads;
        route->lastCity = new_city;
        return true;
    } else {
        tmp_node = *roads;
        while (tmp_node->next != NULL && tmp_node->next->value != NULL)
            tmp_node = tmp_node->next;
        free(tmp_node->next);
        tmp_node->next = route->roads;
        route->roads = *roads;
        route->firstCity = new_city;
        return true;
    }
}

/** @brief Fill given buffer with route description.
 * Assumes that buffer is large enough to contain description.
 * Description is in format:
 * route number;city name;length of road;built/repair year;city name;...;
 * city name.
 * Cities are in order so that cities @p city1 and @p city2 used in @ref newRoute
 * function was in the same order.
 * @param route [in,out]  – pointer to route,
 * @param buffer [in,out] – pointer to buffer with description.
 */
void fillRouteDescription(Route *route, char *buffer) {
    size_t offset = 0;
    size_t len;
    City *curr_city;
    list_t *tmp_node;
    Road *curr_road;

    len = getNumberLength(route->routeId);
    snprintf(buffer, len + 2, "%d;", route->routeId);
    offset += len + 1;

    curr_city = route->firstCity;
    tmp_node = route->roads;
    while (curr_city != route->lastCity) {
        assert(tmp_node != NULL);
        curr_road = (Road *) tmp_node->value;

        assert(curr_road != NULL);

        len = strlen(curr_city->name);
        snprintf(buffer + offset, len + 2, "%s;",
                 curr_city->name);
        offset += len + 1;

        len = getNumberLength(curr_road->length);
        snprintf(buffer + offset, len + 2, "%u;", curr_road->length);
        offset += len + 1;

        len = getNumberLength(curr_road->year);
        snprintf(buffer + offset, len + 2, "%d;", curr_road->year);
        offset += len + 1;

        curr_city = getNextCity(curr_city, curr_road);
        tmp_node = tmp_node->next;
    }
    assert(tmp_node == NULL || tmp_node->value == NULL);
    len = strlen(curr_city->name);
    snprintf(buffer + offset, len + 1, "%s", curr_city->name);
}

/** Calculates size of route description.
 * @param route [in,out]  – pointer to route.
 * @return Size of route description.
 */
size_t getRouteDescriptionLength(Route *route) {
    size_t len = 0;
    City *curr_city;
    list_t *tmp_node;
    Road *curr_road;

    len += getNumberLength(route->routeId);
    len++;

    curr_city = route->firstCity;
    tmp_node = route->roads;
    while (curr_city != route->lastCity) {
        assert(tmp_node != NULL);
        curr_road = (Road *) tmp_node->value;

        assert(curr_road != NULL);

        len += strlen(curr_city->name);
        len++;

        len += getNumberLength(curr_road->length);
        len++;

        len += getNumberLength(curr_road->year);
        len++;

        curr_city = getNextCity(curr_city, curr_road);
        tmp_node = tmp_node->next;
    }
    assert(tmp_node == NULL || tmp_node->value == NULL);
    len += strlen(curr_city->name);
    return len;
}

bool checkIfFirstCityComesFirst(Route *route, City *city1, City *city2) {
    City *curr_city;
    list_t *tmp_node;
    Road *curr_road;

    curr_city = route->firstCity;
    tmp_node = route->roads;
    while (curr_city != route->lastCity) {
        if (curr_city == city1)
            return true;
        if (curr_city == city2)
            return false;

        curr_road = (Road *) tmp_node->value;
        curr_city = getNextCity(curr_city, curr_road);
        tmp_node = tmp_node->next;
    }
    return false;
}

bool routeContains(Route *route, City *city) {
    list_t *tmp_node = route->roads;
    Road *curr_road;
    while (tmp_node != NULL && tmp_node->value != NULL) {
        curr_road = (Road *) tmp_node->value;
        if (curr_road->city1 == city || curr_road->city2 == city)
            return true;
        tmp_node = tmp_node->next;
    }
    return false;
}
