/** @file
 * Interface of class representing city.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#ifndef ROADS_CITY_H
#define ROADS_CITY_H

#include "roads_types.h"

void deleteCityFromAvlMap(void *city);

City *createCity(char *name);

void deleteCity(City *city);

bool addRoadToCity(City *city, Road *road);

void removeRoadFromCity(City *city, Road *road);

#endif //ROADS_CITY_H
