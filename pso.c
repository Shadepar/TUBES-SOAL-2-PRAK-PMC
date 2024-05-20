#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "pso.h"

#define C1 3.0
#define C2 1.0
#define BA 2.5
#define RHOMAX 0.6
#define RHOMIN 0.4

int validation(int city, int currentParticle, int latestIndex, int **pos){
    int valid = 1;
    for(int i=0; i<latestIndex; i++){
        if(city == pos[currentParticle][i]){
            valid = 0;
            break;
        }
    }
    return valid;
}

int permutation(int currentParticle, int latestIndex, int **pos, int N, int startingCity){
    int valid = 0;
    int city = 0;
    if(latestIndex != 0 && latestIndex != N){
        while(!valid){
            city = rand() % (N);
            valid = validation(city, currentParticle, latestIndex, pos);
        }
        return city;
    }
    else{
        return startingCity;
    } 
}

int generateRoute(int currentParticle, double **sortedX, double **x, int *visited, int N, int **pos, int startingCity){
    int newRoute[N-1];
    int index = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(sortedX[currentParticle][i] == x[currentParticle][j]){
                if((visited[j] != 1) && (j != startingCity)){
                    newRoute[index] = j;
                    visited[j] = 1;
                    index += 1;
                    break; 
                }
            }
        }
    }
    for(int i = 1; i < N; i++){
        pos[currentParticle][i] = newRoute[i-1];
    }
}

void updateVelocity(int iter, int NP, int NI, int N, double **vel, double **xPbest, double **x, double *xGbest) {
    double inertia = RHOMAX - ((RHOMAX - RHOMIN) / NI) * iter;
    double r1 = 0, r2 = 0;
    for(int i = 0; i < NP; i++){
        for (int j = 0; j < N; j++) {
            r1 = ((double)rand() / RAND_MAX) * (1.0);
            r2 = ((double)rand() / RAND_MAX) * (1.0);
            vel[i][j] = inertia * vel[i][j] + C1 * r1 * (xPbest[i][j] - x[i][j]) + C2 * r2 * (xGbest[j] - x[i][j]);
            if(vel[i][j] > N/4){
                vel[i][j] = N/4;
            }
            else if(vel[i][j] < -(N/4)){
                vel[i][j] = -(N/4);            }
            x[i][j] = x[i][j] + vel[i][j];
            if(x[i][j] > N){
                x[i][j] = N;
            }
            else if(x[i][j] < 0){
                x[i][j] = 0;
            }
        }
    }
}

void copyArr(int currentParticle, int N, double **sortedX, double **x){
    for(int i = 0; i < N; i++){
        sortedX[currentParticle][i] = x[currentParticle][i];
    }
}

int compare(const void *a, const void *b) {
    const double *double_a = (const double *)a;
    const double *double_b = (const double *)b;
    
    if (*double_a < *double_b) {
        return -1;
    } else if (*double_a > *double_b) {
        return 1;
    } else {
        return 0;
    }
}

void updatePosition(int NP, int N, double **sortedX, int *visited, int startingCity, double **x, int **pos) {
    for(int i=0; i<NP; i++) {
        for(int j = 0; j < N; j++){
            if(j == startingCity){
                visited[j] = 1;
            }
            else{
                visited[j] = 0;
            }
        }
        copyArr(i, N, sortedX, x);
        qsort(sortedX[i], N, sizeof(double), compare);
        generateRoute(i, sortedX, x, visited, N, pos, startingCity);
    }
}

double cost(int* tour, int N, double **dist) {
    double c = 0;
    for(int i=0; i<N; i++) {
        c += dist[tour[i]][tour[i+1]];
    }
    return c;
}

void updateGbest(int NP, int N, int **pbest, int *gbest, double **dist, double *xGbest, double **xPbest) {
    for(int i=0; i<NP; i++) {
        if(cost(pbest[i], N, dist) < cost(gbest, N, dist)) {
            for(int j=0; j<N+1; j++) {
                gbest[j] = pbest[i][j];
                if(j < N){
                    xGbest[j] = xPbest[i][j];
                }
            }
        }
    }
}

