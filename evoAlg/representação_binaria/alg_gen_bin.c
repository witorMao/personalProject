#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define PI 3.1415926535


typedef struct {
	char chromosome[23];
	float x;
	float value;
	float fitness;
	float acumulated_fitness;
}typ_individual;

typedef struct {
	typ_individual individuals[30];
}typ_population;

//algoritmo mergesort utilizado para a ordenadção da população pelo valor de f de x
void mergesort(typ_population *pop, int initialposition , int lastposition) {
	if (lastposition <= initialposition){ 
		return;
	}
	int center = (int) ((lastposition+initialposition)/2.0);
	mergesort(pop, initialposition, center);
	mergesort(pop, center+1, lastposition);

	typ_individual* aux = (typ_individual*) malloc(sizeof(typ_individual) * (lastposition-initialposition+1) );
	
	int i = initialposition;
	int j = center+1;
 	int k = 0;
 	while (i <= center && j <= lastposition) {
		if (pop->individuals[i].value > pop->individuals[j].value) {
			strcpy(aux[k].chromosome, pop->individuals[i].chromosome);
			aux[k].value = pop->individuals[i].value;
			aux[k].x = pop->individuals[i].x; 
			i++;
		} 
		else {
			strcpy(aux[k].chromosome, pop->individuals[j].chromosome);
			aux[k].value = pop->individuals[j].value;
			aux[k].x = pop->individuals[j].x;
			j++;
		}
		k++;
	}

	while (i <= center) {
		strcpy(aux[k].chromosome, pop->individuals[i].chromosome);
		aux[k].value = pop->individuals[i].value;
		aux[k].x = pop->individuals[i].x; 
		i++; 
		k++;
	}	

	while (j <= lastposition) {
		strcpy(aux[k].chromosome, pop->individuals[j].chromosome);
		aux[k].value = pop->individuals[j].value;
		aux[k].x = pop->individuals[j].x; 
		j++; 
		k++;
	}	

	for (i = initialposition, k = 0; i <= lastposition; i++, k++) {
		strcpy(pop->individuals[i].chromosome, aux[k].chromosome);
		pop->individuals[i].value = aux[k].value;
		pop->individuals[i].x = aux[k].x;
	}

	free(aux); 
}

//calcula a aptidão de cada individuo, juntamente com a aptidão acumulada
void fitness(typ_population *pop){
	for (int i = 0; i < 30; ++i){
		pop->individuals[i].fitness = 2 * (30 - (i + 1))/(float)(30 - 1);
		for (int j = 0; j <= i; ++j){
			pop->individuals[i].acumulated_fitness += pop->individuals[j].fitness; 
		}
	}
	return;
}

//função objetivo
float goal_function(float x){
	return (x * sin(10 * PI * x) + 1);
}

//converte cromossomo binario pra base decimal
int convert_to_decimal(char *chromosome, int first, int last){
	int aux = 0;
	int value = 0;
	for (int i = last; i >= first; --i){
		if (chromosome[i] - 48)	value += (int)pow(2, aux);
		aux++;
	}
	return value;
}

void convert_decimal_to_binary(int num, char *string){
	int i = 21;
	string[22] = 0;
	while (i >= 0){ 
        if(num != 0){
        	(num % 2) ? (string[i--] = '1') : (string[i--] = '0'); 
        	num = num/2; 
		}else string[i--] = '0'; 
    } 
    return;
}

//coloca os valores de x e y dentro do intervalo
void to_interval(typ_individual *ind){
	int int_aux_x = convert_to_decimal(ind->chromosome, 0, 21);//encontrando valor inteiro de x
	int min = -1;
	int max = 2;
	ind->x = ((int_aux_x/(pow(2, 22) - 1)) * (max - min)) + min;
	ind->value = goal_function(ind->x);
	ind->acumulated_fitness = 0;
	return;
}

