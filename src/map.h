/** @file
 * Interface of class containing map of roads.
 *
 * @author Łukasz Kamiński <kamis@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>,
 * Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @copyright University of Warsaw
 * @date 21.07.2020
 */

#ifndef __MAP_H__
#define __MAP_H__

#include <stdbool.h>
#include "roads_types.h"

Map *newMap(void);

void deleteMap(Map *map);

bool addRoad(Map *map, const char *city1, const char *city2,
             unsigned length, int builtYear);

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear);

bool newRoute(Map *map, unsigned routeId,
              const char *city1, const char *city2);

bool extendRoute(Map *map, unsigned routeId, const char *city);

bool removeRoad(Map *map, const char *city1, const char *city2);

bool removeRoute(Map *map, unsigned routeId);

char const *getRouteDescription(Map *map, unsigned routeId);

bool createRoute(Map **map, unsigned routeId, list_t *roads_list);

#endif /* __MAP_H__ */