void updatePbest(int NP, int N, int **pbest, double **x, int **pos, double **xPbest, double **dist) {
    for(int i=0; i<NP; i++) {
        if(cost(pos[i], N, dist) < cost(pbest[i], N, dist)) {
            for(int j=0; j<N+1; j++) {
                pbest[i][j] = pos[i][j];
                if(j < N){
                    xPbest[i][j] = x[i][j];
                }
            }
        }
    }
}

void initialize(int N, int NP, double **dist, int *visited, double **x, double **xPbest, double *xGbest, double **vel, int *gbest, int **pbest, int startingCity, int **pos) {
    srand(time(NULL));
    for(int i = 0; i < N; i++){
        if(i == startingCity){
            visited[i] = 1;
        }
        else{
            visited[i] = 0;
        }
    }
    for(int i=0; i<NP; i++) {
        for(int j=0; j<N+1; j++) {
            pos[i][j] = permutation(i, j, pos, N, startingCity);
            pbest[i][j] = pos[i][j];
            if(j<N){
                x[i][j] = ((double)rand() / RAND_MAX) * (BA);
                xPbest[i][j] = x[i][j];
                vel[i][j] = ((double)rand() / RAND_MAX) * (BA);
            }
        }
    }
    for(int i=0; i<N+1; i++) {
        gbest[i] = pos[0][i];
        if(i < N){
            xGbest[i] = xPbest[0][i];
        }
    }
    updateGbest(NP, N, pbest, gbest, dist, xGbest, xPbest);
}

void bersihkanMemori(int N, int NP, double **dist, int *visited, double **x, double **xPbest, double *xGbest, double **vel, int *gBest, int **pBest, int **pos, double** sortedX){
    for (int i=0;i<NP;i++){
        free(pBest[i]);
        free(pos[i]);
        free(vel[i]);
        free(x[i]);
        free(xPbest[i]);
        free(sortedX[i]);
    }
    free(visited);
    free(gBest);
    free(xGbest);
    free(pBest);
    free(pos);
    free(vel);
    free(x);
    free(xPbest);
    free(sortedX);
}

void PSO(int start,int N, double **jarak,char **kota){
    int startingCity = start;
    double **dist = jarak;
    int NP = N * 20;
    int NI = N * 20 + 100;
    int* visited = malloc(N * sizeof(visited));
    int* gBest = malloc((N+1) * sizeof(gBest));
    int** pBest = malloc((NP) * sizeof(*pBest));
    int** pos = malloc((NP) * sizeof(*pos));
    double** vel = malloc(NP * sizeof(*vel));
    double** x = malloc(NP * sizeof(*x));
    double** xPbest = malloc(NP * sizeof(*xPbest));
    double* xGbest = malloc(N * sizeof(xGbest));
    double** sortedX = malloc(NP * sizeof(*sortedX));

    for (int i=0;i<NP;i++){
        pBest[i] = malloc((N+1) * sizeof(*pBest[i]));
        pos[i] = malloc((N+1) * sizeof(*pos[i]));
        vel[i] = malloc(N * sizeof(*vel[i]));
        x[i] = malloc(N * sizeof(*x[i]));
        xPbest[i] = malloc(N * sizeof(*xPbest[i]));
        sortedX[i] = malloc(N * sizeof(*sortedX[i]));
    }

    initialize(N, NP, dist, visited, x, xPbest, xGbest, vel, gBest, pBest, startingCity, pos);
    for(int i=0; i<NI; i++){
        updateVelocity(i, NP, NI, N, vel, xPbest, x, xGbest);
        updatePosition(NP, N, sortedX, visited, startingCity, x, pos);
        updatePbest(NP, N, pBest, x, pos, xPbest, dist);
        updateGbest(NP, N, pBest, gBest, dist, xGbest, xPbest);
    }
    printf("Best route found:\n%s",kota[start]);
    for (int i = 1; i<N+1; i++)
    {
        printf(" -> %s",kota[gBest[i]]);
    }
    printf("\n");
    printf("Best route distance: %f km\n",cost(gBest, N, dist));
    bersihkanMemori(N, NP, dist, visited, x, xPbest, xGbest, vel, gBest, pBest, pos, sortedX);
}
