#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define MAX_CITIES 15
#define R 6371 // Radius of Earth in kilometers
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

City cities[MAX_CITIES];
int numCities;
double distances[MAX_CITIES][MAX_CITIES];
int finalPath[MAX_CITIES + 1];
double finalRes = DBL_MAX;

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

double toRadians(double degree) {
    return degree * (M_PI / 180.0);
}

double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dlat = toRadians(lat2 - lat1);
    double dlon = toRadians(lon2 - lon1);
    lat1 = toRadians(lat1);
    lat2 = toRadians(lat2);

    double a = sin(dlat / 2) * sin(dlat / 2) +
               sin(dlon / 2) * sin(dlon / 2) * cos(lat1) * cos(lat2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

void calculateDistances() {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i == j) {
                distances[i][j] = 0;
            } else {
                distances[i][j] = haversine(cities[i].latitude, cities[i].longitude, cities[j].latitude, cities[j].longitude);
            }
        }
    }
}

void copyToFinal(int currPath[]) {
    for (int i = 0; i < numCities; i++) {
        finalPath[i] = currPath[i];
    }
    finalPath[numCities] = currPath[0];
}

double firstMin(double adj[MAX_CITIES][MAX_CITIES], int i) {
    double min = DBL_MAX;
    for (int k = 0; k < numCities; k++) {
        if (adj[i][k] < min && i != k) {
            min = adj[i][k];
        }
    }
    return min;
}

double secondMin(double adj[MAX_CITIES][MAX_CITIES], int i) {
    double first = DBL_MAX, second = DBL_MAX;
    for (int j = 0; j < numCities; j++) {
        if (i == j) continue;
        if (adj[i][j] <= first) {
            second = first;
            first = adj[i][j];
        } else if (adj[i][j] <= second && adj[i][j] != first) {
            second = adj[i][j];
        }
    }
    return second;
}

void TSPRec(double adj[MAX_CITIES][MAX_CITIES], double currBound, double currWeight,
            int level, int currPath[], int visited[]) {
    if (level == numCities) {
        if (adj[currPath[level - 1]][currPath[0]] != 0) {
            double currRes = currWeight + adj[currPath[level - 1]][currPath[0]];
            if (currRes < finalRes) {
                copyToFinal(currPath);
                finalRes = currRes;
            }
        }
        return;
    }

    for (int i = 0; i < numCities; i++) {
        if (adj[currPath[level - 1]][i] != 0 && visited[i] == 0) {
            double temp = currBound;
            currWeight += adj[currPath[level - 1]][i];

            if (level == 1) {
                currBound -= ((firstMin(adj, currPath[level - 1]) + firstMin(adj, i)) / 2.0);
            } else {
                currBound -= ((secondMin(adj, currPath[level - 1]) + firstMin(adj, i)) / 2.0);
            }

            if (currBound + currWeight < finalRes) {
                currPath[level] = i;
                visited[i] = 1;

                TSPRec(adj, currBound, currWeight, level + 1, currPath, visited);
            }

            currWeight -= adj[currPath[level - 1]][i];
            currBound = temp;

            memset(visited, 0, sizeof(int) * numCities);
            for (int j = 0; j <= level - 1; j++) {
                visited[currPath[j]] = 1;
            }
        }
    }
}

void TSP(double adj[MAX_CITIES][MAX_CITIES], int startCityIndex) {
    int currPath[MAX_CITIES + 1];
    int visited[MAX_CITIES] = {0};

    double currBound = 0;
    memset(currPath, -1, sizeof(currPath));
    memset(finalPath, -1, sizeof(finalPath));

    for (int i = 0; i < numCities; i++) {
        currBound += (firstMin(adj, i) + secondMin(adj, i));
    }

    currBound = (fmod(currBound, 2) == 1) ? currBound / 2.0 + 1 : currBound / 2.0;

    visited[startCityIndex] = 1;
    currPath[0] = startCityIndex;

    TSPRec(adj, currBound, 0, 1, currPath, visited);
}

int findCityIndex(char *cityName) {
    for (int i = 0; i < numCities; i++) {
        if (strcmp(cities[i].name, cityName) == 0) {
            return i;
        }
    }
    return -1;
}

void readCitiesFromFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[100];
    numCities = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%lf,%lf", cities[numCities].name, &cities[numCities].latitude, &cities[numCities].longitude);
        numCities++;
    }

    fclose(file);
}

int main() {
    char fileName[100];
    char startCity[50];
    time_t start, end;

    printf("Enter list of cities file name: ");
    scanf("%s", fileName);

    printf("Enter starting point: ");
    scanf("%s", startCity);

    readCitiesFromFile(fileName);
    calculateDistances();

    int startCityIndex = findCityIndex(startCity);
    if (startCityIndex == -1) {
        printf("Starting city not found in the list.\n");
        return 1;
    }

    start = time(NULL);
    TSP(distances, startCityIndex);
    end = time(NULL);

    double timeTaken = difftime(end, start);

    printf("Best route found:\n");
    for (int i = 0; i <= numCities; i++) {
        printf("%s", cities[finalPath[i]].name);
        if (i < numCities) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.4f km\n", finalRes);
    printf("Time elapsed: %.10f s\n", timeTaken);

    return 0;
}