char *roulette_wheel(typ_population *pop, int sum_fitness){
	//gerando r entre 0 e sum_fitness
	float r = (float)(rand())*((float)(sum_fitness)/(float)RAND_MAX); 
	//esolhendo o primeiro com aptidao maior que r
	float parcial_total = 0;
	int i = 0;
	char *aux = malloc(sizeof(char) * 23);
	while(parcial_total < r && i < 30){
		parcial_total += pop->individuals[i].fitness;
		i++;
	}

	strcpy(aux, pop->individuals[i-1].chromosome);	
	return aux;
}

//crossover de 1 ponto
void crossover_1p(char *parent1, char *parent2, char **sons, float rate_crossover){
 	float aux = rand()/(float)RAND_MAX;
 //	printf("taxa atual : %lf\n", aux);	
	strcpy(sons[0], parent1);
	strcpy(sons[1], parent2);
 	if(aux <= rate_crossover){
 		int num = aux * 22;
 		for(int i = num; i < 22; ++i){
 			sons[0][i] = parent2[i];
 			sons[1][i] = parent1[i];
 		}
 	}
}

//crossover de dois pontos
void crossover_2p(char *parent1, char *parent2, char **sons){
	int point1 = rand() % 22;
	int point2 = rand() % 22;
	strcpy(sons[0], parent1);
	strcpy(sons[1], parent2);
	if (point1 < point2){
		for (int i = point1; i <= point2; ++i){
			sons[0][i] = parent2[i];
			sons[1][i] = parent1[i];
		}
	}else{
		if (point2 < point1){
			for (int i = point2; i <= point1; ++i){
				sons[0][i] = parent2[i];
				sons[1][i] = parent1[i];
			}
		}
	}
}

//crossover uniforme
void uniform_crossover(char *parent1, char *parent2, char **sons){
	float aux = rand() % 4194304;//numero maximo representado por 22 bits
	char random_number[23];
	convert_decimal_to_binary(aux, random_number);
	for (int i = 0; i < 22; ++i){
		(random_number[i] == '1') ? ((sons[0][i] = parent1[i]) && (sons[1][i] = parent2[i])) : ((sons[0][i] = parent2[i]) && (sons[1][i] = parent1[i]));
	}
}

//mutação aplicada em todos os filhos gerados
void mutation(char *son1, char *son2, float rate_mutation){
	for (int i = 0; i < 22; ++i){
 		if(rand()/(float)RAND_MAX <= rate_mutation){
 			if(son1[i] == '1') son1[i] = '0';
 			else son1[i] = '1';
 		}

 		if(rand()/(float)RAND_MAX <= rate_mutation){
 			if(son2[i] == '1') son2[i] = '0';
 			else son2[i] = '1';
 		}	
		
	}
}

//mutação que seleciona o número de individuos que sofrerão mutação aleatoriamente, depois seleciona aleatoriamente quais 
//individuos sofrerão mutação, pra só então selecionar quais genes serão modificados 	
// void mutation_2(typ_population *pop, float rate_mutation){
// 	float num = rand()/(float)RAND_MAX;//selecionando quantos individuos serão modificados
// 	if(num <= rate_mutation){
// 		for (int i = 0; i < num; ++i){
// 			int son = rand() % 30;
// 			for (int i = 0; i < 22; ++i){
//  				if(rand()/(float)RAND_MAX <= rate_mutation){
//  					if(pop->individuals[son].chromosome[i] == '1') pop->individuals[son].chromosome[i] = '0';
//  					else pop->individuals[son].chromosome[i] = '1';
//  				}
//  			}
// 		}
// 	}
// }

