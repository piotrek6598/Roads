/** @file
 *  Linked list C implementation.
 *
 *  @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 *  @date 24.02.2020
 */

#include "list.h"

#include <stdbool.h>
#include <stdlib.h>

/** newList creates new empty list.
 * @return Pointer to new empty list or NULL if allocation problem occurred.
 */
list_t* newList(){
    list_t *list = (list_t*)malloc(sizeof(list_t));
    if (list == NULL)
        return NULL;
    list->next = NULL;
    list->value = NULL;
    return list;
}

/** addList inserts new value in front of list.
 * @param head [in, out]    - pointer to list's head's pointer;
 * @param value [in, out]   - pointer to inserted value.
 * @return Value @p true if element was added, otherwise value @p false.
 */
bool addList(list_t **head, void *value){
    list_t *node = (list_t*)malloc(sizeof(list_t));
    if (node == NULL)
        return false;
    node->next = *head;
    node->value = value;
    *head = node;
    return true;
}

/** containsList checks if given element is stored in list.
 * @param head [in, out]    - pointer to list's head's pointer;
 * @param value [in, out]   - pointer to searched element.
 * @return Value @p true if given element is stored in list, otherwise value
 * @p false.
 */
bool containsList(list_t **head, void *value){
    list_t *tmpNode = *head;
    while (tmpNode != NULL){
        if (value == tmpNode->value)
            return true;
        tmpNode = tmpNode->next;
    }
    return false;
}

/** removeList removes given element from list.
 * @param head [in, out]    - pointer to list's head's pointer;
 * @param value [in, out]   - pointer to element.
 * @return Value @p true if element was removed, otherwise value @p false.
 */
bool removeList(list_t **head, void *value){
    list_t *tmpNode = *head;
    list_t *prev = NULL;
    while (tmpNode != NULL){
        if (value == tmpNode->value){
            if (prev != NULL)
                prev->next = tmpNode->next;
            else
                *head = tmpNode->next;
            free(tmpNode);
            return true;
        }
        prev = tmpNode;
        tmpNode = tmpNode->next;
    }
    return false;
}

/** removeHeadList removes first element from list.
 * @param head [in, out]   - pointer to list's head's pointer.
 * @return Pointer to removed element or NULL if list is empty.
 */
void* removeHeadList(list_t **head){
    list_t *tmpNode = *head;
    if (tmpNode->next == NULL)
        return NULL;
    void *value = tmpNode->value;
    *head = tmpNode->next;
    free(tmpNode);
    return value;
}

/** deleteList removes all elements from list.
 * @param head [in, out]    - pointer to list's head's pointer.
 */
void deleteList(list_t **head){
    list_t *tmpNode = *head;
    list_t *next;
    while (tmpNode != NULL){
        next = tmpNode->next;
        free(tmpNode);
        tmpNode = next;
    }
    *head = NULL;
}

/** emptyList checks if list is empty.
 * @param head [in, out]   - pointer to list's head's pointer.
 * @return Value @p true if list is empty, otherwise value @p false.
 */
bool emptyList(list_t **head){
    return (*head)->next == NULL;
}