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