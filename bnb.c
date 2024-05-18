#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "bnb.h"

#define MAX_CITIES 15

double distances[MAX_CITIES][MAX_CITIES];
int finalPath[MAX_CITIES + 1];
double finalRes = DBL_MAX;

void swapb(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void copyToFinal(int *currPath,int numCities) {
    for (int i = 0; i < numCities; i++) {
        finalPath[i] = currPath[i];
    }
    finalPath[numCities] = currPath[0];
}

double firstMin(double **adj, int i,int numCities) {
    double min = DBL_MAX;
    for (int k = 0; k < numCities; k++) {
        if (adj[i][k] < min && i != k) {
            min = adj[i][k];
        }
    }
    return min;
}

double secondMin(double **adj, int i,int numCities) {
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

void TSPRec(double **adj, double currBound, double currWeight,int level, int *currPath, int *visited,int numCities) {
    if (level == numCities) {
        if (adj[currPath[level - 1]][currPath[0]] != 0) {
            double currRes = currWeight + adj[currPath[level - 1]][currPath[0]];
            if (currRes < finalRes) {
                copyToFinal(currPath,numCities);
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
                currBound -= ((firstMin(adj, currPath[level - 1],numCities) + firstMin(adj, i,numCities)) / 2.0);
            } else {
                currBound -= ((secondMin(adj, currPath[level - 1],numCities) + firstMin(adj, i,numCities)) / 2.0);
            }

            if (currBound + currWeight < finalRes) {
                currPath[level] = i;
                visited[i] = 1;

                TSPRec(adj, currBound, currWeight, level + 1, currPath, visited,numCities);
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

void bnb(int startCityIndex,int numCities,double **adj,char **kota) {
    int currPath[MAX_CITIES + 1];
    int visited[MAX_CITIES] = {0};

    double currBound = 0;
    memset(currPath, -1, sizeof(currPath));
    memset(finalPath, -1, sizeof(finalPath));

    for (int i = 0; i < numCities; i++) {
        currBound += (firstMin(adj, i,numCities) + secondMin(adj, i,numCities));
    }

    currBound = (fmod(currBound, 2) == 1) ? currBound / 2.0 + 1 : currBound / 2.0;

    visited[startCityIndex] = 1;
    currPath[0] = startCityIndex;

    TSPRec(adj, currBound, 0, 1, currPath, visited,numCities);
    printf("Best route found:\n");
    for (int i = 0; i <= numCities; i++) {
        printf("%s", kota[finalPath[i]]);
        if (i < numCities) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.4f km\n", finalRes);
}
