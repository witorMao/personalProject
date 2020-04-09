#include "alg_gen_real.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.14159265359

double goal_function(int x, int y){
	return (0.5 - ((pow(sin(sqrt((x*x) + (y*y))), 2) - 0.5)/(pow((1 + 0.001 * ((x*x) +(y*y))), 2)))); 
}

//algoritmo mergesort utilizado para a ordenadção da população pelo valor de f de x
void mergesort(typ_population *pop, int initialposition , int lastposition) {
	if (lastposition <= initialposition){ 
		return;
	}
	int center = (int) ((lastposition+initialposition)/2.0);
	mergesort(pop, initialposition, center);
	mergesort(pop, center+1, lastposition);

	typ_individual* aux = malloc(sizeof(typ_individual) * (lastposition-initialposition+1) );
	for (int k = 0; k < (lastposition-initialposition+1); ++k){
		aux[k].parameters = malloc(sizeof(double) * pop->parameters_num);
	}
	int i = initialposition;
	int j = center+1;
 	int k = 0;
 	while (i <= center && j <= lastposition) {
		if (pop->individuals[i].value > pop->individuals[j].value){
			for (int l = 0; l < pop->parameters_num; ++l){
				aux[k].parameters[l] = pop->individuals[i].parameters[l]; 
			}
			aux[k].value = pop->individuals[i].value;
			i++;
		} 
		else {
			for (int l = 0; l < pop->parameters_num; ++l){
				aux[k].parameters[l] = pop->individuals[j].parameters[l]; 
			}
			aux[k].value = pop->individuals[j].value;
			j++;
		}
		k++;
	}

	while (i <= center) {
		for (int l = 0; l < pop->parameters_num; ++l){
			aux[k].parameters[l] = pop->individuals[i].parameters[l]; 
		}
		aux[k].value = pop->individuals[i].value;
		i++; 
		k++;
	}	

	while (j <= lastposition) {
		for (int l = 0; l < pop->parameters_num; ++l){
			aux[k].parameters[l] = pop->individuals[j].parameters[l]; 
		}
		aux[k].value = pop->individuals[j].value;
		j++; 
		k++;
	}	

	for (i = initialposition, k = 0; i <= lastposition; i++, k++) {
		for (int l = 0; l < pop->parameters_num; ++l){
			pop->individuals[i].parameters[l] = aux[k].parameters[l]; 
		}
		pop->individuals[i].value = aux[k].value;
	}

	for (int k = 0; k < (lastposition-initialposition+1); ++k){
		free(aux[k].parameters);
	}
	free(aux); 
}

//calcula a aptidão de cada individuo, juntamente com a aptidão acumulada, pela mesma função apresentada para calculo dela na representação binaria
void fitness(typ_population *pop){
	for (int i = 0; i < 30; ++i){
		pop->individuals[i].fitness = 2 * (30 - (i + 1))/(double)(30 - 1);
		for (int j = 0; j <= i; ++j){
			pop->individuals[i].acumulated_fitness += pop->individuals[j].fitness; 
		}
	}
	return;
}

double *roulette_wheel(typ_population *pop, int sum_fitness){
	//gerando r entre 2 e 30
	double r = (double)(rand())*((double)(sum_fitness)/(double)RAND_MAX); 
	//escolhendo o primeiro com aptidao maior que r
	double parcial_total = 0;
	int i = 0;
	double *aux = malloc(sizeof(double) * pop->parameters_num);
	while(parcial_total < r && i < 30){
		parcial_total += pop->individuals[i].fitness;
		i++;
	}
	//copiando o cromossomo
	for (int j = 0; j < pop->parameters_num; ++j){
		aux[j] = pop->individuals[i-1].parameters[j];
	}
	return aux;
}

//Crossovers mostrados no pdf

//crossover por média
double* average_crossover(double *parent1, double *parent2, int parameters_num){
	double *son = malloc(sizeof(double) * parameters_num);
	for (int i = 0; i < parameters_num; ++i){
		son[i] = (parent1[i] + parent2[i])/2.0;
	}
	return son;
}

//crossover por média geomatrica, problema: raiz de numeros negativos, por isso não pode ser aplicada no caso dessa função
double* geometric_average_crossover(double *parent1, double *parent2, int parameters_num){
	double *son = malloc(sizeof(double) * parameters_num);
	for (int i = 0; i < parameters_num; ++i){
		son[i] = sqrt(parent1[i] * parent2[i]);
	}
	return son;
}

