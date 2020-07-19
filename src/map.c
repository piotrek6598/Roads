/** @file
 * Implementation of map module interface.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#include <assert.h>
#include <stddef.h>

#include "map.h"

/** @brief Creates new structure.
 * Creates new empty structure doesn't containing any cities, roads and routes.
 * @return Pointer to new structure or NULL if allocation error occurred.
 */
Map *newMap(void) {
    // todo
    return NULL;
}

/** @brief Deletes structure.
 * Deletes structure pointed by @p map.
 * Do nothing if pointer is NULL.
 * @param map[in]         – pointer to deleted map.
 */
void deleteMap(Map *map) {
    // todo
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
    // todo
    return false;
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
    // todo
    return false;
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
    return false;
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
    return false;
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
    // todo
    return false;
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
    // todo
    return NULL;
}