/** @file
 * Interface of class representing road.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#ifndef ROADS_ROAD_H
#define ROADS_ROAD_H

#include "roads_types.h"

void deleteRoadFromAvlMap(void *road);

Road *createRoad(City *city1, City *city2, unsigned length, int builtYear);

void deleteRoad(Road *road);

void safeDeleteRoad(Road *road);

bool repairRoad(Road *road, int repairYear);

bool markRoadAsPartOfRoute(Road *road, Route *route);

void unmarkRoadAsPartOfRoute(Road *road, Route *route);

#endif //ROADS_ROAD_H
