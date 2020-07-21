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
#include <stdio.h>

#include "map.h"
#include "city.h"
#include "path.h"
#include "road.h"
#include "route.h"
#include "utils.h"

static bool markAllRoadsFromList(list_t **roads, Route *route) {
    int i = 0;
    list_t *tmpNode = *roads;
    while (tmpNode != NULL && tmpNode->value != NULL) {
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

static void unmarkAllRoadsFromList(list_t **roads, Route *route) {
    list_t *tmp_node = *roads;
    while (tmp_node != NULL && tmp_node->value != NULL) {
        unmarkRoadAsPartOfRoute((Road *) tmp_node->value, route);
        tmp_node = tmp_node->next;
    }
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

    new_map->cities_num = 0;
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
    //printf("%s %s %u %d\n", city1, city2, length, builtYear);
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
                if (i == 1 && created_cities[0]) {
                    map->cities_num--;
                    deleteCity(cities[0]);
                }
                return false;
            }
            strcpy(cities_name[i], i == 0 ? city1 : city2);
            cities[i] = createCity(cities_name[i]);
            if (cities[i] == NULL) {
                free(cities_name[i]);
                if (i == 1 && created_cities[0]) {
                    map->cities_num--;
                    deleteCity(cities[0]);
                }
                return false;
            }
            map->cities_num++;
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
            if (created_cities[i]) {
                map->cities_num--;
                deleteCity(cities[i]);
            }
        }
        return false;
    }

    assert(road != NULL);

    for (int i = 0; i < 2; i++) {
        if (!addRoadToCity(cities[i], road)) {
            if (i == 1)
                removeRoadFromCity(cities[0], road);
            deleteRoad(road);
            for (int j = 0; j < 2; j++) {
                if (created_cities[j]) {
                    map->cities_num--;
                    deleteCity(cities[j]);
                }
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
                if (created_cities[j]) {
                    map->cities_num--;
                    deleteCity(cities[j]);
                }
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
    City *cities[2];
    Route *route;
    list_t *roads;
    path_t *path;

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

    path = findBestPath(map, cities[0], cities[1], NULL, true);
    if (path == NULL)
        return false;

    if (checkIfPathDefinedUnambiguously(map, path, cities[0], cities[1],
                                        NULL, true)) {
        roads = path->roads;
        free(path);
    } else {
        free(path);
        return false;
    }

    route = createNewRoute(routeId, cities[0], cities[1], roads);
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
    City *extend_city;
    City *cities[2];
    Route *route;
    list_t *selected_roads = NULL;
    path_t *paths[2];
    bool from_last;

    if (!checkRouteId(routeId) || !checkCityName(city) || map == NULL)
        return false;

    route = map->routes[routeId];
    if (route == NULL)
        return false;

    extend_city = (City *) mapGet(map->cities, (void *) city);
    if (extend_city == NULL)
        return false;

    if (routeContains(route, extend_city))
        return false;

    cities[0] = route->firstCity;
    cities[1] = route->lastCity;

    paths[0] = findBestPath(map, extend_city, cities[0], &route->roads, true);
    paths[1] = findBestPath(map, cities[1], extend_city, &route->roads, true);

    if (paths[0] == NULL) {
        if (paths[1] != NULL &&
            checkIfPathDefinedUnambiguously(map, paths[1], cities[1],
                                            extend_city, &route->roads,
                                            true)) {
            selected_roads = paths[1]->roads;
            from_last = true;
        }
    } else {
        if (paths[1] == NULL) {
            if (checkIfPathDefinedUnambiguously(map, paths[0], extend_city,
                                                cities[0], &route->roads,
                                                true)) {
                selected_roads = paths[0]->roads;
                from_last = false;
            }
        } else {
            if (checkIfFirstPathBetter(paths[0], paths[1])) {
                if (checkIfPathDefinedUnambiguously(map, paths[0], extend_city,
                                                    cities[0], &route->roads,
                                                    true)) {
                    selected_roads = paths[0]->roads;
                    from_last = false;
                }
            } else if (checkIfFirstPathBetter(paths[1], paths[0])) {
                if (checkIfPathDefinedUnambiguously(map, paths[1], cities[1],
                                                    extend_city,
                                                    &route->roads, true)) {
                    selected_roads = paths[1]->roads;
                    from_last = true;
                }
            }
        }
    }
    for (int i = 0; i < 2; i++) {
        if (paths[i] != NULL) {
            if (paths[i]->roads != selected_roads)
                deleteList(&paths[i]->roads);
            free(paths[i]);
        }
    }
    if (selected_roads == NULL)
        return false;

    if (!markAllRoadsFromList(&selected_roads, route)) {
        deleteList(&selected_roads);
        return false;
    }

    // TODO to jest debug
    //fprintf(stderr,"DEBUG START\n");
    list_t *tmp_node = selected_roads;
    while (tmp_node->value != NULL) {
        Road *curr_road = (Road *) tmp_node->value;
        //fprintf(stderr, "%s %s\n", curr_road->city1->name, curr_road->city2->name);
        tmp_node = tmp_node->next;
    }
    //fprintf(stderr, "DEBUG END\n");

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
    City *cities[2];
    Road *road;
    Route *curr_route;
    Route **routes;
    list_t *tmp_node;
    path_t **paths;
    unsigned routes_num;
    bool city1_first;
    bool unambiguous;

    if (!checkCityName(city1) || !checkCityName(city2) || map == NULL)
        return false;
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

    routes_num = road->routes_num;
    paths = (path_t **) malloc(sizeof(path_t *) * routes_num);
    if (paths == NULL)
        return false;

    routes = (Route **) malloc(sizeof(Route *) * routes_num);
    if (routes == NULL) {
        free(paths);
        return false;
    }

    tmp_node = road->partOfRoute;
    for (unsigned i = 0; i < routes_num; i++) {
        curr_route = (Route *) tmp_node->value;
        assert(curr_route != NULL);
        city1_first = checkIfFirstCityComesFirst(curr_route, cities[0],
                                                 cities[1]);
        if (city1_first) {
            paths[i] = findBestPath(map, cities[0], cities[1],
                                    &curr_route->roads, false);
        } else {
            paths[i] = findBestPath(map, cities[1], cities[0],
                                    &curr_route->roads, false);
        }

        if (paths[i] == NULL) {
            for (unsigned j = i; j > 0; j--) {
                deleteList(&paths[j - 1]->roads);
                unmarkAllRoadsFromList(&paths[j - 1]->roads, routes[j - 1]);
                free(paths[j - 1]);
            }
            free(routes);
            free(paths);
            return false;
        }

        if (city1_first) {
            unambiguous = checkIfPathDefinedUnambiguously(map, paths[i],
                                                          cities[0], cities[1],
                                                          &curr_route->roads,
                                                          false);
        } else {
            unambiguous = checkIfPathDefinedUnambiguously(map, paths[i],
                                                          cities[1], cities[0],
                                                          &curr_route->roads,
                                                          false);

        }
        if (!unambiguous) {
            for (unsigned j = i + 1; j > 0; j--) {
                deleteList(&paths[j - 1]->roads);
                if (j != i + 1)
                    unmarkAllRoadsFromList(&paths[j - 1]->roads, routes[j - 1]);
                free(paths[j - 1]);
            }
            free(paths);
            free(routes);
            return false;
        }
        if (!markAllRoadsFromList(&paths[i]->roads, curr_route)) {
            for (unsigned j = i + 1; j > 0; j--) {
                deleteList(&paths[j - 1]->roads);
                if (j != i + 1)
                    unmarkAllRoadsFromList(&paths[j - 1]->roads, routes[j - 1]);
                free(paths[j - 1]);
            }
            free(paths);
            free(routes);
            return false;
        }
        routes[i] = curr_route;
        tmp_node = tmp_node->next;
    }

    tmp_node = road->partOfRoute;
    for (unsigned i = 0; i < routes_num; i++) {
        curr_route = (Route *) tmp_node->value;/*
        // TODO to jest debug
        //fprintf(stderr,"DEBUG START\n");
        tmp_node = paths[i]->roads;
        while (tmp_node->value != NULL) {
            Road *curr_road = (Road *) tmp_node->value;
            //fprintf(stderr, "%s %s\n", curr_road->city1->name, curr_road->city2->name);
            tmp_node = tmp_node->next;
        }
        //fprintf(stderr, "DEBUG END\n");*/
        assert(curr_route != NULL);
        bool ret_val = replaceRoad(curr_route, road, &paths[i]->roads);
        assert(ret_val);

        free(paths[i]);

        tmp_node = tmp_node->next;
    }
    free(paths);
    free(routes);
    removeRoadFromCity(cities[0], road);
    removeRoadFromCity(cities[1], road);
    deleteRoad(road);
    return true;
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
    if (route == NULL) {
        buffer = malloc(sizeof(char));
        if (buffer == NULL)
            return NULL;
        buffer[0] = '\0';
        return buffer;
    }

    desc_len = getRouteDescriptionLength(route);
    buffer = malloc(sizeof(char) * (desc_len + 1));
    if (buffer == NULL)
        return NULL;
    fillRouteDescription(route, buffer);
    buffer[desc_len] = '\0';
    //printf("%zu\n", desc_len);
    return buffer;
}

static void destroyRoadDescList(list_t *roads) {
    while (!emptyList(&roads))
        free(removeHeadList(&roads));
    deleteList(&roads);
}

static void destroyRoadList(list_t *roads) {
    while (!emptyList(&roads))
        deleteRoad((Road *) removeHeadList(&roads));
    deleteList(&roads);
}

static void destroyCityList(list_t *cities) {
    while (!emptyList(&cities))
        deleteCity((City *) removeHeadList(&cities));
    deleteList(&cities);
}

static bool
safeInsertOrModify(Map **map, list_t **cities, list_t **roads, list_t **years) {
    list_t *tmp_node, *tmp_node2, *tmp_node3, *tmp_node4;
    unsigned i = 0;
    tmp_node = *cities;

    while (tmp_node != NULL) {
        if (tmp_node->value != NULL) {
            City *curr_city = (City *) tmp_node->value;
            if (!mapInsert((*map)->cities, (void *) curr_city->name,
                           tmp_node->value)) {
                tmp_node2 = *cities;
                for (unsigned j = 0; j < i; j++) {
                    if (tmp_node2->value != NULL) {
                        curr_city = (City *) tmp_node2->value;
                        mapRemove((*map)->cities, (void *) curr_city->name, 0);
                    }
                    tmp_node2 = tmp_node2->next;
                }
                return false;
            }
        }
        i++;
        tmp_node = tmp_node->next;
    }

    tmp_node = *years;
    tmp_node2 = *roads;
    i = 0;
    while (tmp_node != NULL) {
        if (tmp_node->value == NULL && tmp_node->next != NULL) {
            Road *curr_road = (Road *) tmp_node2->value;
            bool str = false;
            if (!addRoadToCity(curr_road->city1, curr_road))
                str = true;
            if (!str && !addRoadToCity(curr_road->city2, curr_road)) {
                str = true;
                removeRoadFromCity(curr_road->city1, curr_road);
            }
            if (str) {
                tmp_node3 = tmp_node2->next;
                tmp_node4 = tmp_node->next;
                while (tmp_node3 != NULL && tmp_node3->next != NULL) {
                    if (tmp_node4->value != NULL) {
                        Road *tmp_road = (Road *) tmp_node3->value;
                        tmp_road->year = *(int *) tmp_node4->value;
                    }
                    tmp_node3 = tmp_node3->next;
                    tmp_node4 = tmp_node4->next;
                }

                tmp_node3 = *roads;
                tmp_node4 = *years;
                for (unsigned j = 0; j < i; j++) {
                    Road *tmp_road = (Road *) tmp_node3->value;
                    if (tmp_node4->value != NULL) {
                        tmp_road->year = *(int *) tmp_node4->value;
                    } else {
                        removeRoadFromCity(tmp_road->city1, tmp_road);
                        removeRoadFromCity(tmp_road->city2, tmp_road);
                    }
                }

                tmp_node3 = *cities;
                while (tmp_node3 != NULL) {
                    if (tmp_node3->value != NULL) {
                        City *tmp_city = (City *) tmp_node3->value;
                        mapRemove((*map)->cities, (void *) tmp_city->name, 0);
                    }
                    tmp_node3 = tmp_node3->next;
                }
                return false;
            }
        }
        i++;
        tmp_node = tmp_node->next;
        tmp_node2 = tmp_node2->next;
    }
    return true;
}

bool createRoute(Map **map, unsigned int routeId, list_t *roads_list) {
    City *city1, *city2, *first_city, *last_city = NULL;
    Road *road;
    Route *route;
    list_t *tmp_node;
    list_t *route_roads = newList(), *add_cities = newList();
    list_t *add_roads = newList(), *old_years = newList();
    size_t roads_list_size = 0;
    char *road_desc, *city_name1, *city_name2;
    unsigned length;
    int year;
    bool route_created = false;

    if (!checkRouteId(routeId) || map == NULL || route_roads == NULL ||
        add_cities == NULL || add_roads == NULL || old_years == NULL) {
        deleteList(&route_roads);
        deleteList(&add_cities);
        deleteList(&add_roads);
        deleteList(&old_years);
        destroyRoadDescList(roads_list);
        return false;
    }

    route = (*map)->routes[routeId];
    if (route != NULL) {
        deleteList(&route_roads);
        deleteList(&add_cities);
        deleteList(&add_roads);
        deleteList(&old_years);
        destroyRoadDescList(roads_list);
        return false;
    }

    tmp_node = roads_list;
    while (tmp_node != NULL && tmp_node->value != NULL) {
        roads_list_size++;
        tmp_node = tmp_node->next;
    }
    if (roads_list_size == 0) {
        destroyRoadDescList(roads_list);
        deleteList(&route_roads);
        deleteList(&add_cities);
        deleteList(&add_roads);
        deleteList(&old_years);
        return false;
    }

    while ((road_desc = (char *) removeHeadList(&roads_list)) != NULL) {
        city_name1 = strtok(road_desc, ";");
        length = parseStringToUnsigned(strtok(NULL, ";"));
        year = parseStringToInt(strtok(NULL, ";"));
        city_name2 = strtok(NULL, ";");

        //printf("TEST\n%s %u %d %s\n", city_name1, length, year, city_name2);

        char *cities[2];
        cities[0] = malloc(sizeof(char) * (strlen(city_name1) + 1));
        cities[1] = malloc(sizeof(char) * (strlen(city_name2) + 1));

        if (!checkCityName(city_name1) || !checkCityName(city_name2) ||
            !checkLength(length) || !checkYear(year) || cities[0] == NULL ||
            cities[1] == NULL) {
            free(road_desc);
            free(cities[0]);
            free(cities[1]);
            goto rollback;
        }
        if (last_city == NULL) {
            city2 = mapGet((*map)->cities, (void *) city_name2);
            last_city = city2;
        } else {
            city2 = city1;
        }
        if (city2 == NULL) {
            assert(last_city == NULL);
            strcpy(cities[1], city_name2);
            city2 = createCity(cities[1]);
            if (city2 == NULL) {
                free(road_desc);
                free(cities[0]);
                free(cities[1]);
                goto rollback;
            }
            if (last_city == NULL) {
                if (!addList(&add_cities, (void *) city2)) {
                    deleteCity(city2);
                    free(road_desc);
                    free(cities[0]);
                    goto rollback;
                }
                last_city = city2;
            }
        } else {
            free(cities[1]);
            if (!addList(&add_cities, NULL)) {
                free(cities[0]);
                free(road_desc);
                goto rollback;
            }
        }

        city1 = mapGet((*map)->cities, (void *) city_name1);
        if (city1 == NULL) {
            strcpy(cities[0], city_name1);
            city1 = createCity(cities[0]);
            if (city1 == NULL) {
                free(road_desc);
                free(cities[0]);
                goto rollback;
            }
            if (!addList(&add_cities, (void *) city1)) {
                free(road_desc);
                deleteCity(city1);
                goto rollback;
            }
        } else {
            free(cities[0]);
            if (!addList(&add_cities, NULL)) {
                free(road_desc);
                goto rollback;
            }
        }
        first_city = city1;

        road = mapGet(city1->connected_roads, (void *) city2->name);
        if (road == NULL) {
            road = createRoad(city1, city2, length, year);
            if (road == NULL) {
                free(road_desc);
                goto rollback;
            }
            if (!addList(&old_years, NULL)) {
                free(road_desc);
                deleteRoad(road);
                goto rollback;
            }
            if (!addList(&add_roads, (void *) road)) {
                free(road_desc);
                removeHeadList(&old_years);
                deleteRoad(road);
                goto rollback;
            }
        } else {
            if (road->length != length || road->year > year) {
                free(road_desc);
                goto rollback;
            }
            if (!addList(&old_years, (void *) &road->year)) {
                free(road_desc);
                goto rollback;
            }
            road->year = year;
            if (!addList(&add_roads, road)) {
                free(road_desc);
                removeHeadList(&old_years);
                goto rollback;
            }
        }

        free(road_desc);

        if (!addList(&route_roads, (void *) road))
            goto rollback;
        //printf("%zu %zu %zu %zu\n", strlen(cities[0]), strlen(city1->name), strlen(cities[1]), strlen(city2->name));
    }
    //printf("%s %s\n", first_city->name, last_city->name);
    route = createNewRoute(routeId, first_city, last_city, route_roads);
    if (route == NULL)
        goto rollback;
    //printf("%s %s\n", route->firstCity->name, route->lastCity->name);
    route_created = true;
    if (!markAllRoadsFromList(&route_roads, route)) {
        deleteRoute(route);
        goto rollback;
    }

    if (!safeInsertOrModify(map, &add_cities, &add_roads, &old_years)) {
        unmarkAllRoadsFromList(&route_roads, route);
        deleteRoute(route);
        goto rollback;
    }
    (*map)->routes[routeId] = route;
    deleteList(&roads_list);
    deleteList(&add_roads);
    deleteList(&add_cities);
    deleteList(&old_years);
    //printf("%s %s\n", (*map)->routes[routeId]->firstCity->name, (*map)->routes[routeId]->lastCity->name);
    return true;

    rollback:
    destroyRoadDescList(roads_list);
    destroyCityList(add_cities);
    destroyRoadList(add_roads);
    if (!route_created)
        deleteList(&route_roads);
    deleteList(&old_years);
    return false;
}