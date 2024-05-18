#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pso.h"

#define C1 3.0
#define C2 1.0
#define BA 2.5
#define BB 0.0
#define RHOMAX 0.9
#define RHOMIN 0.4

int validation(int city, int currentParticle, int latestIndex, int **pos){
    int valid = 1;
    for(int i=0; i<latestIndex; i++){
        if(city == pos[currentParticle][i]){
            valid = 0;
        }
    }
    return valid;
}

int randomPermutation(int currentParticle, int latestIndex, int **pos, int N, int startingCity){
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

int generateRoute(int currentParticle, double **sortedProb, double **prob, int *visited, int N, int **pos, int startingCity){
    int newRoute[N-1];
    int index = 0;
    for(int i = 0; i < N; i++){
        //index = 0;
        for(int j = 0; j < N; j++){
            if(sortedProb[currentParticle][i] == prob[currentParticle][j]){
                //printf("%lf %lf\n", sortedProb[currentParticle][i], prob[currentParticle][j]);
                if((visited[j] != 1) && (j != startingCity)){
                    newRoute[index] = j;
                    //printf("%d \n", pos[currentParticle][j]);
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

void printProb(int NP, int N, double **prob){
    for(int i = 0; i <NP; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", prob[i][j]);
        }
        printf("probability\n");
    }
}

void update_velocity(int iter, int NP, int NI, int N, double **vel, double **probPbest, double **prob, double *probGbest) {
    //srand(time(NULL));
    for(int i = 0; i < NP; i++){
        double inertia = RHOMAX - ((RHOMAX - RHOMIN) / NI) * iter;
        /*
        for(int j = 0; j < N; j++){
            printf("%lf ", prob[i][j]);
        }
        printf("probability\n");
        */
        for (int j = 0; j < N; j++) {
            double r1 = ((double)rand() / RAND_MAX) * (1.0);
            double r2 = ((double)rand() / RAND_MAX) * (1.0);
            vel[i][j] = inertia * vel[i][j] + C1 * r1 * (probPbest[i][j] - prob[i][j]) + C2 * r2 * (probGbest[j] - prob[i][j]);
            prob[i][j] = prob[i][j] + vel[i][j];
            //printf("%lf ", vel[i][j]);
            if(prob[i][j] > BA){
                prob[i][j] = BA;
            }
            else if(prob[i][j] < BB){
                prob[i][j] = BB;
            }
        }
        //printf("velocity\n");
        /*
        for(int j = 0; j < N; j++){
            printf("%lf ", prob[i][j]);
        }
        printf("probability\n");
        */
    }
}

void printSortedProb(int NP, int N, double **sortedProb){
    for(int i = 0; i <NP; i++){
        for(int j = 0; j < N; j++){
            printf("%lf ", sortedProb[i][j]);
        }
        printf("sortedProb\n");
    }
}

void copyArr(int currentParticle, int N, double **sortedProb, double **prob){
    for(int i = 0; i < N; i++){
        sortedProb[currentParticle][i] = prob[currentParticle][i];
    }
    //printSortedProb();
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



void update_position(int NP, int N, double **sortedProb, int *visited, int startingCity, double **prob, int **pos) {
    for(int i=0; i<NP; i++) {
        for(int j = 0; j < N; j++){
            if(j == startingCity){
                visited[j] = 1;
            }
            else{
                visited[j] = 0;
            }
        }
        copyArr(i, N, sortedProb, prob);
        qsort(sortedProb[i], N, sizeof(double), compare);
        generateRoute(i, sortedProb, prob, visited, N, pos, startingCity);
    }
    //printSortedProb();
}

double cost(int* tour, int N, double **dist) {
    double c = 0;
    for(int i=0; i<N; i++) {
        c += dist[tour[i]][tour[i+1]];
    }
    return c;
}

void update_gbest(int NP, int N, int **pbest, int *gbest, double **dist, double *probGbest, double **probPbest) {
    for(int i=0; i<NP; i++) {
        if(cost(pbest[i], N, dist) < cost(gbest, N, dist)) {
            for(int j=0; j<N+1; j++) {
                gbest[j] = pbest[i][j];
            }
            for(int j=0; j<N; j++) {
                probGbest[j] = probPbest[i][j];
            }
        }
    }
}

void update_pbest(int NP, int N, int **pbest, double **prob, int **pos, double **probPbest, double **dist) {
    for(int i=0; i<NP; i++) {
        if(cost(pos[i], N, dist) < cost(pbest[i], N, dist)) {
            for(int j=0; j<N+1; j++) {
                pbest[i][j] = pos[i][j];
            }
            for(int j=0; j<N; j++) {
                probPbest[i][j] = prob[i][j];
            }
        }
    }
}


void printPbest(int NP, int N, int **pbest, double **dist){
    for(int i = 0; i <NP; i++){
        for(int j = 0; j < N+1; j++){
            printf("%d ", pbest[i][j]);
        }
        printf("pbest %lf\n", cost(pbest[i], N, dist));
    }
}

void printGbest(int N, int *gbest, double **dist){
    for(int i = 0; i <N+1; i++){
        printf("%d ", gbest[i]);
    }
    printf("gbest %lf\n", cost(gbest, N, dist));
}

void initialize(int N, int NP, double **dist, int *visited, double **prob, double **probPbest, double *probGbest, double **vel, int *gbest, int **pbest, int startingCity, int **pos) {
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
            pos[i][j] = randomPermutation(i, j, pos, N, startingCity);
            pbest[i][j] = pos[i][j];
        }
    }
    for(int i = 0; i <NP; i ++){
        for(int j = 0; j < N; j++){
            prob[i][j] = ((double)rand() / RAND_MAX) * (BA);
            probPbest[i][j] = prob[i][j];
            vel[i][j] = ((double)rand() / RAND_MAX) * (BA);
        }
    }
    for(int i=0; i<N+1; i++) {
        gbest[i] = pos[0][i];
    }
    for(int i = 0; i < N; i++){
        probGbest[i] = probPbest[0][i];
    }
    gbest[N] = gbest[0];
    //printPbest();
    update_gbest(NP, N, pbest, gbest, dist, probGbest, probPbest);
    printGbest(N, gbest, dist);
}

void printPos(int NP, int N, int **pos){
    for(int i = 0; i <NP; i++){
        for(int j = 0; j < N+1; j++){
            printf("%d ", pos[i][j]);
        }
        printf("pos\n");
    }
}

void PSO(int start,int N, double **jarak,char **kota){
    int startingCity = start;
    double **dist = jarak;
    int NP = N * 20;
    int NI = N * 20 + 100;
    int* visited = malloc(N * sizeof(visited));
    int* gBest = malloc((N+1) * sizeof(gBest));

    int** pBest = malloc((NP) * sizeof(*pBest));
    for (int i=0;i<NP;i++){
        pBest[i] = malloc((N+1) * sizeof(*pBest[i]));
    }

    int** pos = malloc((NP) * sizeof(*pos));
    for (int i=0;i<NP;i++){
        pos[i] = malloc((N+1) * sizeof(*pos[i]));
    }

    double** vel = malloc(NP * sizeof(*vel));
    for (int i=0;i<NP;i++){
        vel[i] = malloc(N * sizeof(*vel[i]));
    }

    double** x = malloc(NP * sizeof(*x));
    for (int i=0;i<NP;i++){
        x[i] = malloc(N * sizeof(*x[i]));
    }

    double** xPbest = malloc(NP * sizeof(*xPbest));
    for (int i=0;i<NP;i++){
        xPbest[i] = malloc(N * sizeof(*xPbest[i]));
    }

    double* xGbest = malloc(N * sizeof(xGbest));

    double** sortedX = malloc(NP * sizeof(*sortedX));
    for (int i=0;i<NP;i++){
        sortedX[i] = malloc(N * sizeof(*sortedX[i]));
    }

    initialize(N, NP, dist, visited, x, xPbest, xGbest, vel, gBest, pBest, startingCity, pos);
    for(int i=0; i<NI; i++){
        //printPos();
        update_velocity(i, NP, NI, N, vel, xPbest, x, xGbest);
        update_position(NP, N, sortedX, visited, startingCity, x, pos);
        //printPos();
        update_pbest(NP, N, pBest, x, pos, xPbest, dist);
        //printPbest();
        update_gbest(NP, N, pBest, gBest, dist, xGbest, xPbest);
        printGbest(N, gBest, dist);
    }
    printf("Best route found:\n%s",kota[start]);
    for (int i = 1; i<N+1; i++)
    {
        printf(" -> %s",kota[gBest[i]]);
    }
    printf("\n");
    printf("Best route distance:%f\n",cost(gBest, N, dist));
}
