#include "map.h"

#include <stdlib.h>
#include <string.h>

#undef NDEBUG

#include <assert.h>
#include <stdio.h>

#define DK(_map, _dk, _descr)                           \
  do {                                                  \
    char const *_str = getRouteDescription(_map, _dk);  \
    assert(_str);                                       \
    assert(strcmp(_str, _descr) == 0);                  \
    free((void *)_str);                                 \
  } while (0)

int forum1(void) {
    Map *m = newMap();
    assert(m);

    assert(addRoad(m, "poczatek", "a", 10, 2019));
    assert(addRoad(m, "a", "c", 10, 2019));
    assert(addRoad(m, "c", "koniec", 10, 2019));

    assert(newRoute(m, 1, "poczatek", "koniec"));

    assert(addRoad(m, "poczatek", "koniec", 1, 2019));
    assert(addRoad(m, "a", "b", 100, 2019));
    assert(addRoad(m, "b", "c", 100, 2019));
    assert(addRoad(m, "a", "koniec", 1, 2019));

    assert(removeRoad(m, "a", "c"));

    DK(m, 1, "1;poczatek;10;2019;a;100;2019;b;100;2019;c;10;2019;koniec");

    deleteMap(m);
    return 0;
}

int forum3(void) {
    Map* m = newMap();
    assert(m);

    assert(addRoad(m, "A", "B", 1, 2000));
    assert(addRoad(m, "B", "C", 1, 2000));
    assert(addRoad(m, "C", "D", 1, 2000));
    assert(addRoad(m, "C", "E", 1, 2000));
    assert(addRoad(m, "E", "F", 1, 2000));
    assert(addRoad(m, "E", "D", 7, 2000));

    assert(newRoute(m, 10, "A", "D"));
    assert(extendRoute(m, 10, "F"));

    DK(m, 10, "10;A;1;2000;B;1;2000;C;1;2000;D;7;2000;E;1;2000;F");

    assert(newRoute(m, 11, "A", "F"));

    DK(m, 11, "11;A;1;2000;B;1;2000;C;1;2000;E;1;2000;F");

    deleteMap(m);
    return 0;
}

int description2(void) {
    Map* m = newMap();
    assert(m);

    for (unsigned dk = 1; dk <= 999; ++dk)
        DK(m, dk, "");

    deleteMap(m);
    return 0;
}

int route5(void) {
    Map *m = newMap();
    assert(m);

    assert(addRoad(m, "A", "B", 1, 20));
    assert(addRoad(m, "B", "C", 2, 20));
    assert(addRoad(m, "B", "D", 3, 20));
    assert(addRoad(m, "C", "E", 4, 20));
    assert(addRoad(m, "D", "E", 5, 20));
    assert(addRoad(m, "E", "B", 8, 20));
    assert(addRoad(m, "E", "F", 3, 20));
    assert(!repairRoad(m, "E", "C", 19));
    assert(repairRoad(m, "E", "C", 21));
    assert(!repairRoad(m, "E", "A", 22));
    assert(addRoad(m, "F", "G", 4, 20));
    assert(addRoad(m, "A", "G", 10, 20));
    assert(addRoad(m, "H", "G", 3, 20));
    assert(addRoad(m, "F", "C", 2, 20));
    assert(!addRoad(m, "F", "C", 2, 20));
    assert(!addRoad(m, "C", "F", 2, 20));
    assert(!repairRoad(m, "E", "B", 18));
    assert(repairRoad(m, "E", "C", 22));
    assert(repairRoad(m, "E", "F", 23));
    assert(addRoad(m, "H", "C", 3, 20));
    assert(addRoad(m, "G", "K", 1, 20));
    assert(addRoad(m, "G", "J", 9, 20));
    assert(addRoad(m, "G", "I", 8, 20));
    assert(addRoad(m, "H", "I", 1, 20));
    assert(addRoad(m, "H", "J", 10, 20));
    assert(addRoad(m, "H", "K", 11, 20));
    assert(addRoad(m, "K", "J", 1, 20));
    assert(addRoad(m, "J", "I", 1, 20));
    assert(newRoute(m, 10, "A", "E"));

    DK(m, 10, "10;A;1;20;B;2;20;C;4;22;E");

    assert(extendRoute(m, 10, "H"));

    DK(m, 10, "10;A;1;20;B;2;20;C;4;22;E;3;23;F;4;20;G;3;20;H");

    assert(removeRoad(m, "H", "G"));

    DK(m, 10, "10;A;1;20;B;2;20;C;4;22;E;3;23;F;4;20;G;1;20;K;1;20;J;1;20;I;1;20;H");

    assert(!removeRoad(m, "J", "K"));

    DK(m, 10, "10;A;1;20;B;2;20;C;4;22;E;3;23;F;4;20;G;1;20;K;1;20;J;1;20;I;1;20;H");

    deleteMap(m);
    return 0;
}

