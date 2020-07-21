//
// Created by piotr on 20.07.2020.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "text_interface.h"
#include "map.h"
#include "utils.h"

#define SUCCESS 0
#define ERROR 1
#define CRITICAL_ERROR 2

void testList(list_t **list) {
    list_t *tmp_node = *list;
    while (tmp_node != NULL && tmp_node->value != NULL) {
        printf("%s\n", (char *) tmp_node->value);
        tmp_node = tmp_node->next;
    }
}

static int executeCreateRoute(Map **map, unsigned routeId) {
    char *road_params[4] = {NULL, NULL, NULL, NULL};
    char *road;
    int curr_param = 0;
    bool str = false;
    list_t *road_desc = newList();
    map_t *cities_name = mapInit(cityNameCmp, cityNameEq, printCityName, free,
                                 free);

    if (road_desc == NULL || cities_name == NULL) {
        deleteList(&road_desc);
        mapDelete(cities_name, 0);
        return CRITICAL_ERROR;
    }

    while ((road_params[curr_param] = strtok(NULL, ";")) != NULL) {
        if (curr_param == 0) {
            if (!mapInsert(cities_name, (void *) road_params[0], "")) {
                deleteList(&road_desc);
                mapDelete(cities_name, 0);
                return CRITICAL_ERROR;
            }
        }
        if (curr_param == 3) {
            str = true;
            unsigned long long road_desc_len = 0;
            if (mapContains(cities_name, (void *) road_params[3])) {
                while (!emptyList(&road_desc))
                    free(removeHeadList(&road_desc));
                deleteList(&road_desc);
                mapDelete(cities_name, 0);
                return ERROR;
            } else {
                if (!mapInsert(cities_name, (void *) road_params[3], "")) {
                    while (!emptyList(&road_desc))
                        free(removeHeadList(&road_desc));
                    deleteList(&road_desc);
                    mapDelete(cities_name, 0);
                    return CRITICAL_ERROR;
                }
            }
            for (int i = 0; i < 4; i++) {
                road_desc_len += strlen(road_params[i]);
                road_desc_len++;
            }
            road = malloc(sizeof(char) * road_desc_len);
            if (road == NULL) {
                while (!emptyList(&road_desc))
                    free(removeHeadList(&road_desc));
                deleteList(&road_desc);
                mapDelete(cities_name, 0);
                return CRITICAL_ERROR;
            }
            road_desc_len = 0;
            for (int i = 0; i < 4; i++) {
                strcpy(road + road_desc_len, road_params[i]);
                road_desc_len = strlen(road);
                if (i == 3)
                    road[road_desc_len] = '\0';
                else
                    road[road_desc_len] = ';';
                road_desc_len++;
            }
            if (!addList(&road_desc, (void *) road)) {
                free(road);
                while (!emptyList(&road_desc))
                    free(removeHeadList(&road_desc));
                deleteList(&road_desc);
                mapDelete(cities_name, 0);
                return CRITICAL_ERROR;
            }
            road_params[0] = road_params[3];
            curr_param = 1;
        } else {
            curr_param++;
        }
    }

    if (curr_param == 1 && str) {
        //testList(&road_desc);
        //return ERROR;
        mapDelete(cities_name, 0);
        if (createRoute(map, routeId, road_desc)) {
            return SUCCESS;
        } else {
            return ERROR;
        }
    } else {
        while (!emptyList(&road_desc))
            free(removeHeadList(&road_desc));
        deleteList(&road_desc);
        mapDelete(cities_name, 0);
        return ERROR;
    }
}

static int executeAddRoad(Map **map) {
    char *args[4];

    for (int i = 0; i < 4; i++) {
        //if (i != 3)
        args[i] = strtok(NULL, ";");
        //else
        //    args[i] = strtok(NULL, "\n");
        if (args[i] == NULL)
            return ERROR;
    }
    //if (!checkIfSemicolonLast(args[2]))
    //    return ERROR;
    //if (!checkIfSemicolonLast(args[3]))
    //    return ERROR;
    unsigned length = parseStringToUnsigned(args[2]);
    int year = parseStringToInt(args[3]);
    if (strtok(NULL, ";") != NULL)
        return ERROR;
    if (addRoad(*map, args[0], args[1], length, year)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

static int executeRepairRoad(Map **map) {
    char *args[3];
    int year;

    for (int i = 0; i < 3; i++) {
        //if (i != 2)
        args[i] = strtok(NULL, ";");
        //else {
        //    args[i] = strtok(NULL, "\n");
        //}
        if (args[i] == NULL)
            return ERROR;
    }
    //if (!checkIfSemicolonLast(args[2]))
    //    return ERROR;
    year = parseStringToInt(args[2]);
    if (strtok(NULL, ";") != NULL)
        return ERROR;
    if (repairRoad(*map, args[0], args[1], year)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

static int executeGetRouteDescription(Map **map) {
    char *route_id = strtok(NULL, ";");

    if (route_id == NULL)
        return ERROR;
    //if (!checkIfSemicolonLast(route_id))
    //    return ERROR;
    if (strtok(NULL, ";") != NULL)
        return ERROR;
    unsigned route = parseStringToUnsigned(route_id);
    const char *desc = getRouteDescription(*map, route);
    if (desc != NULL) {
        printf("%s\n", desc);
        free((void *) desc);
        return SUCCESS;
    } else {
        return ERROR;
    }
}

static int parseAndExecuteTextLine(char *line, Map **map) {
    char *command;
    unsigned route_id;

    if (*line == '#' || *line == '\n')
        return SUCCESS;

    line = strtok(line, "\n");

    if (checkIfSemicolonLast(line))
        return ERROR;

    command = strtok(line, ";");
    if (command == NULL)
        return ERROR;
    if (strcmp(command, "addRoad") == 0)
        return executeAddRoad(map);
    if (strcmp(command, "repairRoad") == 0)
        return executeRepairRoad(map);
    if (strcmp(command, "getRouteDescription") == 0)
        return executeGetRouteDescription(map);
    route_id = parseStringToUnsigned(command);
    if (route_id > 0 && route_id < 1000)
        return executeCreateRoute(map, route_id);
    return ERROR;
}

int runMapInterface() {
    size_t size;
    int line_counter = 0;
    int ret_val;
    char *line = NULL;

    Map *map = newMap();

    if (map == NULL)
        return 0;

    while (getline(&line, &size, stdin) != -1) {
        ret_val = parseAndExecuteTextLine(line, &map);

        line_counter++;

        if (ret_val == ERROR) {
            fprintf(stderr, "ERROR %d\n", line_counter);
        } else if (ret_val == CRITICAL_ERROR) {
            deleteMap(map);
            free(line);
            return 0;
        }
    }

    free(line);
    deleteMap(map);

    return 0;
}
