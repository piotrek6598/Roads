/** @file
 * Implementation of map module interface.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "city.h"
#include "road.h"
#include "route.h"
#include "utils.h"

static bool markAllRoadsFromList(list_t **roads, Route *route) {
    int i = 0;
    list_t *tmpNode = *roads;
    while (tmpNode != NULL) {
        if (!markRoadAsPartOfRoute((Road *) tmpNode->value, route)) {
            tmpNode = *roads;
            for (int j = 0; j < i; j++) {
                unmarkRoadAsPartOfRoute((Road *) tmpNode->value, route);
                tmpNode = tmpNode->next;
            }
            return false;
        }
        i++;
        tmpNode = tmpNode->next;
    }
    return true;
}

/** @brief Creates new structure.
 * Creates new empty structure doesn't containing any cities, roads and routes.
 * @return Pointer to new structure or NULL if allocation error occurred.
 */
Map *newMap(void) {
    Map *new_map = (Map *) malloc(sizeof(Map));

    if (new_map == NULL)
        return NULL;

    new_map->cities = mapInit(cityNameCmp, cityNameEq, printCityName,
                              free, deleteCityFromAvlMap);
    if (new_map->cities == NULL) {
        free(new_map);
        return NULL;
    }

    for (unsigned i = 0; i < 1000; i++)
        new_map->routes[i] = NULL;

    return new_map;
}

/** @brief Deletes structure.
 * Deletes structure pointed by @p map.
 * Do nothing if pointer is NULL.
 * @param map[in]         – pointer to deleted map.
 */
void deleteMap(Map *map) {
    if (map == NULL)
        return;

    for (size_t i = 0; i < 1000; i++) {
        if (map->routes[i] != NULL)
            deleteRoute(map->routes[i]);
    }

    mapDelete(map->cities, 2);
    free(map);
}

/** @brief Adds new road between two cities.
 * If any city doesn't exist, adds city to map and then adds road to map.
 * @param map [in,out]    – pointer to map,
 * @param city1 [in]      – pointer to name of first city,
 * @param city2 [in]      – pointer to name of second city,
 * @param length [in]     – length of road in km,
 * @param builtYear [in]  – road built year.
 * @return Value @p true if road was added. Value @p false if error occurred:
 * any parameter has invalid value, cities are the same, road already exists
 * or allocation error occurred.
 */
