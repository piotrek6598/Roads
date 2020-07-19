/** @file
 * Implementation in C of map interface based on AVL tree structure. Enables
 * standard map operations and printing keys in weak order. Keys and values
 * are stored by void*. Map is initiated with comparator which returns true
 * only if first arguments comes before its second argument in weak ordering,
 * equals operator which returns true only if two elements should be
 * treated as same element, printer which prints keys to standard output and
 * keys and values destructors.
 * NULL cannot be stored neither as key nor value.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 13.03.2020
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "avl_map.h"

/** @brief Defines avl tree rotation left type.
 */
#define LEFT 0
/** @brief Defines avl tree rotation right type.
 */
#define RIGHT 1

/** @brief Allocates memory for new node and creates it.
 * @param key [in]     - pointer to node's key;
 * @param value [in]   - pointer to stored value.
 * @return Pointer to new node or NULL if allocation error occurred.
 */
static tree_t *createNode(void *key, void *value) {
    tree_t *newNode = (tree_t *) malloc(sizeof(tree_t));

    if (newNode == NULL)
        return NULL;

    newNode->children = (tree_t **) malloc(2 * sizeof(tree_t *));

    if (newNode->children == NULL) {
        free(newNode);
        return NULL;
    }

    newNode->children[0] = newNode->children[1] = NULL;
    newNode->key = key;
    newNode->value = value;
    newNode->height = 1;

    return newNode;
}

/** @brief Utility function to check height of given tree.
 * @param tree [in, out]   - pointer to tree.
 * @return Height of tree or 0 if given tree is NULL.
 */
static inline int treeHeight(tree_t *tree) {
    return tree == NULL ? 0 : tree->height;
}

/** @brief Utility function to return maximum of two numbers.
 * @param left   - first number;
 * @param right  - second number.
 * @return Greater of given numbers.
 */
static inline int max(int left, int right) {
    return left > right ? left : right;
}

/** @brief Calculates tree balance defined as difference between height
 * of left sub-tree and right sub-tree.
 * @param tree [in, out]   - pointer to tree.
 * @return Balance of given tree or 0 if NULL was given.
 */
static inline int getTreeBalance(tree_t *tree) {
    if (tree == NULL)
        return 0;
    return treeHeight(tree->children[0]) - treeHeight(tree->children[1]);
}

/** @brief Updates height of given tree.
 * Assumes that tree is not NULL.
 * @param tree [in, out]   - pointer to tree.
 */
static inline void updateTreeHeight(tree_t *tree) {
    tree->height = 1 + max(treeHeight(tree->children[0]),
                           treeHeight(tree->children[1]));
}

/** @brief Performs rotation on given avl tree.
 * If @p type is 0, left rotation is performed, if @p type is 1, right rotation
 * is performed.
 * @param tree [in, out]   - pointer to tree;
 * @param type             - type of rotation.
 * @return Pointer to new root of tree.
 */
static tree_t *avlRotate(tree_t *tree, int type) {
    tree_t *t1 = tree->children[1 - type];
    tree_t *t2 = t1->children[type];

    t1->children[type] = tree;
    tree->children[1 - type] = t2;

    updateTreeHeight(tree);
    updateTreeHeight(t1);

    return t1;
}

/** @brief Finds node with minimal key in tree.
 * @param tree [in, out]   - pointer to tree.
 * @return Pointer to node with minimal key.
 */
static tree_t *findMinKeyNode(tree_t *tree) {
    tree_t *tmpNode = tree;

    while (tmpNode->children[0] != NULL)
        tmpNode = tmpNode->children[0];

    return tmpNode;
}

/** @brief Utility function to destroy objects stored in node.
 * Deletion type specify if keys and values should be destroyed.
 * Value @p 0 -> nothing to destroy, value @p 1 -> only keys to destroy,
 * value @p 2 -> only values to destroy, value @p 3 keys and value to destroy.
 * @param map [in, out]    - pointer to map;
 * @param node [in, out]   - pointer to node;
 * @param type             - deletion type.
 */
