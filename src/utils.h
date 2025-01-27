/** @file
 * Utilities used by other modules.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 21.07.2020
 */

#ifndef ROADS_UTILS_H
#define ROADS_UTILS_H

bool cityNameCmp(void *name1, void *name2);

bool cityNameEq(void *name1, void *name2);

void printCityName();

bool checkCityName(const char *name);

bool checkYear(int year);

bool checkLength(unsigned length);

bool checkRouteId(unsigned routeId);

size_t getNumberLength(long long int num);

int powerOf2Greater(int num);

unsigned parseStringToUnsigned(char *text);

int parseStringToInt(char *text);

bool checkIfSemicolonLast(char *text);

#endif //ROADS_UTILS_H