//crossover BLX com beta variável
double* BLX_crossover(double *parent1, double *parent2, int parameters_num){
	double *son = malloc(sizeof(double) * parameters_num);
	//alfa valendo 0.5
	double beta;	
	int i = 0;
	while (i < parameters_num){
		beta = ((rand()/(double)RAND_MAX) * 2) - 0.5;
		son[i] = parent1[i] + beta * (parent2[i] - parent1[i]);
		if (son[i] >= -100 && son[i] <= 100) i++;
	}
	return son;
}

//crossover linear, problema: gerando filhos fora do intervalo do x e y 
double* linear_crossover(double *parent1, double *parent2, int parameters_num){
	double *son1 = malloc(sizeof(double) * parameters_num);
	double *son2 = malloc(sizeof(double) * parameters_num);
	double *son3 = malloc(sizeof(double) * parameters_num);
	for (int i = 0; i < parameters_num; ++i){
		son1[i] = 0.5 * parent1[i] + 0.5 * parent2[i];
		son2[i] = 1.5 * parent1[i] - 0.5 * parent2[i];
		son3[i] = - 0.5 * parent1[i] + 1.5 * parent2[i];
	}	

	double values[3];
	
	values[0] = goal_function(son1[0], son1[1]);
	values[1] = goal_function(son2[0], son2[1]);
	values[2] = goal_function(son3[0], son3[1]);

	
	// printf("son1 - x = %lf, y = %lf, f de x,y = %lf\n", son1[0], son1[1], values[0]);
	// printf("son2 - x = %lf, y = %lf, f de x,y = %lf\n", son2[0], son2[1], values[1]);
	// printf("son3 - x = %lf, y = %lf, f de x,y = %lf\n", son3[0], son3[1], values[2]);
	// printf("\n");

	double bigger = values[0];
	int i = 0;
	while(i < 3){
		if (values[i] > bigger){
			bigger = values[i];
		}	
		++i;
	}

	int j = 0;

	while(j < 3){
		if (values[j] == bigger){
			break;
		}
		j++;
	}

	if (j == 0) {
		free(son2);
		free(son3);
		return son1;
	}
	if (j == 1) {
		free(son1);
		free(son3);
		return son2;
	}
	if (j == 2) {
		free(son1);
		free(son2);
		return son3;
	}
}

//crossover aritmetico
double** aritmetic_crossover(double *parent1, double *parent2, int parameters_num){
	double **son = malloc(sizeof(double*) * 2);
	for (int i = 0; i < 2; ++i){
		son[i] = malloc(sizeof(double) * parameters_num);
	}

	double beta = (rand()/(double)RAND_MAX);
	int i = 0;
	while (i < parameters_num){
		son[0][i] = beta * parent1[i] + (1 - beta) * parent2[i];
		son[1][i] = (1 - beta) * parent1[i] + beta * parent2[i];
		i++;
	}
	return son;
}

//crossover heuristico
double* heuristic_crossover(typ_population *pop, int parameters_num){
	int num1 = rand() % 30;//buscando parent1
	int num2 = num1 + rand() % (30 - num1);//buscando parent2 de aptidão menor que parent1

	double *son = malloc(sizeof(double) * parameters_num);
	//encontrando parent1 de aptidao maior que parent2
	double r;	
	int i = 0;
	int t = 1;
	//número máximo de tentativas na geração do filho = 20
	while (t != 20 && i < parameters_num){
		r = (rand()/(double)RAND_MAX);
		son[i] = pop->individuals[num1].parameters[i] + (r * (pop->individuals[num1].parameters[i] - pop->individuals[num2].parameters[i]));
		if ((son[i] >= -100 && son[i] <= 100)) i++;
		else t++;
	}

	//copiando o pai no lugar do filho caso o número máximo de tentativas seja atingido
	if(t == 20){
		for (int i = 0; i < parameters_num; ++i){
			son[i] = pop->individuals[num1].parameters[i];
		}
	}
	// printf("r = %lf\n", r);
	// printf("parent1 - x: %lf, y: %lf\n", pop->individuals[num1].parameters[0], pop->individuals[num1].parameters[1]);
	// printf("parent2 - x: %lf, y: %lf\n", pop->individuals[num2].parameters[0], pop->individuals[num2].parameters[1]);

	// printf("x: %lf, y: %lf\n", son[0], son[1]);
	// printf("\n");
	return son;
}

