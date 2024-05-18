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
int finalPath[MAX_CITIES + 1];
double finalRes = DBL_MAX;

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

double haversine(double lat1, double lon1, double lat2, double lon2){
    lat1 = lat1*(3.14159265359/180);
    lon1 = lon1*(3.14159265359/180);
    lat2 = lat2*(3.14159265359/180);
    lon2 = lon2*(3.14159265359/180);
    return 2*6371*asin(sqrt(pow(sin((lat2-lat1)/2),2)+cos(lat1)*cos(lat2)*pow(sin((lon2-lon1)/2),2)));
}

void copyToFinal(int currPath[], int len) {
    for (int i = 0; i < len; i++) {
        finalPath[i] = currPath[i];
    }
    finalPath[len] = currPath[0];
}

double firstMin(double adj[MAX_CITIES][MAX_CITIES], int i, int len) {
    double min = DBL_MAX;
    for (int k = 0; k < len; k++) {
        if (adj[i][k] < min && i != k) {
            min = adj[i][k];
        }
    }
    return min;
}

double secondMin(double adj[MAX_CITIES][MAX_CITIES], int i, int len) {
    double first = DBL_MAX, second = DBL_MAX;
    for (int j = 0; j < len; j++) {
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
            int level, int currPath[], int visited[], int len) {
    if (level == len) {
        if (adj[currPath[level - 1]][currPath[0]] != 0) {
            double currRes = currWeight + adj[currPath[level - 1]][currPath[0]];
            if (currRes < finalRes) {
                copyToFinal(currPath, len);
                finalRes = currRes;
            }
        }
        return;
    }

    for (int i = 0; i < len; i++) {
        if (adj[currPath[level - 1]][i] != 0 && visited[i] == 0) {
            double temp = currBound;
            currWeight += adj[currPath[level - 1]][i];

            if (level == 1) {
                currBound -= ((firstMin(adj, currPath[level - 1], len) + firstMin(adj, i, len)) / 2.0);
            } else {
                currBound -= ((secondMin(adj, currPath[level - 1], len) + firstMin(adj, i, len)) / 2.0);
            }

            if (currBound + currWeight < finalRes) {
                currPath[level] = i;
                visited[i] = 1;

                TSPRec(adj, currBound, currWeight, level + 1, currPath, visited, len);
            }

            currWeight -= adj[currPath[level - 1]][i];
            currBound = temp;

            memset(visited, 0, sizeof(int) * len);
            for (int j = 0; j <= level - 1; j++) {
                visited[currPath[j]] = 1;
            }
        }
    }
}

void TSP(double adj[MAX_CITIES][MAX_CITIES], int startCityIndex, int len) {
    int currPath[MAX_CITIES + 1];
    int visited[MAX_CITIES] = {0};

    double currBound = 0;
    memset(currPath, -1, sizeof(currPath));
    memset(finalPath, -1, sizeof(finalPath));

    for (int i = 0; i < len; i++) {
        currBound += (firstMin(adj, i, len) + secondMin(adj, i, len));
    }

    currBound = (fmod(currBound, 2) == 1) ? currBound / 2.0 + 1 : currBound / 2.0;

    visited[startCityIndex] = 1;
    currPath[0] = startCityIndex;

    TSPRec(adj, currBound, 0, 1, currPath, visited, len);
}

int findCityIndex(char *cityName, int len) {
    for (int i = 0; i < len; i++) {
        if (strcmp(cities[i].name, cityName) == 0) {
            return i;
        }
    }
    return -1;
}

void readCitiesFromFile(char *fileName, int *len) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[100];

    *len = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%lf,%lf", cities[*len].name, &cities[*len].latitude, &cities[*len].longitude);
        (*len)++;
    }

    fclose(file);
}

int main() {
    char fileName[100];
    char startCity[50];
    time_t start, end;

    int len;
    double latlong[MAX_CITIES][2];

    printf("Enter list of cities file name: ");
    scanf("%s", fileName);

    printf("Enter starting point: ");
    scanf("%s", startCity);

    readCitiesFromFile(fileName, &len);

    int startCityIndex = findCityIndex(startCity, len);
    if (startCityIndex == -1) {
        printf("Starting city not found in the list.\n");
        return 1;
    }

    // Initialize the adjacency matrix for distances between cities
    double adj[MAX_CITIES][MAX_CITIES];
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            if (i == j) {
                adj[i][j] = 0;
            } else {
                adj[i][j] = haversine(cities[i].latitude, cities[i].longitude, cities[j].latitude, cities[j].longitude);
            }
        }
    }

    start = time(NULL);
    TSP(adj, startCityIndex, len);
    end = time(NULL);

    double timeTaken = difftime(end, start);

    printf("Best route found:\n");
    for (int i = 0; i <= len; i++) {
        printf("%s", cities[finalPath[i]].name);
        if (i < len) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.4f km\n", finalRes);
    printf("Time elapsed: %.10f s\n", timeTaken);

    return 0;
}