int example() {
    char const* str;

    Map* m = newMap();
    assert(m);

    assert(addRoad(m, "Alinów", "Bór", 1, 2020));
    assert(addRoad(m, "Bór", "Cielińsk-Niekłańsk", 2, 2020));
    assert(addRoad(m, "Bór", "Dąb Stary", 3, 2020));
    assert(addRoad(m, "Cielińsk-Niekłańsk", "Emiliew", 4, 2020));
    assert(addRoad(m, "Dąb Stary", "Emiliew", 5, 2020));
    assert(addRoad(m, "Emiliew", "Bór", 8, 2020));
    assert(addRoad(m, "Emiliew", "Fraźnik Nowy", 3, 2020));
    assert(!repairRoad(m, "Emiliew", "Cielińsk-Niekłańsk", 2019));
    assert(repairRoad(m, "Emiliew", "Cielińsk-Niekłańsk", 2021));
    assert(!repairRoad(m, "Emiliew", "Alinów", 2020));
    assert(addRoad(m, "Fraźnik Nowy", "Grzegrzewo", 4, 2020));
    assert(addRoad(m, "Alinów", "Grzegrzewo", 10, 2020));
    assert(addRoad(m, "Homilcin", "Grzegrzewo", 5, 2020));
    assert(addRoad(m, "Fraźnik Nowy", "Cielińsk-Niekłańsk", 2, 2020));
    assert(!addRoad(m, "Fraźnik Nowy", "Cielińsk-Niekłańsk", 2, 2020));
    assert(!addRoad(m, "Cielińsk-Niekłańsk", "Fraźnik Nowy", 2, 2020));
    assert(!repairRoad(m, "Emiliew", "Bór", 2018));
    assert(repairRoad(m, "Emiliew", "Cielińsk-Niekłańsk", 2021));
    assert(repairRoad(m, "Emiliew", "Fraźnik Nowy", 2023));
    assert(addRoad(m, "Homilcin", "Cielińsk-Niekłańsk", 3, 2020));
    assert(newRoute(m, 10, "Alinów", "Emiliew"));

    str = getRouteDescription(m, 10);
    assert(strcmp(str, "10;Alinów;1;2020;Bór;2;2020;Cielińsk-Niekłańsk;4;2021;Emiliew") == 0);
    free((void *)str);

    assert(extendRoute(m, 10, "Homilcin"));

    str = getRouteDescription(m, 10);
    assert(strcmp(str, "10;Alinów;1;2020;Bór;2;2020;Cielińsk-Niekłańsk;4;2021;Emiliew"
                       ";3;2023;Fraźnik Nowy;4;2020;Grzegrzewo;5;2020;Homilcin") == 0);
    free((void *)str);

    deleteMap(m);

    return 0;
}
static int dk2str(unsigned dk, char *str) {
    int i = 3;
    do {
        --i;
        str[i] = dk % 10 + '0';
        dk /= 10;
    } while (dk);
    return i;
}

int memory(void) {
    Map *m = newMap();
    assert(m);

    assert(addRoad(m, "a", "b", 1, 1));
    assert(addRoad(m, "b", "c", 1, 1));
    assert(newRoute(m, 1, "a", "b"));
    assert(extendRoute(m, 1, "c"));

    /* Alokujemy całą dostępną pamięć. */
    for (size_t s = 1024 * 1024 * 1024; s >= 1; s /= 2) {
        void *p;
        do {
            p = malloc(s);
        } while (p);
    }

    while (newMap());

    for (unsigned dk = 2; dk <= 999; ++dk) {
        if (newRoute(m, dk, "a", "c")) {
            char const *str = getRouteDescription(m, dk);
            if (str) {
                static char rd[] = "000;a;1;1;b;1;1;c";
                int i = dk2str(dk, rd);
                assert(strcmp(str, rd + i) == 0);
            }
        }
        else {
            char const *str = getRouteDescription(m, dk);
            if (str) {
                assert(strcmp(str, "") == 0);
            }
        }
    }

    char city1[2], city2[2];
    city1[0] = 'c';
    city2[0] = 'd';
    city1[1] = city2[1] = '\0';

    for (; city2[0] <= 'z'; city1[0] = city2[0], city2[0]++)
        if (addRoad(m, city1, city2, 1, 1))
            if (extendRoute(m, 1, city2) == false)
                break;

    /* To jest test alokacji pamięci – nie zwalniamy jej. */
    return 0;
}

int main() {
    memory();
    return 0;
}