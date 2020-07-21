/** @file
 * Set of types used in Roads project.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 21.07.2020
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
 * Type representing heap node.
 */
typedef struct heap_node heap_node_t;

/**
 * Type representing heap.
 */
typedef struct heap heap_t;

/**
 * Type representing path.
 */
typedef struct path path_t;

/**
 * Structure representing map of roads.
 * Contains map of cities, list of created routes and number of cities.
 */
struct Map {
    unsigned cities_num;    /**< Number of cities in map. */
    map_t *cities;          /**< Map containing cites, keys are city names,
                                 values are pointers to cities. */
    Route *routes[1000];    /**< Array of pointers to routes. */
};

/**
 * Structure representing city.
 * Contains city name, list of outgoing roads and variables
 * used by Dijkstra algorithm.
 */
struct City {
    char *name;             /**< Pointer to city name. */
    map_t *connected_roads; /**< Map of outgoing roads from city.
                                Keys are city name, values pointers to roads. */

    unsigned num_in_heap;   /**< Number of heap node with this city */
    City *prev_city;        /**< Pointer to previous city in Dijkstra algorithm. */
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
    unsigned routes_num;    /**< Number of routes containing road. */

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

/**
 * Structure representing heap node.
 * Contains pointer to city, total distance from start city
 * in Dijkstra algorithm and year of oldest road on that way.
 */
struct heap_node {
    City *city;             /**< Pointer to city. */
    unsigned total_len;     /**< Shortest distance from start city. */
    int year;               /**< Year of oldest road on that way */
};

/**
 * Structure representing heap.
 * Contains heap size and array of heap nodes. At the top is node
 * with lowest distance.
 */
struct heap {
    size_t size;            /**< Heap size. */
    heap_node_t *heap_tab;  /**< Array of heap nodes. */
};

/**
 * Structure representing path.
 * Contains list roads making this path, length of path and year of oldest road.
 */
struct path {
    list_t *roads;          /**< List of roads. */
    unsigned total_len;     /**< Length of path. */
    int year;               /**< Year of oldest road. */
};

#endif //ROADS_ROADS_TYPES_H