static void nodeObjectsDestroy(map_t *map, tree_t *node, int type) {
    if (type >= 2) {
        map->valueDestructor(node->value);
        type -= 2;
    }
    if (type > 0)
        map->keyDestructor(node->key);
}

/** @brief Recursive function to insert new element into AVL tree.
 * Do nothing if key is already stored in tree.
 * @param map [in, out]   - pointer to map;
 * @param tree [in, out]  - pointer to tree;
 * @param key  [in, out]  - pointer to stored key;
 * @param value [in, out] - pointer to stored value;
 * @param added [in, out] - pointer to flag indicating
 *                          if insertion was really performed.
 * @return Pointer to new root of tree.
 */
static tree_t *avlInsert(map_t *map, tree_t *tree, void *key, void *value,
                         bool *added) {
    tree_t *newNode;
    int balanceFactor;

    // Inserting new node
    if (tree == NULL) {
        newNode = createNode(key, value);
        if (newNode != NULL)
            *added = true;
        return newNode;
    }

    // Standard BST insertion
    if (map->equals(tree->key, key)) {
        *added = true;
        return tree;
    } else if (map->compare(key, tree->key)) {
        tree->children[0] = avlInsert(map, tree->children[0], key, value,
                                      added);
    } else {
        tree->children[1] = avlInsert(map, tree->children[1], key, value,
                                      added);
    }

    updateTreeHeight(tree);

    balanceFactor = getTreeBalance(tree);

    // Performing rotations if tree is unbalanced
    if (balanceFactor > 1 && map->compare(key, tree->children[0]->key))
        return avlRotate(tree, RIGHT);
    if (balanceFactor < -1 && map->compare(tree->children[1]->key, key))
        return avlRotate(tree, LEFT);
    if (balanceFactor > 1 && map->compare(tree->children[0]->key, key)) {
        tree->children[0] = avlRotate(tree->children[0], LEFT);
        return avlRotate(tree, RIGHT);
    }
    if (balanceFactor < -1 && map->compare(key, tree->children[1]->key)) {
        tree->children[1] = avlRotate(tree->children[1], RIGHT);
        return avlRotate(tree, LEFT);
    }

    return tree;
}

/** @brief Recursive function to find and delete node from given tree.
 * Do nothing if key is not stored in tree.
 * Deletion type specify if keys and values should be freed.
 * Value @p 0 -> nothing to delete, value @p 1 -> only keys to delete,
 * value @p 2 -> only values to delete, value @p 3 both keys and value to delete.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in, out]    - pointer to map;
 * @param tree [in, out]   - pointer to tree;
 * @param key [in, out]    - pointer to key;
 * @param removed [out]    - flag indicates if node was really removed;
 * @param type [in]        - deletion type.
 * @return Pointer to root of given tree after deleting node.
 */
static tree_t *avlDeleteNode(map_t *map, tree_t *tree, void *key, bool *removed,
                             int type) {
    tree_t *tmpNode;
    int balanceFactor;

    // Standard BST deletion
    if (tree == NULL)
        return tree;
    if (map->equals(key, tree->key)) {
        // Deleted node has no or 1 child
        if (tree->children[0] == NULL || tree->children[1] == NULL) {
            tmpNode = tree->children[0] == NULL ? tree->children[1]
                                                : tree->children[0];

            // Deleted node has no children
            if (tmpNode == NULL) {
                tmpNode = tree;
                nodeObjectsDestroy(map, tree, type);
                free(tree->children);
                tree = NULL;
            } else {
                nodeObjectsDestroy(map, tree, type);
                tree->key = tmpNode->key;
                tree->value = tmpNode->value;
                tree->children[0] = tmpNode->children[0];
                tree->children[1] = tmpNode->children[1];
                free(tmpNode->children);
            }
            free(tmpNode);
        } else {
            // Finding node with minimum value in right sub-tree
            tmpNode = findMinKeyNode(tree->children[1]);

            nodeObjectsDestroy(map, tree, type);

            // Moving stored variables from this node
            tree->key = tmpNode->key;
            tree->value = tmpNode->value;

            // Deleting this node
            tree->children[1] = avlDeleteNode(map, tree->children[1],
                                              tmpNode->key,
                                              removed, 0);
        }
        // Marking that node was removed from tree
        *removed = true;
    } else if (map->compare(key, tree->key)) {
        tree->children[0] = avlDeleteNode(map, tree->children[0], key, removed,
                                          type);
    } else {
        tree->children[1] = avlDeleteNode(map, tree->children[1], key, removed,
                                          type);
    }

    if (tree == NULL)
        return tree;

    updateTreeHeight(tree);

    balanceFactor = getTreeBalance(tree);

    // Performing rotations if tree is unbalanced
    if (balanceFactor > 1) {
        if (getTreeBalance(tree->children[0]) >= 0) {
            return avlRotate(tree, RIGHT);
        } else {
            tree->children[0] = avlRotate(tree->children[0], LEFT);
            return avlRotate(tree, RIGHT);
        }
    }
    if (balanceFactor < -1) {
        if (getTreeBalance(tree->children[1]) <= 0) {
            return avlRotate(tree, LEFT);
        } else {
            tree->children[1] = avlRotate(tree->children[1], RIGHT);
            return avlRotate(tree, LEFT);
        }
    }

    return tree;
}

