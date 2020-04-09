#ifndef ALG_GEN_REAL_H
#define ALG_GEN_REAL_H

//declaraçõa do tipo que representa um indivíduo 
typedef struct individual {
	double *parameters;
	double value;
	double fitness;
	double acumulated_fitness;
}typ_individual;

//declaração do tipo que representa um população de 30 indivíduos
typedef struct population {
	int generation;
	int parameters_num;
	typ_individual individuals[30];
}typ_population;

double goal_function(int x, int y);
void mergesort(typ_population *pop, int initialposition , int lastposition);
void fitness(typ_population *pop);
double *roulette_wheel(typ_population *pop, int sum_fitness);
double* average_crossover(double *parent1, double *parent2, int parameters_num);
double* geometric_average_crossover(double *parent1, double *parent2, int parameters_num);
double* BLX_crossover(double *parent1, double *parent2, int parameters_num);
double* linear_crossover(double *parent1, double *parent2, int parameters_num);
double** aritmetic_crossover(double *parent1, double *parent2, int parameters_num);
double* heuristic_crossover(typ_population *pop, int parameters_num);
void uniform_mutation(typ_individual *ind, int parameters_num);
void limit_mutation(typ_individual *ind, int parameters_num);
void irregular_mutation(typ_individual *ind, int parameters_num, int generation, int max_generation);
void multiple_irregular_mutation(typ_individual *ind, int parameters_num, int generation, int max_generation);
void create_new_individuals_aritmetic(typ_population *pop, typ_individual *ind1, typ_individual *ind2, int sum_fitness, int max_generation);
typ_population* create_new_population_aritmetic(typ_population *pop, int max_generation);
void create_new_individuals(typ_population *pop, typ_individual *ind, int sum_fitness, int max_generation);
typ_population* create_new_population(typ_population *pop, int max_generation);
typ_population* initial_population();

#endif