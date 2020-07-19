/** @file
 * Implementation of class representing road.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#include <stdlib.h>

#include "road.h"

/** @brief Road destructor used only by avl_map module.
 * @param road [in]       - pointer to road.
 */
void deleteRoadFromAvlMap(void *road) {
    safeDeleteRoad((Road *) road);
}

/** @brief Creates new road between two cities.
 * @param city1 [in]      - pointer to first city,
 * @param city2 [in]      - pointer to second city,
 * @param length [in]     – length of road,
 * @param builtYear [in]  - road built year.
 * @return Pointer to new road or NULL if allocation error occurred.
 */
Road *createRoad(City *city1, City *city2, unsigned length, int builtYear) {
    Road *new_road = (Road *) malloc(sizeof(Road));

    if (new_road == NULL)
        return NULL;

    new_road->partOfRoute = newList();
    if (new_road == NULL) {
        free(new_road);
        return NULL;
    }

    new_road->city1 = city1;
    new_road->city2 = city2;
    new_road->length = length;
    new_road->year = builtYear;
    new_road->citiesCounter = 2;
    return new_road;
}

/** @brief Deletes road.
 * Doesn't delete cities or routes.
 * @param road [in]       – pointer to road.
 */
void deleteRoad(Road *road) {
    deleteList(&road->partOfRoute);
    free(road);
}

/** @brief Decreases cities counter and deletes road if counter achieved 0.
 * Doesn't delete cities or routes.
 * @param road [in,out]   - pointer to road.
 */
void safeDeleteRoad(Road *road) {
    road->citiesCounter--;
    if (road->citiesCounter == 0)
        deleteRoad(road);
}

/** @brief Repairs road.
 * Do nothing if repairYear is not greater than built year or year
 * of last repair.
 * @param road [in,out]   - pointer to road,
 * @param repairYear [in] - repair year.
 * @return Value @p true if road was repaired. Otherwise value @p false.
 */
bool repairRoad(Road *road, int repairYear) {
    if (repairYear > road->year) {
        road->year = repairYear;
        return true;
    }
    return false;
}

/** @brief Marks that road is part of specified route.
 * @param road [in,out]   - pointer to road,
 * @param route [in,out]  – pointer to route.
 * @return Value @p true if road was successfully marked.
 * Otherwise value @p false.
 */
bool markRoadAsPartOfRoute(Road *road, Route *route) {
    return addList(&road->partOfRoute, (void *) route);
}

/** @brief Marks that road is not longer part of specified road.
 * Do nothing if road wasn't part of specified road.
 * @param road [in,out]   - pointer to road,
 * @param route [in,out]  - pointer to route.
 */
void unmarkRoadAsPartOfRoute(Road *road, Route *route) {
    removeList(&road->partOfRoute, (void *) route);
}