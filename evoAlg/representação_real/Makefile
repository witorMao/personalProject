all: alggen
alggen: alg_gen_real.o real_main.o
	gcc -o alggen -g alg_gen_real.o real_main.o  -lm
alg_gen_real.o: alg_gen_real.c alg_gen_real.h
	gcc -o alg_gen_real.o -c alg_gen_real.c 
real_main.o: real_main.c alg_gen_real.h
	gcc -o real_main.o -c real_main.c 
clean:
	rm -rf *.o
mrproper: clean
	rm -rf alggen
run: alggen
	./alggen