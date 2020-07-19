//
// Created by piotr on 19.07.2020.
//

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

#endif //ROADS_UTILS_H
