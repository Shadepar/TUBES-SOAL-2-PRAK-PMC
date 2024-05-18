#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 15
#define MIN_CITIES 4
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
double distances[MAX_CITIES][MAX_CITIES];
double pheromones[MAX_CITIES][MAX_CITIES];
Ant ants[NUM_ANTS];

void calculate_distance_between_two_cities(int i, int j) {
    if (i == j) {
        distances[i][j] = 0.0;
    } else {
        double lat1 = cities[i].latitude * M_PI / 180.0;
        double lon1 = cities[i].longitude * M_PI / 180.0;
        double lat2 = cities[j].latitude * M_PI / 180.0;
        double lon2 = cities[j].longitude * M_PI / 180.0;

        double lat_diff = lat2 - lat1;
        double lon_diff = lon2 - lon1;
        double a = sin(lat_diff / 2) * sin(lat_diff / 2) +
                   cos(lat1) * cos(lat2) *
                   sin(lon_diff / 2) * sin(lon_diff / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        distances[i][j] = EARTH_RADIUS * c;
    }
}

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

double calculate_probability(int from, int to, int ant_index, int num_cities) {
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

int select_next_city(int from, int ant_index, int num_cities) {
    double probabilities[MAX_CITIES];
    double sum = 0.0;

    for (int i = 0; i < num_cities; i++) {
        if (!ants[ant_index].visited[i] && i != from) {
            probabilities[i] = calculate_probability(from, i, ant_index, num_cities);
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

void update_pheromones(int num_cities) {
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

void solve_tsp(int num_cities, int start_city) {
    initialize_pheromones(num_cities);

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        initialize_ants(num_cities, start_city); // Reinitialize ants each iteration

        for (int k = 0; k < NUM_ANTS; k++) {
            for (int i = 1; i < num_cities; i++) {
                int from = ants[k].path[i - 1];
                int next_city = select_next_city(from, k, num_cities);
                if (next_city != -1) {
                    ants[k].path[i] = next_city;
                    ants[k].visited[next_city] = 1;
                    ants[k].tour_length += distances[from][next_city];
                }
            }
            // Return to start city
            ants[k].tour_length += distances[ants[k].path[num_cities - 1]][start_city];
        }

        update_pheromones(num_cities);
    }
}

void print_solution(int num_cities, int start_city) {
    double best_tour_length = ants[0].tour_length;
    int best_ant_index = 0;

    for (int k = 1; k < NUM_ANTS; k++) {
        if (ants[k].tour_length < best_tour_length) {
            best_tour_length = ants[k].tour_length;
            best_ant_index = k;
        }
    }

    printf("Best route found:\n");
    for (int i = 0; i < num_cities; i++) {
        printf("%s -> ", cities[ants[best_ant_index].path[i]].name);
    }
    printf("%s\n", cities[start_city].name);  // Return to start city
    printf("Best route distance: %lf km\n", best_tour_length);
}

void printMatrix(int rows, int cols, double matrix[rows][cols]) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));

    int num_cities;
    printf("Enter number of cities (4-15): ");
    scanf("%d", &num_cities);

    if (num_cities < MIN_CITIES || num_cities > MAX_CITIES) {
        printf("Number of cities must be between 4 and 15.\n");
        return 1;
    }

    for (int i = 0; i < num_cities; i++) {
        printf("Enter name, latitude, and longitude of city %d: ", i + 1);
        scanf("%s %lf %lf", cities[i].name, &cities[i].latitude, &cities[i].longitude);
    }

    // Hitung jarak antara semua kota setelah input selesai
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            calculate_distance_between_two_cities(i, j);
        }
    }

    int start_city;
    printf("Enter the index of the start city (0-%d): ", num_cities - 1);
    scanf("%d", &start_city);

    printf("\n");
    printMatrix(15,15,distances);
    printf("\n");

    clock_t start_time = clock();
    solve_tsp(num_cities, start_city);
    clock_t end_time = clock();
    double time_elapsed = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    print_solution(num_cities, start_city);
    printf("Time elapsed: %lf s\n", time_elapsed);

    return 0;
}
