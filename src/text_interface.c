/** @file
 * Implementation of text interface.
 *
 * @author Piotr Jasinski <jasinskipiotr99@gmail.com>
 * @date 21.07.2020
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "text_interface.h"
#include "map.h"
#include "utils.h"

/**
 * Macro defining exit code in case of map module function success.
 */
#define SUCCESS 0
/**
 * Macro defining exit code in case of map module function error.
 */
#define ERROR 1
/**
 * Macro defining critical error like lack ot memory etc.
 */
#define CRITICAL_ERROR 2

/** @brief Executes @ref createRoute.
 * Converts route description in format like in @ref getRouteDescription to
 * road descriptions acceptable by @ref createRoute.
 * Checks if new route makes path.
 * Assumes that routeId; was already removed from description and rest
 * of description is available through strtok function.
 * @param map [in,out] - double pointer to map,
 * @param routeId [in] - route number.
 * @return Value @ref SUCCESS, @ref ERROR depending on function result.
 * Value @ref CRITICAL_ERROR if allocation error occurred.
 */
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

/** @brief Executes @ref addRoad.
 * Assumes that description city1;city2;length;year is available
 * through strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeAddRoad(Map **map) {
    char *args[4];
    int year;
    unsigned length;

    for (int i = 0; i < 4; i++) {
        args[i] = strtok(NULL, ";");
        if (args[i] == NULL)
            return ERROR;
    }

    length = parseStringToUnsigned(args[2]);
    year = parseStringToInt(args[3]);

    if (strtok(NULL, ";") != NULL)
        return ERROR;
    if (addRoad(*map, args[0], args[1], length, year)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Executes @ref repairRoad.
 * Assumes that description city1;city2;year is available through strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeRepairRoad(Map **map) {
    char *args[3];
    int year;

    for (int i = 0; i < 3; i++) {
        args[i] = strtok(NULL, ";");
        if (args[i] == NULL)
            return ERROR;
    }

    year = parseStringToInt(args[2]);

    if (strtok(NULL, ";") != NULL)
        return ERROR;
    if (repairRoad(*map, args[0], args[1], year)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Executes @ref getRouteDescription.
 * Assumes that route number is available through strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeGetRouteDescription(Map **map) {
    char *route_id = strtok(NULL, ";");
    unsigned route;
    const char *desc;

    if (route_id == NULL)
        return ERROR;
    if (strtok(NULL, ";") != NULL)
        return ERROR;

    route = parseStringToUnsigned(route_id);
    desc = getRouteDescription(*map, route);

    if (desc != NULL) {
        printf("%s\n", desc);
        free((void *) desc);
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Executes @ref newRoute
 * Assumes that description routeId;city1;city2 is available through
 * strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeNewRoute(Map **map) {
    char *args[3];
    unsigned route_id;

    for (int i = 0; i < 3; i++) {
        args[i] = strtok(NULL, ";");
        if (args[i] == NULL)
            return ERROR;
    }
    if (strtok(NULL, ";") != NULL)
        return ERROR;
    route_id = parseStringToUnsigned(args[0]);
    if (newRoute(*map, route_id, args[1], args[2])) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Executes @ref extendRoute.
 * Assumes that description routeId;city is available through strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeExtendRoute(Map **map) {
    char *args[2];
    unsigned route_id;

    for (int i = 0; i < 2; i++) {
        args[i] = strtok(NULL, ";");
        if (args[i] == NULL)
            return ERROR;
    }
    if (strtok(NULL, ";") != NULL)
        return ERROR;

    route_id = parseStringToUnsigned(args[0]);

    if (extendRoute(*map, route_id, args[1])) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Executes @ref removeRoad.
 * Assumes that description city1;city2 is available through strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeRemoveRoad(Map **map) {
    char *args[2];

    for (int i = 0; i < 2; i++) {
        args[i] = strtok(NULL, ";");
        if (args[i] == NULL)
            return ERROR;
    }
    if (strtok(NULL, ";") != NULL)
        return ERROR;
    if (removeRoad(*map, args[0], args[1])) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Executes @ref removeRoute.
 * Assumes that route number is available through strtok function.
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS or @ref ERROR depending on function result.
 */
static int executeRemoveRoute(Map **map) {
    char *arg = strtok(NULL, ";");
    unsigned route_id;

    if (arg == NULL)
        return ERROR;
    if (strtok(NULL, ";") != NULL)
        return ERROR;

    route_id = parseStringToUnsigned(arg);

    if (removeRoute(*map, route_id)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/** @brief Parses text line and executes requested function.
 * If line is empty or is a comment does nothing.
 * @param line [in]    - pointer to text line,
 * @param map [in,out] - double pointer to map.
 * @return Value @ref SUCCESS if line describes correct command and function
 * has ended with @ref SUCCESS or line is command or empty. Otherwise value
 * @ref ERROR.
 */
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
    if (strcmp(command, "newRoute") == 0)
        return executeNewRoute(map);
    if (strcmp(command, "extendRoute") == 0)
        return executeExtendRoute(map);
    if (strcmp(command, "removeRoad") == 0)
        return executeRemoveRoad(map);
    if (strcmp(command, "removeRoute") == 0)
        return executeRemoveRoute(map);
    route_id = parseStringToUnsigned(command);
    if (route_id > 0 && route_id < 1000)
        return executeCreateRoute(map, route_id);
    return ERROR;
}

/** @brief Runs text interface.
 * @return Value @p 0.
 */
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
