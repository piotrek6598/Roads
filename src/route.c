/** @file
 * Implementation of class representing route.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#include "route.h"

/** @brief Creates new route with specified number.
 * Creates route with number @p routeId from @p city1 to @p city2 made by
 * specified roads (@p roads).
 * @param routeId [in]    - route number,
 * @param city1 [in]      - pointer to first city,
 * @param city2 [in]      - pointer to last city,
 * @param roads [in]      - list of roads which make this route.
 * @return Pointer to new route or NULL if allocation error occurred.
 */
Route *createRoute(unsigned routeId, City *city1, City *city2, list_t roads) {
    // todo
    return NULL;
}

/** @brief Deletes specified route.
 * Doesn't delete cities or roads.
 * @param route [in]
 */
void deleteRoute(Route *route) {
    // todo
}

/** @brief Replaces road by list of other roads.
 * Replaces specified road in specified route with list of other roads.
 * Assumes that after replacing route will be coherent.
 * @param route [in,out]  – pointer to route,
 * @param road [in]       - pointer to road,
 * @param roads [in]      - list of substituting roads.
 * @return Value @p true if road was replaced. Otherwise value @p false.
 */
bool replaceRoad(Route *route, Road *road, list_t roads) {
    // todo
    return false;
}

/** @brief Extends route in specified direction.
 * Updates first or last city.
 * @param route [in,out]  – pointer to route,
 * @param roads [in]      – list of extending roads,
 * @param from_last [in]  – flag indicating if route is extended from last city.
 * @return Value @p true if route was extended. Otherwise value @p false.
 */
bool extendRouteInDirection(Route *route, list_t roads, bool from_last) {
    // todo
    return false;
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
    // todo
}

/** Calculates size of route description.
 * @param route [in,out]  – pointer to route.
 * @return Size of route description.
 */
size_t getRouteDescriptionSize(Route *route) {
    // todo
    return 0;
}