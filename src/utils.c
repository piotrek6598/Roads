/** @file
 *  Implementation of utilities used by other modules.
 *
 *  @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 *  @date 21.07.2020
 */

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/** @brief Utility function compares city names.
 * Used only by avlMaps.
 * @param name1 [in]   - first name,
 * @param name2 [in]   - second name.
 * @return Negative value if @p name1 comes first in lexicographical order,
 * @p 0 if names are the same, otherwise positive value.
 */
bool cityNameCmp(void *name1, void *name2) {
    return strcmp((const char *) name1, (const char *) name2) <= 0;
}

/** @brief Utility function checks if city names are equal.
 * Used only by avlMap.
 * @param name1 [in]   - first name,
 * @param name2 [in]   - second name.
 * @return Value @p true if names are the same. Otherwise value @p false.
 */
bool cityNameEq(void *name1, void *name2) {
    return strcmp((const char *) name1, (const char *) name2) == 0;
}

/** @brief Do nothing, but can be used to print city name. */
void printCityName() {}

/** @brief Checks in text is valid city name.
 * Name is valid if doesn't contain any character with code [0, 31] and ';'.
 * @param name [in]    - text.
 * @return Value @p true if it's valid city name. Otherwise value @p false.
 */
bool checkCityName(const char *name) {
    size_t len = strlen(name);
    if (len == 0)
        return false;
    for (size_t i = 0; i < len; i++) {
        if (name[i] == ';' || (name[i] >= 0 && name[i] <= 31))
            return false;
    }
    return true;
}

/** @brief Checks if year is valid.
 * Year is valid if is other than 0.
 * @param year [in]    - number.
 * @return Value @p true if year is valid. Otherwise value @p false.
 */
bool checkYear(int year) {
    return year != 0;
}

/** @brief Checks if road length is valid.
 * Road length is valid if it is positive number,
 * @param length [in]  - number.
 * @return Value @p true if road length is valid. Otherwise value @p false.
 */
bool checkLength(unsigned length) {
    return length > 0;
}

/** @brief Checks if route number is valid.
 * Route number is valid if belongs to [1, 999].
 * @param routeId [in] - number.
 * @return Value @p true if route number is valid. Otherwise value @p false.
 */
bool checkRouteId(unsigned routeId) {
    return routeId > 0 && routeId < 1000;
}

/** @brief Count how many chars described number.
 * @param num [in]     - number.
 * @return Number of chars describing number.
 */
size_t getNumberLength(long long int num) {
    size_t len = 0;

    if (num == 0)
        return 1;

    if (num < 0)
        len++;

    while (num != 0) {
        len++;
        num /= 10;
    }
    return len;
}

/** @brief Finds smallest power of 2 greater than specified number.
 * @param num [in]     - number.
 * @return Smallest power of 2 greater than @p num.
 */
int powerOf2Greater(int num) {
    int result = 1;

    if (num <= 0)
        return 0;

    while (result <= num)
        result *= 2;
    return result;
}

/** @brief Parses string to unsigned.
 * Returns 0 if string can't be parsed to unsigned.
 * @param text [in]    - text.
 * @return Parsed number or 0 if it's impossible.
 */
unsigned parseStringToUnsigned(char *text) {
    if (*text == '-' || *text == '\n' || *text == '+')
        return 0;

    unsigned long long int ret_val = strtoull(text, NULL, 10);
    return ret_val > UINT_MAX ? 0 : (unsigned) ret_val;
}

/** @brief Parses string to int.
 * Returns 0 if string can't be parsed to int.
 * @param text [in]    - text.
 * @return Parsed number or 0 if it's impossible.
 */
int parseStringToInt(char *text) {
    if (*text == '\n' || *text == '+')
        return 0;

    long long int ret_val = strtoll(text, NULL, 10);
    return (ret_val > INT_MAX || ret_val < INT_MIN) ? 0 : (int) ret_val;
}

/** @brief Checks if text line ends with ';'.
 * @param text [in]    - text.
 * @return Value @p true if text line ends with ';'. Otherwise value @p false.
 */
bool checkIfSemicolonLast(char *text) {
    bool ret_val = false;
    while (*text != '\0' && *text != '\n') {
        ret_val = *text == ';';
        text++;
    }
    return ret_val;
}