//gerar número de uma distribuição normal, por esse método pode-se gerar dois números, mas nesse código apenas é utilizado um dos números gerados
double box_muller_transform(double media, double deviation){
	double u1, u2;
	//números de distribuição uniforme
	u1 = rand()/(double)RAND_MAX;
//		printf("rand = %d, randmax = %d, u1 = %lf\n",rando, RAND_MAX, u1);
	
	
	while(u1 == 0.00) {
		u1 = rand()/(double)RAND_MAX;
//		printf("rand = %d, randmax = %d, u1 = %lf\n",rando, RAND_MAX, u1);
	}
	u2 = rand()/(double)RAND_MAX;
	//gerando os números da distribuição normal
	double z1 = sqrt(-2*log(u1))*cos(2*PI*u2);
	double z2 = sqrt(-2*log(u1))*sin(2*PI*u2);
	return (z1*deviation + media);
}

//mutações mostradas no pdf
//mutação uniforme
void uniform_mutation(typ_individual *ind, int parameters_num){
	int pos = rand() % parameters_num;

	ind->parameters[pos] = ((rand()/(double)RAND_MAX) * 200) - 100;
	//printf("pos = %d, new_gene = %lf\n", pos, ind->parameters[pos]);
	
	//printf("pos = %d, new_gene = %lf\n", pos, ind->parameters[pos]);
}

//mutação guassiana
void gaussian_mutation(typ_individual *ind, int parameters_num){
	int pos = rand() % parameters_num;
	int deviation = 5;

	//calculando a média do cromossomo
	double media = 0;
	for (int i = 0; i < parameters_num; ++i) media += ind->parameters[i];
	media = media/parameters_num;
	
	//gerando números de uma distribuição normal dentro do intervalo das variáveis da função
	double aux = box_muller_transform(media, deviation);
	while(aux < -100 || aux > 100) aux = box_muller_transform(media, deviation);
	ind->parameters[pos] = aux;
}

//mutação creep, nessa função a mutação acontece em apenas um gene escolhido aleatoriamente
void creep_mutation(typ_individual *ind, int parameters_num){
	int pos = rand() % parameters_num;
	
	double small_num = box_muller_transform(0, 4);
	double aux = ind->parameters[pos] + small_num; 
	//verificando se o número gerado está dentro do intervalo de x e y
	while(aux > 100 || aux < -100){
		small_num = box_muller_transform(0, 4);
		aux = ind->parameters[pos] + small_num;
	}
	ind->parameters[pos] = aux;
}

//mutação limite
void limit_mutation(typ_individual *ind, int parameters_num){
	int pos = rand() % parameters_num;
	//printf("pos = %d, new_gene = %lf\n", pos, ind->parameters[pos]);
	double r = rand()/(double)RAND_MAX;
	if (r < 0.5) ind->parameters[pos] = -100;
	else if(r >= 0.5) ind->parameters[pos] = 100;
	//printf("pos = %d, new_gene = %lf\n", pos, ind->parameters[pos]);
}

//mutação não uniforme
void irregular_mutation(typ_individual *ind, int parameters_num, int generation, int max_generation){
	int pos = rand() % parameters_num;
	double r1 = rand()/(double)RAND_MAX;
	double r2 = rand()/(double)RAND_MAX;
	//6 equivale ao b da equação no pdf 
	double f_de_g = pow((r2 * (1-(generation/max_generation))), 6);
	if (r1 < 0.5) ind->parameters[pos] = ind->parameters[pos] + ((100 - ind->parameters[pos]) * f_de_g);
	else if(r1 >= 0.5) ind->parameters[pos] = ind->parameters[pos] - ((ind->parameters[pos] -(-100)) * f_de_g);
	//printf("pos = %d, new_gene = %lf, generation = %d\n", pos, ind->parameters[pos], generation);

}

//mutação não unifiorme mútipla
void multiple_irregular_mutation(typ_individual *ind, int parameters_num, int generation, int max_generation){
	double r1;
	double r2;
	double f_de_g;

	for (int i = 0; i < parameters_num; ++i){
		r1 = rand()/(double)RAND_MAX;
		r2 = rand()/(double)RAND_MAX;
		f_de_g = pow((r2 * (1-(generation/max_generation))), 6);
		if (r1 < 0.5) ind->parameters[i] = ind->parameters[i] + ((100 - ind->parameters[i]) * f_de_g);//100 é o limite superior do domínio de x e y
		else if(r1 >= 0.5) ind->parameters[i] = ind->parameters[i] - ((ind->parameters[i] -(-100)) * f_de_g);//-100 é o limite inferior do domínio de x e y
		//printf("pos = %d, new_gene = %lf, generation = %d\n", i, ind->parameters[i], generation);	
	}	
}

