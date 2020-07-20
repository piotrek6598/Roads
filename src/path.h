/** @file
 * Interface of module finding best path.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#ifndef ROADS_PATH_H
#define ROADS_PATH_H

#include "roads_types.h"

path_t *
findBestPath(Map *map, City *city1, City *city2, list_t **exclude_roads,
             bool direct);

bool checkIfPathDefinedUnambiguously(Map *map, path_t *path, City *city1,
                                     City *city2, list_t **exclude_roads,
                                     bool direct);

bool checkIfFirstPathBetter(path_t *path1, path_t *path2);

#endif //ROADS_PATH_H
