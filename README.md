# Roads

## Program description

Program simulates map of roads. Map consists of cities, roads and national routes.
Roads can be added, removed and repair. National routes between to cities should be
as short as possible. Ones defined national route can be extended, completely removed
and changed only if road is removed. Road won't be removed if it breaks any national route
and diversion is impossible.

### Installation
```
mkdir release && cd release
cmake .. && make
```
And documentation
```
make doc
```

### Usage
```
routeId;city;length;builtYear;city;...;city
```
Creates new national route with given number and path. Adds all necessary cities and roads.
If it is possible repairs roads.
```
addRoad;city1;city2;length;builtYear
```
Adds new road.
```
repairRoad;city1;city2;repairYear
```
Repairs specified road.
```
getRouteDescription;routeId
```
Prints description of route with given number.
```
newRoute;routeId;city1;city2
```
Creates new national route with given number from city1 to city2. <br>
New route is as short as possible. And if there are multiply options
chooses that with the oldest road as young as possible. 
```
extendRoute;routeId;city
```
Extends national route with given number to the city. Again choosing the shortest option.
```
removeRoad;city1;city2
```
Removes road from map. If it breaks any national route, finds diversion.<br>
If it is impossible, road can't be removed.
```
removeRoute;routeId
```
Removes national route with given number from map.