/** @file
 * Map interface based on AVL tree structure. Enables standard map
 * operations and printing keys is weak ordering. Keys and values are stored
 * by void*. Map need to be initiated with comparator which returns true only
 * if first argument comes before its second argument in weak ordering,
 * equals operator which returns true only if two elements should be treated
 * as same element, printer which prints keys to standard output and keys
 * and values destructors.
 * NULL cannot be stored neither as key nor value.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 12.03.2020
 */

#ifndef MALE_AVL_MAP_H
#define MALE_AVL_MAP_H

#include <stddef.h>
#include <stdbool.h>

#include "list.h"

/**
 * Struct node represents avl tree, contains pointers to stored key
 * and mapped value, array of pointers to node's children, height of tree
 * including node.
 */
typedef struct node {
    void *key;              /**< pointer to stored key */
    void *value;            /**< pointer to stored value */
    struct node **children; /**< array of pointers to node's children */
    int height;             /**< height of subtree */
} tree_t;

/**
 * Struct map_tree represents avl map, contains pointer to root of AVL tree
 * and pointers to functions: comparator, equals operator, printer,
 * key's destructor and value's destructor.
 */
typedef struct map_tree {
    tree_t *root;                    /**< pointer to root of AVL tree */

    bool (*compare)(void *, void *); /**< pointer to comparator */
    bool (*equals)(void *, void *);  /**< pointer to equals operator */
    void (*printer)(void *);         /**< pointer to printer */
    void (*keyDestructor)(void *);   /**< pointer to key's destructor */
    void (*valueDestructor)(void *); /**< pointer to value's destructor */
} map_t;

/** @brief Initiates empty map with given comparator.
 * When any pair <key, value> is removed or map is deleted, given destructors
 * are used if deletion type requires it.
 * @param compare [in]           - pointer to comparator;
 * @param equals [in]            - pointer to equals operator.
 * @param printer [in]           - pointer to printer;
 * @param keyDestructor [in]     - pointer to key's destructor;
 * @param valueDestructor [in]   - pointer to value's destructor.
 * @return Pointer to new empty map or NULL if allocation error occurred.
 */
map_t *mapInit(bool (*compare)(void *, void *), bool (*equals)(void *, void *),
               void (*printer)(void *), void (*keyDestructor)(void *),
               void (*valueDestructor)(void *));

/** @brief Inserts new pair <key, value> to map.
 * Do nothing if key already exists in map.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key;
 * @param value [in]      - pointer to value.
 * @return Value @p true if pair was inserted or already exist value @p false
 * if allocation error occurred.
 */
bool mapInsert(map_t *map, void *key, void *value);

/** @brief Removes pair identified by key from map.
 * Deletion type specify if keys and values should be freed.
 * Value @p 0 -> nothing to delete, value @p 1 -> only keys to delete,
 * value @p 2 -> only values to delete, value @p 3 both keys and value to delete.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key;
 * @param type [in]       - deletion type.
 * @return Value @p true if pair was removed, value @p false if map doesn't
 * contain given key.
 */
bool mapRemove(map_t *map, void *key, int type);

/** @brief Returns mapped value for given key.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key.
 * @return Pointer to stored value or NULL if key is not stored in map.
 */
void *mapGet(map_t *map, void *key);

/** @brief Checks if map contains given key.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key.
 * @return Value @p true if map contains given key, otherwise value @p false.
 */
bool mapContains(map_t *map, void *key);

/** @brief Deletes map and frees previously allocated space.
 * Deletion type specify if keys and values should be freed.
 * Value @p 0 -> nothing to delete, value @p 1 -> only keys to delete,
 * value @p 2 -> only values to delete, value @p both keys and value to delete.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in]   - pointer to map;
 * @param type [in]  - defines type of deletion.
 */
void mapDelete(map_t *map, int type);

/** @brief Prints keys to standard output using given printer in weak ordering.
 * @param map [in, out]    - pointer to map.
 */
void mapPrintKeys(map_t *map);

/** @brief Creates list of keys in weak ordering.
 * @param map [in, out]   - pointer to map.
 * @return Pointer to created list or NULL if allocation error occurred.
 */
list_t *mapGetKeysList(map_t *map);

/** @brief Creates list of values.
 * List is in that order, that paired keys create weak ordering.
 * @param map [in, out]   - pointer to map.
 * @return Pointer to created list or NULL if allocation error occurred.
 */
list_t *mapGetValuesList(map_t *map);

/** @brief Removes all data and frees previously allocated space.
 * Clear type specify if keys and values should be freed.
 * Value @p 0 -> nothing to free, value @p 1 -> only keys to free,
 * value @p 2 -> only values to free, value @p both keys and value to free.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in]   - pointer to map;
 * @param type [in]  - defines type of clear.
 */
void mapClear(map_t *map, int type);

#endif //MALE_AVL_MAP_H
