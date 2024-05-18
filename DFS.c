#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "DFS.h"

void DFS(int startingPoint, int NUM_CITIES, double **distances, char **cities) {
    // Fungsi swap
    void swap(int *x, int *y) {
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
    void permute(int *array, int start, int end, int NUM_CITIES, double **distances, int *shortestRoute, double *minDistance, double currentDistance) {
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
                swap((array + start), (array + i));
                double newDistance = currentDistance;
                if (start > 0) {
                    newDistance += distances[array[start-1]][array[start]];
                }
                permute(array, start + 1, end, NUM_CITIES, distances, shortestRoute, minDistance, newDistance);
                swap((array + start), (array + i)); // backtrack
            }
        }
    }

    // Implementasi DFS
    int route[NUM_CITIES], shortestRoute[NUM_CITIES];
    for (int i = 0; i < NUM_CITIES; i++) {
        route[i] = i;
    }
    // inisialisasi array
    if (startingPoint != 0) {
        swap(&route[0], &route[startingPoint]);
    }

    double minDistance = DBL_MAX;
    permute(route, 1, NUM_CITIES, NUM_CITIES, distances, shortestRoute, &minDistance, 0.0);




    printf("Best route found: \n");
    for (int i = 0; i < NUM_CITIES; i++) {

        printf("%s -> ", cities[shortestRoute[i]]);
    }

    printf("%s", cities[shortestRoute[0]]);
    printf("\nBest route distance: %lf\n", minDistance);
}

