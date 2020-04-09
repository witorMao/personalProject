#include "alg_gen_real.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
	srand(time(0));//usado para gerar numeros aleatorios diferentes em todas as chamadas da função rand

  	int max_generation = 1000;
 	typ_population *pop;
 	clock_t start, end;
 	double time = 0;
	for (int i = 0; i < 100; ++i){
 		start = clock();
		pop = initial_population();
	 	
	 	//loop até atingir o máximo global da função que é 1
	 	while(pop->individuals[0].value != 1.0){
	 		/*Função para chamada do crossover aritmetico, além da chamada das mutações uniforme, limite, não uniforme 
			e multipla não uniforme. Caso queria mudar a mutação selecinada, basta ir no arquivo alg_gen_real.c, na função
			create_new_population_aritmetic, descomentar a linha da mutação desejada e comentar a linha da atual*/
	 		//pop = create_new_population_aritmetic(pop, max_generation);
			
			/*Função para chamada dos crossovers media, blx e heuristisco, além da chamada das mutações uniforme, limite, não uniforme 
			e multipla não uniforme. Caso queria mudar o crossover ou a mutação selecinada, basta ir no arquivo alg_gen_real.c, na função
			create_new_population, descomentar a linha do crossover ou mutação desejado e comentar a linha do atual*/
	 		pop = create_new_population(pop, max_generation);
	 	// 	for (int i = 0; i < 30; ++i){
			// 	printf("x = %lf, y = %lf, value = %lf\n", pop->individuals[i].parameters[0], pop->individuals[i].parameters[1], pop->individuals[i].value);	
			// }
			// printf("\n");
	 	}

	 	for (int i = 0; i < 30; ++i){
			free(pop->individuals[i].parameters);	
		}
	  	free(pop);
		
		end = clock();
		time += (end - start) / (double)CLOCKS_PER_SEC;
		//printf("tempo em segundos = %lf\n", time);
	}
	printf("4\t%lf\n", time/100);
		
	return 0;
}