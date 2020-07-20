/** @file
 * Implementation of class representing city.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#include <stdlib.h>
#include <string.h>

#include "city.h"
#include "utils.h"
#include "road.h"

/** @brief Returns name of second city connected by road.
 * @param city [in,out] - pointer to city,
 * @param road [in,out] - pointer to road.
 * @return Name of second city connected by road.
 */
static char *getNextCityName(City *city, Road *road) {
    City *next_city = getNextCity(city, road);
    return next_city == NULL ? NULL : next_city->name;
}

/** @brief City destructor used only by avl_map module.
 * @param city [in]     - pointer to city.
 */
void deleteCityFromAvlMap(void *city) {
    deleteCity((City *) city);
}

/** @brief Creates city.
 * @param name [in]     - pointer to city name.
 * @return Pointer to new city or NULL if allocation error occurred.
 */
City *createCity(char *name) {
    City *new_city = (City *) malloc(sizeof(City));

    if (new_city == NULL)
        return NULL;

    new_city->connected_roads = mapInit(cityNameCmp, cityNameEq, printCityName,
                                        free, deleteRoadFromAvlMap);
    if (new_city->connected_roads == NULL) {
        free(new_city);
        return NULL;
    }

    new_city->name = name;
    return new_city;
}

/** @brief Deletes city.
 * @param city [in]     - pointer to city.
 */
void deleteCity(City *city) {
    free(city->name);
    mapDelete(city->connected_roads, 2);
    free(city);
}

/** @brief Adds road to city.
 * @param city [in,out] - pointer to city,
 * @param road [in]     - pointer to road.
 * @return Value @p true if road was added. Otherwise value @p false.
 */
bool addRoadToCity(City *city, Road *road) {
    char *next_city_name = getNextCityName(city, road);
    return mapInsert(city->connected_roads, (void *) next_city_name,
                     (void *) road);
}

/** @brief Removes road from city.
 * Doesn't delete road.
 * @param city [in,out] - pointer to city,
 * @param road [in]     - pointer to road.
 */
void removeRoadFromCity(City *city, Road *road) {
    char *next_city_name = getNextCityName(city, road);
    mapRemove(city->connected_roads, (void *) next_city_name, 0);
}