//funções pra criação de nova população e de indivíduos

//fução para criação de novos individuos com o crossover aritmetico, pois ele retorna dois filhos, diferente de todos os outros crossovers  
void create_new_individuals_aritmetic(typ_population *pop, typ_individual *ind1, typ_individual *ind2, int sum_fitness, int max_generation){
	//definindo taxa de crossover e mutaçao
	double rate_crossover = 0.8;
	double rate_mutation = 0.03;
	//selecionando os pais 	
 	double *parent1 = roulette_wheel(pop, sum_fitness);
 	double *parent2 = roulette_wheel(pop, sum_fitness);

 	//aplicando o crosover nos filhos
 	double **sons = aritmetic_crossover(parent1, parent2, pop->parameters_num);

 	ind1->parameters = sons[0];
 	ind2->parameters = sons[1];

 	//aplicando mutação nos filhos, a mutação escolhida deve ser aplicada nos dois filhos
 	//para selecionar a mutação desejada basta descomentá-la e comentar a que estava sendo utilizada na última execução do código
 	uniform_mutation(ind1, pop->parameters_num);
	uniform_mutation(ind2, pop->parameters_num);
 	//gaussian_mutation(ind1, pop->parameters_num);
 	//gaussian_mutation(ind2, pop->parameters_num);
 	//creep_mutation(ind1, pop->parameters_num);
 	//creep_mutation(ind2, pop->parameters_num);
 	//limit_mutation(ind1, pop->parameters_num);
 	//limit_mutation(ind2, pop->parameters_num); 
 	//irregular_mutation(ind1, pop->parameters_num, pop->generation, max_generation);
 	//irregular_mutation(ind2, pop->parameters_num, pop->generation, max_generation);
	//multiple_irregular_mutation(ind1, pop->parameters_num, pop->generation, max_generation);
 	//multiple_irregular_mutation(ind2, pop->parameters_num, pop->generation, max_generation);

 	free(sons);

 	ind1->value = goal_function(ind1->parameters[0], ind1->parameters[1]);
 	ind2->value = goal_function(ind2->parameters[0], ind2->parameters[1]);
 	//mutation(sons[0], sons[1], rate_mutation);
 	free(parent1);
 	free(parent2);
}
//fução para criação de nova população com o crossover aritmetico, pois ele retorna dois filhos, diferente de todos os outros crossovers
typ_population* create_new_population_aritmetic(typ_population *pop, int max_generation){
	double sum_fitness = 0;
	for (int i = 0; i < 30; ++i){
		sum_fitness += pop->individuals[i].fitness;
	}
	
	typ_population *new_pop = calloc(1, sizeof(typ_population));
	new_pop->generation = pop->generation + 1;
	new_pop->parameters_num = pop->parameters_num;
	//aplicando o elistismo(copia do melhor indivíduo da geração anterior na nova população)
	// new_pop->individuals[0].parameters = malloc(sizeof(double) * pop->parameters_num);
	// for (int i = 0; i < pop->parameters_num; ++i) new_pop->individuals[0].parameters[i] = pop->individuals[0].parameters[i];
	// new_pop->individuals[0].value = pop->individuals[0].value;
	
	typ_individual aux;
	aux.parameters = NULL;
	//nesse for caso o elitismo seja aplicado deve-se inicializar i com 1, caso contrário deve ser inicializado com 0
	for (int i = 0; i < 30; i += 2){
		if (i == 29) create_new_individuals_aritmetic(pop, &new_pop->individuals[i], &aux, (int)sum_fitness, max_generation); 	
		else create_new_individuals_aritmetic(pop, &new_pop->individuals[i], &new_pop->individuals[i+1], (int)sum_fitness, max_generation); 
	}

	if (aux.parameters != NULL) free(aux.parameters);

	for (int i = 0; i < 30; ++i){
		free(pop->individuals[i].parameters);	
	}
	free(pop);

	mergesort(new_pop, 0, 29);
	fitness(new_pop);

	return new_pop;
}