/** @brief Deletes avlTree and frees previously allocated space.
 * Deletion type specify if keys and values should be freed.
 * Value @p 0 -> nothing to delete, value @p 1 -> only keys to delete,
 * value @p 2 -> only values to delete, value @p 3 both keys and value to delete.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in]   - pointer to map;
 * @param tree [in]  - pointer to tree;
 * @param type [in]  - defines type of deletion.
 */
static void avlDelete(map_t *map, tree_t *tree, int type) {
    if (tree->children[0] != NULL)
        avlDelete(map, tree->children[0], type);
    if (tree->children[1] != NULL)
        avlDelete(map, tree->children[1], type);

    free(tree->children);

    nodeObjectsDestroy(map, tree, type);

    free(tree);
}

/** @brief Prints all keys in weak order.
 * @param map [in, out]    - pointer to map;
 * @param node [in, out]   - pointer to tree.
 */
static void avlPrintKeys(map_t *map, tree_t *node) {
    if (node->children[0] != NULL)
        avlPrintKeys(map, node->children[0]);

    map->printer(node->key);

    if (node->children[1] != NULL)
        avlPrintKeys(map, node->children[1]);
}

/** @brief Adds keys from given subtree to list.
 * In list keys are stored in weak ordering.
 * @param tree [in, out]   - pointer to tree;
 * @param head [in, out]   - double pointer to head of list;
 * @param error [out]      - flag indicates if error occurred.
 */
static void avlGetKeyList(tree_t *tree, list_t **head, bool *error) {
    if (tree->children[1] != NULL)
        avlGetKeyList(tree->children[1], head, error);

    if (!addList(head, tree->key))
        *error = true;

    if (tree->children[0] != NULL)
        avlGetKeyList(tree->children[0], head, error);
}

/** @brief Adds values from given subtree to list.
 * In list values are in that order, that paired keys create weak ordering.
 * @param tree [in, out]   - pointer to tree;
 * @param head [in, out]   - double pointer to head of list;
 * @param error [out]      - flag indicates if error occurred.
 */
static void avlGetValuesList(tree_t *tree, list_t **head, bool *error) {
    if (tree->children[1] != NULL)
        avlGetValuesList(tree->children[1], head, error);

    if (!addList(head, tree->value))
        *error = true;

    if (tree->children[0] != NULL)
        avlGetValuesList(tree->children[0], head, error);
}

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
               void (*valueDestructor)(void *)) {
    map_t *newMap = (map_t *) malloc(sizeof(map_t));

    if (newMap == NULL)
        return NULL;

    newMap->compare = compare;
    newMap->equals = equals;
    newMap->printer = printer;
    newMap->keyDestructor = keyDestructor;
    newMap->valueDestructor = valueDestructor;
    newMap->root = NULL;

    return newMap;
}

