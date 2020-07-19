/** @file
 * Set of types used in Roads project.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 19.07.2020
 */

#ifndef ROADS_ROADS_TYPES_H
#define ROADS_ROADS_TYPES_H

#include "avl_map.h"
#include "list.h"

/**
 * Type representing map containing roads.
 */
typedef struct Map Map;

/**
 * Type representing city.
 */
typedef struct City City;

/**
 * Type representing road.
 */
typedef struct Road Road;

/**
 * Type representing route.
 */
typedef struct Route Route;

/**
 * Structure representing map of roads.
 * Contains map of cities and list of created routes.
 */
struct Map {
    map_t *cities;           /**< Map containing cites, keys are city names,
                                 values are pointers to cities. */
    Route *routes[1000];    /**< Array of pointers to routes. */
};

/**
 * Structure representing city.
 * Contains city name and list of outgoing roads.
 */
struct City {
    char *name;             /**< Pointer to city name. */
    map_t *connected_roads; /**< Map of outgoing roads from city.
                                Keys are city name, values pointers to roads. */
};

/**
 * Structure representing road.
 * Contains pointers to city which road connects, length in km, built year
 * or year of last repair, list of routes which contain this road and
 * counter how many of connected cities still exist.
 * Counter is used only during deleting map of roads.
 */
struct Road {
    City *city1;            /**< Pointer to first city. */
    City *city2;            /**< Pointer to last city. */
    unsigned length;        /**< Length of road. */
    int year;               /**< Built year or year of last repair. */
    list_t *partOfRoute;    /**< List of pointers to routes containing road. */

    int citiesCounter;      /**< Counts how many of connected cities exist. */
};

/**
 * Structure representing route.
 * Contains route number, pointer to first and last city of route and
 * list of pointers to roads making this route.
 */
struct Route {
    unsigned routeId;       /**< Route number. */
    City *firstCity;        /**< Pointer to first city. */
    City *lastCity;         /**< Pointer to last city. */
    list_t *roads;          /**< List of pointers to roads making this route. */
};

#endif //ROADS_ROADS_TYPES_H
