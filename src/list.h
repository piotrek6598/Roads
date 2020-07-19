/** @file
 * Linked list interface. List stores pointer to any data type.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 24.02.2020
 */
#ifndef CSTRUCTURES_LIST_H
#define CSTRUCTURES_LIST_H

#include <stdbool.h>

/**
 * List's node contains pointer to stored value and pointer to next list's node.
 */
typedef struct list {
    void *value;       /**< pointer to stored value */
    struct list *next; /**< pointer to next list's node */
} list_t;

/** newList creates new empty list.
 * @return Pointer to new empty list or NULL if allocation problem occurred.
 */
list_t *newList();
/** addList inserts new value in front of list.
 * @param head [in, out]    - pointer to list's head's pointer;
 * @param value [in, out]   - pointer to inserted value.
 * @return Value @p true if element was added, otherwise value @p false.
 */
bool addList(list_t **head, void *value);
/** containsList checks if given element is stored in list.
 * @param head [in, out]    - pointer to list's head's pointer;
 * @param value [in, out]   - pointer to searched element.
 * @return Value @p true if given element is stored in list, otherwise value
 * @p false.
 */
bool containsList(list_t **head, void *value);

/** removeList removes given element from list.
 * @param head [in, out]    - pointer to list's head's pointer;
 * @param value [in, out]   - pointer to element.
 * @return Value @p true if element was removed, otherwise value @p false.
 */
bool removeList(list_t **head, void *value);

/** removeHeadList removes first element from list.
 * @param head [in, out]   - pointer to list's head's pointer.
 * @return Pointer to removed element or NULL if list is empty.
 */
void *removeHeadList(list_t **head);

/** deleteList removes all elements from list.
 * @param head [in, out]    - pointer to list's head's pointer.
 */
void deleteList(list_t **head);
/** emptyList checks if list is empty.
 * @param head [in, out]   - pointer to list's head's pointer.
 * @return Value @p true if list is empty, otherwise value @p false.
 */
bool emptyList(list_t **head);

#endif //CSTRUCTURES_LIST_H
