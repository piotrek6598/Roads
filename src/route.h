/** @file
 * Interface of class representing route.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#ifndef ROADS_ROUTE_H
#define ROADS_ROUTE_H

#include "roads_types.h"

Route *createRoute(unsigned routeId, City *city1, City *city2, list_t *roads);

void deleteRoute(Route *route);

bool replaceRoad(Route *route, Road *road, list_t *roads);

bool extendRouteInDirection(Route *route, list_t *roads, bool from_last);

void fillRouteDescription(Route *route, char *buffer);

size_t getRouteDescriptionLength(Route *route);

#endif //ROADS_ROUTE_H
