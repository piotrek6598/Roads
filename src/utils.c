//
// Created by piotr on 19.07.2020.
//

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


bool cityNameCmp(void *name1, void *name2) {
    return strcmp((const char *) name1, (const char *) name2) <= 0;
}

bool cityNameEq(void *name1, void *name2) {
    return strcmp((const char *) name1, (const char *) name2) == 0;
}

void printCityName() {}

bool checkCityName(const char *name) {
    size_t len = strlen(name);
    for (size_t i = 0; i < len; i++) {
        if (name[i] == ';' || (name[i] >= 0 && name[i] <= 31))
            return false;
    }
    return true;
}

bool checkYear(int year) {
    return year != 0;
}

bool checkLength(unsigned length) {
    return length > 0;
}

bool checkRouteId(unsigned routeId) {
    return routeId > 0 && routeId < 1000;
}