//fução para criação de novos individuos com todos os crossovers, exceto o aritmetico  
void create_new_individuals(typ_population *pop, typ_individual *ind, int sum_fitness, int max_generation){
	//definindo taxa de crossover e mutaçao
	double rate_crossover = 0.8;
	double rate_mutation = 0.03;
	//selecionando os pais 	
 	double *parent1 = roulette_wheel(pop, sum_fitness);
 	double *parent2 = roulette_wheel(pop, sum_fitness);

 	//aplicando o crossover no filho
 	//para selecionar crossover desejado basta descomentá-lo e comentar o que estava sendo utilizado na última execução do código
 	//ind->parameters = average_crossover(parent1, parent2, pop->parameters_num);
 	/////problema: nesse caso dá raíz de número negativo/////ind->parameters = geometric_average_crossover(parent1, parent2, pop->parameters_num);
 	//ind->parameters = BLX_crossover(parent1, parent2, pop->parameters_num);
 	/////problema extrapolando o intervalo/////ind->parameters = linear_crossover(parent1, parent2, pop->parameters_num);
 	ind->parameters = heuristic_crossover(pop, pop->parameters_num);
 	
 	//aplicando a mutação no filho gerado
 	//para selecionar a mutação desejada basta descomentá-la e comentar a que estava sendo utilizada na última execução do código
 	uniform_mutation(ind, pop->parameters_num);
	//gaussian_mutation(ind, pop->parameters_num);
 	//creep_mutation(ind, pop->parameters_num);
 	//limit_mutation(ind, pop->parameters_num);
 	//irregular_mutation(ind, pop->parameters_num, pop->generation, max_generation);
	//multiple_irregular_mutation(ind, pop->parameters_num, pop->generation, max_generation);


 	ind->value = goal_function(ind->parameters[0], ind->parameters[1]);
 	//mutation(sons[0], sons[1], rate_mutation);
 	free(parent1);
 	free(parent2);
}

//fução para criação de nova população com todos os crossovers, exceto o aritmetico
typ_population* create_new_population(typ_population *pop, int max_generation){
	double sum_fitness = 0;
	for (int i = 0; i < 30; ++i){
		sum_fitness += pop->individuals[i].fitness;
	}
	
	typ_population *new_pop = calloc(1, sizeof(typ_population));
	new_pop->generation = pop->generation + 1;
	new_pop->parameters_num = pop->parameters_num;
	//aplicando o elistismo(copia do melhor indivíduo da geração anterior na nova população)

	new_pop->individuals[0].parameters = malloc(sizeof(double) * pop->parameters_num);
	// for (int i = 0; i < pop->parameters_num; ++i) {
	// 	new_pop->individuals[0].parameters[i] = pop->individuals[0].parameters[i];
	// }
	// new_pop->individuals[0].value = pop->individuals[0].value;

	//nesse for caso o elitismo seja aplicado deve-se inicializar i com 1, caso contrário deve ser inicializado com 0
	for (int i = 0; i < 30; ++i){
		create_new_individuals(pop, &new_pop->individuals[i], (int)sum_fitness, max_generation); 
	}

	for (int i = 0; i < 30; ++i){
		free(pop->individuals[i].parameters);	
	}
	free(pop);
	mergesort(new_pop, 0, 29);
	fitness(new_pop);

	return new_pop;
}

//criando população inicial
typ_population* initial_population(){
	typ_population *pop = calloc(1, sizeof(typ_population));
	pop->parameters_num = 2;//coloca-se aqui o número de parametros da função objetivo
	pop->generation = 0;//inicializando a geração da população
	for (int i = 0; i < 30; ++i){
		pop->individuals[i].parameters = calloc(pop->parameters_num, sizeof(double));
		//gerando numeros reais aleatorios entre -100 e 100, 200 representa o tamanho do intervalo do dominio
		pop->individuals[i].parameters[0] = ((rand()/(double)RAND_MAX) * 200.0) - 100.0 ;//x
		pop->individuals[i].parameters[1] = ((rand()/(double)RAND_MAX) * 200.0) - 100.0 ;//y
		pop->individuals[i].value = goal_function(pop->individuals[i].parameters[0], pop->individuals[i].parameters[1]);//calculando
	}
	//ordenando a população pra poder ser aplicada a função para o cálculo da aptidão
	mergesort(pop, 0, 29);
	//calculando a aptidão
	fitness(pop);
	return pop;	
}
