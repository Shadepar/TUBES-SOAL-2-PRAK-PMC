#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "DFS.h"

// Fungsi swap
void swapd(int *x, int *y) {
        int temp = *x;
        *x = *y;
        *y = temp;
    }

// Fungsi copyArray
    void copyArray(int *source, int *destination, int length) {
        for (int i = 0; i < length; i++) {
            destination[i] = source[i];
        }
    }

// Fungsi permute dengan teknik prunning dan backtrack DFS
    void permuted(int *array, int start, int end, int NUM_CITIES, double **distances, int *shortestRoute, double *minDistance, double currentDistance) {
        if (currentDistance > *minDistance) { // Pruning step
            return;
        }
        if (start == end) {
            double totalDistance = currentDistance + distances[array[end-1]][array[0]];
            if (totalDistance < *minDistance) {
                *minDistance = totalDistance;
                copyArray(array, shortestRoute, NUM_CITIES);
            }
        } else {
            for (int i = start; i < end; i++) {
                swapd((array + start), (array + i));
                double newDistance = currentDistance;
                if (start > 0) {
                    newDistance += distances[array[start-1]][array[start]];
                }
                permuted(array, start + 1, end, NUM_CITIES, distances, shortestRoute, minDistance, newDistance);
                swapd((array + start), (array + i)); // backtrack
            }
        }
    }

void DFS(int startingPoint, int NUM_CITIES, double **distances, char **cities) {
    // Implementasi DFS
    int route[NUM_CITIES], shortestRoute[NUM_CITIES];
    for (int i = 0; i < NUM_CITIES; i++) {
        route[i] = i;
    }
    // inisialisasi array
    if (startingPoint != 0) {
        swapd(&route[0], &route[startingPoint]);
    }

    double minDistance = DBL_MAX;
    permuted(route, 1, NUM_CITIES, NUM_CITIES, distances, shortestRoute, &minDistance, 0.0);

    printf("Best route found: \n");
    for (int i = 0; i < NUM_CITIES; i++) {

        printf("%s -> ", cities[shortestRoute[i]]);
    }

    printf("%s", cities[shortestRoute[0]]);
    printf("\nBest route distance: %lf km\n", minDistance);
}