/** @brief Inserts new pair <key, value> to map.
 * Do nothing if key already exists in map.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key;
 * @param value [in]      - pointer to value.
 * @return Value @p true if pair was inserted or already exist value @p false
 * if allocation error occurred.
 */
bool mapInsert(map_t *map, void *key, void *value) {
    bool added = false;

    if (map == NULL)
        return false;

    map->root = avlInsert(map, map->root, key, value, &added);

    return added;
}

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
bool mapRemove(map_t *map, void *key, int type) {
    bool removed = false;

    if (map == NULL)
        return false;
    map->root = avlDeleteNode(map, map->root, key, &removed, type);

    return removed;
}

/** @brief Returns mapped value for given key.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key.
 * @return Pointer to stored value or NULL if key is not stored in map.
 */
void *mapGet(map_t *map, void *key) {
    tree_t *tmpNode;

    if (map == NULL)
        return NULL;
    if (map->root == NULL)
        return NULL;

    tmpNode = map->root;

    while (tmpNode != NULL) {
        if (map->equals(key, tmpNode->key))
            return tmpNode->value;
        tmpNode = map->compare(key, tmpNode->key) ? tmpNode->children[0]
                                                  : tmpNode->children[1];
    }

    return NULL;
}

/** @brief Checks if map contains given key.
 * @param map [in, out]   - pointer to map;
 * @param key [in]        - pointer to key.
 * @return Value @p true if map contains given key, otherwise value @p false.
 */
bool mapContains(map_t *map, void *key) {
    return (mapGet(map, key) != NULL);
}

/** @brief Deletes map and frees previously allocated space.
 * Deletion type specify if keys and values should be freed.
 * Value @p 0 -> nothing to delete, value @p 1 -> only keys to delete,
 * value @p 2 -> only values to delete, value @p both keys and value to delete.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in]   - pointer to map;
 * @param type [in]  - defines type of deletion.
 */
void mapDelete(map_t *map, int type) {
    if (map == NULL)
        return;
    if (map->root != NULL)
        avlDelete(map, map->root, type);

    free(map);
}

/** @brief Prints keys to standard output using given printer in weak ordering.
 * @param map [in, out]    - pointer to map.
 */
void mapPrintKeys(map_t *map) {
    if (map == NULL)
        return;
    if (map->root == NULL)
        return;

    avlPrintKeys(map, map->root);
}

/** @brief Creates list of keys in weak ordering.
 * @param map [in, out]   - pointer to map.
 * @return Pointer to created list or NULL if allocation error occurred.
 */
list_t *mapGetKeysList(map_t *map) {
    bool error = false;
    list_t *keyList = newList();

    if (keyList == NULL)
        return NULL;
    if (map->root == NULL)
        return keyList;

    avlGetKeyList(map->root, &keyList, &error);

    if (error) {
        deleteList(&keyList);
        return NULL;
    }

    return keyList;
}

/** @brief Creates list of values.
 * List is in that order, that paired keys create weak ordering.
 * @param map [in, out]   - pointer to map.
 * @return Pointer to created list or NULL if allocation error occurred.
 */
list_t *mapGetValuesList(map_t *map) {
    bool error = false;
    list_t *valueList = newList();

    if (valueList == NULL)
        return NULL;
    if (map->root == NULL)
        return valueList;

    avlGetValuesList(map->root, &valueList, &error);

    if (error) {
        deleteList(&valueList);
        return NULL;
    }

    return valueList;
}

/** @brief Removes all data and frees previously allocated space.
 * Clear type specify if keys and values should be freed.
 * Value @p 0 -> nothing to free, value @p 1 -> only keys to free,
 * value @p 2 -> only values to free, value @p both keys and value to free.
 * Given (in @ref mapInit) destructors are used.
 * @param map [in]   - pointer to map;
 * @param type [in]  - defines type of clear.
 */
void mapClear(map_t *map, int type) {
    if (map == NULL)
        return;
    if (map->root == NULL)
        return;

    avlDelete(map, map->root, type);

    map->root = NULL;
}