char ** create_new_individuals(typ_population *pop, int sum_fitness){
	//definindo taxa de crossover e mutaçao
	float rate_crossover = 0.8;
	float rate_mutation = 0.03;
	//selecionando os pais 	
 	char *parent1 = roulette_wheel(pop, sum_fitness);
 	char *parent2 = roulette_wheel(pop, sum_fitness);

 	char **sons = malloc(sizeof(char*) * 2);
 	for (int i = 0; i < 2; ++i){
 		sons[i] = calloc(23, sizeof(char));
 	}
 	
 	//crossover_1p(parent1, parent2, sons, rate_mutation);
 	uniform_crossover(parent1, parent2, sons);
 	//crossover_2p(parent1, parent2, sons);
 	mutation(sons[0], sons[1], rate_mutation);

 	free(parent1);
 	free(parent2);
 	return sons;
}

//criando nova população a partir da geração anterior, sem elitismo
typ_population *create_new_population(typ_population *pop){
	//calculando aptidão total(soma da aptidão de todos indivíduos)
	float sum_fitness = 0;
	for (int i = 0; i < 30; ++i) sum_fitness += pop->individuals[i].fitness;

	typ_population *new_pop = calloc(1, sizeof(typ_population));
	//aplicando elitismo
	//copiando melhor individuo da população atual
	strcpy(new_pop->individuals[0].chromosome, pop->individuals[0].chromosome);
	new_pop->individuals[0].x = pop->individuals[0].x;
	new_pop->individuals[0].value = pop->individuals[0].value;
	new_pop->individuals[0].fitness = pop->individuals[0].fitness;
	//copiando segundo melhor individuo da população atual
	strcpy(new_pop->individuals[1].chromosome, pop->individuals[1].chromosome);
	new_pop->individuals[1].x = pop->individuals[1].x;
	new_pop->individuals[1].value = pop->individuals[1].value;
	new_pop->individuals[1].fitness = pop->individuals[1].fitness;

	char **sons;
	for (int i = 2; i < 30; i += 2){
		sons = create_new_individuals(pop, sum_fitness);//criando dois filhos
		strcpy(new_pop->individuals[i].chromosome, sons[0]);//copiando cromossomo filho1 para a nova população
		//printf("copiou %s, para posiçao %d\n", sons[0], i); 
		to_interval(&(new_pop->individuals[i]));//calculando valores
		strcpy(new_pop->individuals[i+1].chromosome, sons[1]);//copiando cromossomo filho2 para a nova população
		//printf("copiou %s, para posiçao %d\n", sons[1], i+1);   
		to_interval(&(new_pop->individuals[i+1]));
		//libernado a memoria alocada em sons
		for (int i = 0; i < 2; ++i){
			free(sons[i]);
		}
		free(sons);
	}
	mergesort(new_pop, 0, 29);
	fitness(new_pop);

	
	free(pop);

	return new_pop;
}

//criando população inicial
typ_population* initial_population(){
	typ_population *pop = calloc(1, sizeof(typ_population));
	int aux;
	for (int i = 0; i < 30; ++i){
		aux = rand() % 4194304;//numero maximo representado por 22 bits
		convert_decimal_to_binary(aux, pop->individuals[i].chromosome);//convertendo o numero aleatorio em binario
		to_interval(&(pop->individuals[i]));	
	}
	mergesort(pop, 0, 29);
	fitness(pop);
	return pop;	
}

int main(){
	srand(time(0));//usado para gerar numeros aleatorios diferentes em todas as chamadas da função rand


	typ_population *pop = initial_population();
	int i = 1;

 	while(pop->individuals[0].value <= 2.850272){
	 	pop = create_new_population(pop);
  		printf("valor = %lf, indo - loop %d\n", pop->individuals[0].value, i);
  		i++;
	}
 	printf("\npopulação final:\n");

 	for (int i = 0; i < 30; ++i){
 		printf("printf[%d] = %s\n", i,  pop->individuals[i].chromosome);
	 	printf("x = %lf, f de x = %lf, fitness = %lf, acumulated fitness = %lf\n", pop->individuals[i].x, pop->individuals[i].value, 
	 	pop->individuals[i].fitness, pop->individuals[i].acumulated_fitness);
 	}

  	free(pop);
	return 0;
}
