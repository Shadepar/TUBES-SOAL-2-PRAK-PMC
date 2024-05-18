#include "AntColonyOptimization.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 15
#define NUM_ANTS 10
#define MAX_ITERATIONS 100
#define ALPHA 1.0  // Pengaruh jejak feromon
#define BETA 5.0   // Pengaruh jarak
#define EVAPORATION_RATE 0.5
#define Q 100.0    // Faktor konstan untuk pembaruan feromon
#define EARTH_RADIUS 6371.0  // Radius bumi dalam kilometer
#define M_PI 3.14159265358979323846  // Definisi konstanta pi

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

typedef struct {
    int path[MAX_CITIES];
    double tour_length;
    int visited[MAX_CITIES];
} Ant;


City cities[MAX_CITIES];
double pheromones[MAX_CITIES][MAX_CITIES];
Ant ants[NUM_ANTS];

void initialize_pheromones(int num_cities) {
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            pheromones[i][j] = 1.0;  // Inisialisasi feromon awal
        }
    }
}

void initialize_ants(int num_cities, int start_city) {
    for (int k = 0; k < NUM_ANTS; k++) {
        for (int i = 0; i < num_cities; i++) {
            ants[k].path[i] = -1;  // Inisialisasi jalur semut
            ants[k].visited[i] = 0;  // Inisialisasi kota yang dikunjungi
        }
        ants[k].path[0] = start_city;  // Set kota awal
        ants[k].visited[start_city] = 1;
        ants[k].tour_length = 0.0;
    }
}

double calculate_probability(int from, int to, int ant_index, int num_cities, double** distances) {
    double pheromone = pow(pheromones[from][to], ALPHA);
    double distance = pow(1.0 / distances[from][to], BETA);
    double total = 0.0;

    for (int i = 0; i < num_cities; i++) {
        if (!ants[ant_index].visited[i] && i != from) {
            total += pow(pheromones[from][i], ALPHA) * pow(1.0 / distances[from][i], BETA);
        }
    }

    return (pheromone * distance) / total;
}

int select_next_city(int from, int ant_index, int num_cities, double** distances) {
    double probabilities[MAX_CITIES];
    double sum = 0.0;

    for (int i = 0; i < num_cities; i++) {
        if (!ants[ant_index].visited[i] && i != from) {
            probabilities[i] = calculate_probability(from, i, ant_index, num_cities, distances);
            sum += probabilities[i];
        } else {
            probabilities[i] = 0.0;
        }
    }

    double r = ((double)rand() / (RAND_MAX)) * sum;
    double partial_sum = 0.0;

    for (int i = 0; i < num_cities; i++) {
        if (probabilities[i] > 0.0) {
            partial_sum += probabilities[i];
            if (partial_sum >= r) {
                return i;
            }
        }
    }
    return -1;
}

void update_pheromones(int num_cities, double** distances) {
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            pheromones[i][j] *= (1.0 - EVAPORATION_RATE);  // Evaporasi feromon
        }
    }

    for (int k = 0; k < NUM_ANTS; k++) {
        for (int i = 0; i < num_cities - 1; i++) {
            int from = ants[k].path[i];
            int to = ants[k].path[i + 1];
            pheromones[from][to] += Q / ants[k].tour_length;
            pheromones[to][from] += Q / ants[k].tour_length;
        }
        // Update pheromone for returning to start city
        int last_city = ants[k].path[num_cities - 1];
        int start_city = ants[k].path[0];
        pheromones[last_city][start_city] += Q / ants[k].tour_length;
        pheromones[start_city][last_city] += Q / ants[k].tour_length;
    }
}

void AntColonyOptimization(int start, int len, double **jarak, char **kota) {
    srand(time(NULL));

    initialize_pheromones(len);

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        initialize_ants(len, start); // Reinitialize ants each iteration

        for (int k = 0; k < NUM_ANTS; k++) {
            for (int i = 1; i < len; i++) {
                int from = ants[k].path[i - 1];
                int next_city = select_next_city(from, k, len, jarak);
                if (next_city != -1) {
                    ants[k].path[i] = next_city;
                    ants[k].visited[next_city] = 1;
                    ants[k].tour_length += jarak[from][next_city];
                }
            }
            // Return to start city
            ants[k].tour_length += jarak[ants[k].path[len - 1]][start];
        }

        update_pheromones(len, jarak);
    }

    double best_tour_length = ants[0].tour_length;
    int best_ant_index = 0;

    for (int k = 1; k < NUM_ANTS; k++) {
        if (ants[k].tour_length < best_tour_length) {
            best_tour_length = ants[k].tour_length;
            best_ant_index = k;
        }
    }

    printf("Best route found:\n");
    for (int i = 0; i < len; i++) {
        printf("%s -> ", kota[ants[best_ant_index].path[i]]);
    }
    printf("%s\n", kota[start]);  // Return to start city
    printf("Best route distance: %lf km\n", best_tour_length);
}