bool addRoad(Map *map, const char *city1, const char *city2,
             unsigned length, int builtYear) {
    City *cities[2];
    Road *road;
    char *cities_name[2];
    bool created_cities[2] = {false, false};

    if (!checkCityName(city1) || !checkCityName(city2) ||
        !checkLength(length) || !checkYear(builtYear) || map == NULL) {
        return false;
    }

    if (strcmp(city1, city2) == 0)
        return false;

    cities[0] = (City *) mapGet(map->cities, (void *) city1);
    cities[1] = (City *) mapGet(map->cities, (void *) city2);

    for (int i = 0; i < 2; i++) {
        if (cities[i] == NULL) {
            cities_name[i] = malloc(
                    sizeof(char) * (strlen(i == 0 ? city1 : city2) + 1));
            if (cities_name[i] == NULL) {
                if (i == 1 && created_cities[0])
                    deleteCity(cities[0]);
                return false;
            }
            strcpy(cities_name[i], i == 0 ? city1 : city2);
            cities[i] = createCity(cities_name[i]);
            if (cities[i] == NULL) {
                free(cities_name[i]);
                if (i == 1 && created_cities[0])
                    deleteCity(cities[0]);
                return false;
            }
            created_cities[i] = true;
        }
    }

    assert(cities[0] != NULL);
    assert(cities[1] != NULL);

    road = mapGet(cities[0]->connected_roads, (void *) city2);

    if (road != NULL) {
        road = NULL;
    } else {
        road = createRoad(cities[0], cities[1], length, builtYear);
    }

    if (road == NULL) {
        for (int i = 0; i < 2; i++) {
            if (created_cities[i])
                deleteCity(cities[i]);
        }
        return false;
    }

    assert(road != NULL);

    for (int i = 0; i < 2; i++) {
        if (!mapInsert(cities[i]->connected_roads, (void *) cities[1 - i]->name,
                       (void *) road)) {
            if (i == 1) {
                mapRemove(cities[0]->connected_roads, (void *) cities[1]->name,
                          0);
            }
            deleteRoad(road);
            for (int j = 0; j < 2; j++) {
                if (created_cities[j])
                    deleteCity(cities[j]);
            }
            return false;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (created_cities[i] &&
            !mapInsert(map->cities, (void *) cities[i]->name,
                       (void *) cities[i])) {
            if (i == 1 && created_cities[0])
                mapRemove(map->cities, (void *) cities[0]->name, 0);
            for (int j = 0; j < 2; j++) {
                if (created_cities[j])
                    deleteCity(cities[j]);
            }
            return false;
        }
    }

    return true;
}

/** @brief Modifies year of last road repair.
 * For road between two specified cities, changes last repair year or sets it
 * if it's first repair.
 * @param map [in,out]    – pointer to map,
 * @param city1 [in]      – pointer to name of first city,
 * @param city2 [in]      – pointer to name of second city,
 * @param repairYear [in] – last repair year.
 * @return Value @p true if modification succeed. Value @p false if error
 * occurred: any parameter has invalid value, any city doesn't exist, road
 * connecting these cities doesn't exist, repairYear is earlier than last repair
 * year or build year.
 */
bool
repairRoad(Map *map, const char *city1, const char *city2, int repairYear) {
    City *cities[2];
    Road *road;

    if (!checkCityName(city1) || !checkCityName(city2) ||
        !checkYear(repairYear) || map == NULL) {
        return false;
    }

    if (strcmp(city1, city2) == 0)
        return false;

    cities[0] = (City *) mapGet(map->cities, (void *) city1);
    cities[1] = (City *) mapGet(map->cities, (void *) city2);

    if (cities[0] == NULL || cities[1] == NULL)
        return false;

    road = (Road *) mapGet(cities[0]->connected_roads,
                           (void *) cities[1]->name);
    if (road == NULL)
        return false;

    return repairSelectedRoad(road, repairYear);
}

/** @brief Creates route connecting specified cities.
 * Creates route connecting specified cities and sets its number.
 * Finds the shortest way, and if there is more than one option chooses
 * the newest option, where option is described by repair/built year of oldest
 * road.
 * @param map [in,out]    – pointer to map,
 * @param routeId [in]    – route number,
 * @param city1 [in]      – pointer to name of first city,
 * @param city2 [in]      – pointer to name of second city.
 * @return Value @p true if new route was created. Value @p false if error
 * occurred: any parameter is invalid, route already exists, any city doesn't
 * exist, cities are the same, way matching above criteria isn't defined
 * unambiguously, allocation error occurred.
 */
bool newRoute(Map *map, unsigned routeId,
              const char *city1, const char *city2) {
    // todo
    City *cities[2];
    Route *route;
    list_t *roads;

    if (!checkCityName(city1) || !checkCityName(city2) ||
        !checkRouteId(routeId) || map == NULL) {
        return false;
    }

    if (strcmp(city1, city2) == 0)
        return false;
    if (map->routes[routeId] != NULL)
        return false;

    cities[0] = (City *) mapGet(map->cities, (void *) city1);
    cities[1] = (City *) mapGet(map->cities, (void *) city2);

    for (int i = 0; i < 2; i++) {
        if (cities[i] == NULL)
            return false;
    }

    assert(cities[0] != NULL);
    assert(cities[1] != NULL);

    // TODO roads = find path(...);

    if (roads == NULL)
        return false;

    route = createRoute(routeId, cities[0], cities[1], roads);
    if (route == NULL) {
        deleteList(&roads);
        return false;
    }

    if (!markAllRoadsFromList(&roads, route)) {
        deleteRoute(route);
        return false;
    }
    map->routes[routeId] = route;
    return true;
}

/** @brief Extends route to specified city.
 * Ands new roads to route so that new part of route was the shortest.
 * If there is more than one possibility chooses the newest way, where way
 * is described by repair/built year of oldest road.
 * @param map [in,out]    – pointer to map,
 * @param routeId [in]    – route number,
 * @param city [in]       – pointer to name of specified city.
 * @return Value @p true if route was extended. Value @p false if error occurred:
 * any parameter is invalid, route doesn't exist, city doesn't exist, city is
 * already connected to route, way matching above criteria isn't defined
 * unambiguously, allocation error occurred.
 */
bool extendRoute(Map *map, unsigned routeId, const char *city) {
    // todo
    City *extend_city;
    City *cities[2];
    Route *route;
    list_t *roads[2];
    list_t *selected_roads;
    bool from_last;

    if (!checkRouteId(routeId) || !checkCityName(city) || map == NULL)
        return false;

    route = map->routes[routeId];
    if (route == NULL)
        return false;

    extend_city = (City *) mapGet(map->cities, (void *) city);
    if (extend_city == NULL)
        return false;

    cities[0] = route->firstCity;
    cities[1] = route->lastCity;

    // TODO roads[0] = find path(...);
    // TODO roads[1] = find path(...);
    // TODO from_last = choose_better_path(...);

    if (from_last) {
        selected_roads = roads[1];
        deleteList(&roads[0]);
    } else {
        selected_roads = roads[0];
        deleteList(&roads[1]);
    }

    if (!markAllRoadsFromList(&selected_roads, route)) {
        deleteList(&selected_roads);
        return false;
    }

    if (!extendRouteInDirection(route, &selected_roads, extend_city,
                                from_last)) {
        deleteList(&selected_roads);
        return false;
    }
    return true;
}

/** @brief Removes road between two cities.
 * Removes road between two cities. If it breaks any route, will try to replace
 * this road by shortest possible way. If there is more than one possibility
 * chooses the newest way, where way is described by repair/built year of oldest
 * road.
 * @param map [in,out]    – pointer to map,
 * @param city1 [in]      – pointer to name of first city,
 * @param city2 [in]      – pointer to name of second city.
 * @return Value @p true if road was removed. Value @p false if error occurred:
 * any parameter is invalid, any city doesn't exist, road between these cities
 * doesn't exist, way matching above criteria isn't defined unambiguously,
 * allocation error occurred.
 */
bool removeRoad(Map *map, const char *city1, const char *city2) {
    // todo

    if (strcmp(city1, city2) == 0)
        return false;
    return false;
}

/** @brief Removes route with specified number.
 * Removes route with specified number, Do nothing if route doesn't exist.
 * Doesn't remove cities or roads.
 * @param map [in,out]    – pointer to map,
 * @param routeId [in]    – route number.
 * @return Value @p true if route was removed. Value @p false if error occurred:
 * any parameter is invalid, route doesn't exist.
 */
bool removeRoute(Map *map, unsigned routeId) {
    Route *route;

    if (!checkRouteId(routeId) || map == NULL)
        return false;

    route = map->routes[routeId];
    if (route == NULL)
        return false;

    map->routes[routeId] = NULL;
    deleteRoute(route);
    return true;
}

/** @brief Create route description.
 * Return pointer to description, allocates memory for it. Description is empty
 * if route doesn't exist. Memory allocated for description should be released
 * later using free function. Description format is:
 * route number;city name;length of road;built/repair year;city name;...;
 * city name.
 * Cities are in order so that cities @p city1 and @p city2 used in @ref newRoute
 * function was in the same order.
 * @param map [in,out]    – pointer to map,
 * @param routeId [in]    – route number.
 * @return Pointer to description or NULL if allocation error occurred.
 */
char const *getRouteDescription(Map *map, unsigned routeId) {
    Route *route;
    char *buffer;
    size_t desc_len;

    if (!checkRouteId(routeId) || map == NULL)
        return NULL;

    route = map->routes[routeId];
    if (route == NULL)
        return "";

    desc_len = getRouteDescriptionLength(route);
    buffer = malloc(sizeof(char) * (desc_len + 1));
    if (buffer == NULL)
        return NULL;
    fillRouteDescription(route, buffer);
    buffer[desc_len] = '\0';
    return buffer;
}