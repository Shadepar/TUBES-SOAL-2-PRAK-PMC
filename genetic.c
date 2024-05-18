#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "genetic.h"

#define MAX_CITIES 100
#define POPULATION_SIZE 50
#define MUTATION_RATE 0.01
#define NUM_GENERATIONS 1000

// Structure to represent a chromosome (route)
struct Chromosome {
    int genes[MAX_CITIES];
    double fitness;
};

// Function to initialize a population of chromosomes randomly
void initializePopulation(struct Chromosome *population, int numCities, int startCityIndex) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        for (int j = 0; j < numCities; j++) {
            population[i].genes[j] = j;
        }
        // Shuffle the genes randomly except for the start city
        for (int j = numCities - 1; j > 0; j--) {
            int k = rand() % (j + 1);
            int temp = population[i].genes[j];
            population[i].genes[j] = population[i].genes[k];
            population[i].genes[k] = temp;
        }
        // Make sure the start city is at the beginning of the chromosome
        for (int j = 0; j < numCities; j++) {
            if (population[i].genes[j] == startCityIndex) {
                int temp = population[i].genes[0];
                population[i].genes[0] = startCityIndex;
                population[i].genes[j] = temp;
                break;
            }
        }
    }
}

// Function to calculate the fitness of each chromosome (route)
void calculateFitness(struct Chromosome *population, double **jarak, int numCities) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        double totalDistance = 0;
        for (int j = 0; j < numCities - 1; j++) {
            totalDistance += jarak[population[i].genes[j]][population[i].genes[j + 1]];
        }
        totalDistance += jarak[population[i].genes[numCities - 1]][population[i].genes[0]];
        population[i].fitness = totalDistance;
    }
}

// Function to perform tournament selection to choose parents for crossover
void tournamentSelection(struct Chromosome *population, struct Chromosome *parent1, struct Chromosome *parent2) {
    *parent1 = population[rand() % POPULATION_SIZE];
    *parent2 = population[rand() % POPULATION_SIZE];
    if (parent1->fitness < parent2->fitness) {
        *parent2 = *parent1;
    } else {
        *parent1 = *parent2;
    }
}

// Function to perform partially-mapped crossover (PMX)
void crossover(struct Chromosome parent1, struct Chromosome parent2, struct Chromosome *child1, struct Chromosome *child2, int numCities) {
    int crossoverPoint1 = rand() % numCities;
    int crossoverPoint2 = rand() % numCities;
    if (crossoverPoint1 > crossoverPoint2) {
        // Make sure crossoverPoint1 is less than crossoverPoint2
        int temp = crossoverPoint1;
        crossoverPoint1 = crossoverPoint2;
        crossoverPoint2 = temp;
    }
    // Initialize child chromosomes
    *child1 = parent1;
    *child2 = parent2;

    // Create mapping tables to track gene swaps
    int mapping1[numCities], mapping2[numCities];
    memset(mapping1, -1, sizeof(mapping1));
    memset(mapping2, -1, sizeof(mapping2));

    // Perform crossover within the crossover points
    for (int i = crossoverPoint1; i <= crossoverPoint2; i++) {
        int gene1 = parent1.genes[i];
        int gene2 = parent2.genes[i];
        child1->genes[i] = gene2;
        child2->genes[i] = gene1;
        mapping1[gene1] = gene2;
        mapping2[gene2] = gene1;
    }

    // Fix duplicate genes in child1
    for (int i = 0; i < numCities; i++) {
        if (i < crossoverPoint1 || i > crossoverPoint2) {
            int gene = parent1.genes[i];
            while (mapping1[gene] != -1) {
                gene = mapping1[gene];
            }
            child1->genes[i] = gene;
            mapping1[gene] = gene;
        }
    }

    // Fix duplicate genes in child2
    for (int i = 0; i < numCities; i++) {
        if (i < crossoverPoint1 || i > crossoverPoint2) {
            int gene = parent2.genes[i];
            while (mapping2[gene] != -1) {
                gene = mapping2[gene];
            }
            child2->genes[i] = gene;
            mapping2[gene] = gene;
        }
    }

    // Ensure the start city is at the beginning of the chromosome
    for (int i = 0; i < numCities; i++) {
        if (child1->genes[i] == parent1.genes[0]) {
            int temp = child1->genes[0];
            child1->genes[0] = child1->genes[i];
            child1->genes[i] = temp;
            break;
        }
    }
    for (int i = 0; i < numCities; i++) {
        if (child2->genes[i] == parent2.genes[0]) {
            int temp = child2->genes[0];
            child2->genes[0] = child2->genes[i];
            child2->genes[i] = temp;
            break;
        }
    }
}

// Function to perform mutation (swap two genes randomly)
void mutate(struct Chromosome *child, int numCities) {
    if ((double)rand() / RAND_MAX < MUTATION_RATE) {
        int index1 = rand() % numCities;
        int index2 = rand() % numCities;
        if (index1 == 0) index1++; // Skip the first gene (start city)
        if (index2 == 0) index2++; // Skip the first gene (start city)
        int temp = child->genes[index1];
        child->genes[index1] = child->genes[index2];
        child->genes[index2] = temp;
    }
}

// Function to perform elitism (keep the best chromosome from the previous generation)
void elitism(struct Chromosome *population, struct Chromosome *newPopulation, int numCities) {
    int bestIndex = 0;
    for (int i = 1; i < POPULATION_SIZE; i++) {
        if (population[i].fitness < population[bestIndex].fitness) {
            bestIndex = i;
        }
    }
    newPopulation[0] = population[bestIndex];
}

// Function to evolve the population using genetic algorithm
void evolve(struct Chromosome *population, struct Chromosome *newPopulation, int numCities) {
    elitism(population, newPopulation, numCities);
    for (int i = 1; i < POPULATION_SIZE; i++) {
        struct Chromosome parent1, parent2, child1, child2;
        tournamentSelection(population, &parent1, &parent2);
        crossover(parent1, parent2, &child1, &child2, numCities);
        mutate(&child1, numCities);
        mutate(&child2, numCities);
        newPopulation[i] = child1;
        newPopulation[++i] = child2;
    }
}


void genetic(int startCityIndex,int numCities,double **jarak,char **kota) {
    struct Chromosome template;
    struct Chromosome *population = malloc(POPULATION_SIZE * sizeof(template));
    struct Chromosome *newPopulation = malloc(POPULATION_SIZE * sizeof(template));
    if (!population || !newPopulation) {
        printf("Memory allocation failed.\n");
        return;
    }
    
    initializePopulation(population, numCities, startCityIndex); // Pass startCityIndex
    calculateFitness(population, jarak, numCities);
    for (int generation = 0; generation < NUM_GENERATIONS; generation++) {
        evolve(population, newPopulation, numCities);
        struct Chromosome *temp = population;
        population = newPopulation;
        newPopulation = temp;
        calculateFitness(population, jarak, numCities);
    }
    int bestIndex = 0;
    for (int i = 1; i < POPULATION_SIZE; i++) {
        if (population[i].fitness < population[bestIndex].fitness) {
            bestIndex = i;
        }
    }
    printf("Best route found:\n");
    for (int i = 0; i < numCities; i++) {
        printf("%s -> ", kota[population[bestIndex].genes[i]]);
    }
    printf("%s\n", kota[population[bestIndex].genes[0]]); // Back to starting city
    printf("Best route distance: %.2f km\n", population[bestIndex].fitness);
}