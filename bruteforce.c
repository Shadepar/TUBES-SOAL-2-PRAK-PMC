#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include "bruteforce.h"

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void bruteForce(int start, int len, double **jarak, char **kota) {
    if (len < 6 || len > 15){
        printf("Cities cannot be less than 6 and more than 15\n");
        return;
    }
    int *rute = malloc(len * sizeof(rute)); // Array untuk menyimpan rute
    int *shortestPath = malloc(len * sizeof(shortestPath)); // Array untuk menyimpan rute terdekat
    double shortestDistance = DBL_MAX; // Jarak rute terdekat
    int j;
    
    // Inisialisasi rute awal
    for (j = 0; j < len; j++)
        rute[j] = j;
    
    // Tukar startingpoint dengan posisi awal kota
    swap(&rute[0], &rute[start]);

    // Memanggil Fungsi rekursif untuk menghitung jarak
    permute(start, 1, len, jarak, kota, rute, &shortestDistance, shortestPath); // Start from 1 to len - 1, as 0 is fixed for start city

    // Menambah starting point di akhir rute terpendek
    shortestPath[len] = start;

    printf("Best route found:\n");
    for (j = 0; j < len; j++)
        printf("%s -> ", kota[shortestPath[j]]);
    printf("%s\n", kota[start]); // Berakhir pada starting point

    printf("Best route distance : %.2f km\n", shortestDistance);

    // Free memory
    free(rute);
    free(shortestPath);
}


void permute(int start, int i, int len, double **jarak, char **kota, int *rute, double *shortestDistance, int *shortestPath) {
    int j;
    if (i == len) {
        double distance = 0;
        for (j = 0; j < len; j++) {
            distance += jarak[rute[j]][rute[(j + 1) % len]]; //Menghitung jarak untuk suatu rute
        }
        if (distance < *shortestDistance) {
            *shortestDistance = distance;
            for (j = 0; j < len; j++)
                shortestPath[j] = rute[j];
        }
    } else {
        for (j = i; j < len; j++) {
            swap(&rute[i], &rute[j]);
            permute(start, i+1, len, jarak, kota, rute, shortestDistance, shortestPath);
            swap(&rute[i], &rute[j]);
        }
    }
}