/************************************************************
* 	INTEGRANTES: 
*		Yonel Mamani
*		Jorge Macetas
*
*	
*	Implementacion Paralela para resolver el problema 
*	del camino mas corto con Bellman-Ford
*
*	COMPILACION Y EJECUCION:	
*	gcc bellman-ford.c -o bellman-ford -fopenmp -g -Wall 
*	./bellman-ford 4
**************************************************************/

#include <stdio.h>
#include <omp.h>
#include <time.h>



/*
*	estructura aristas
*	ariast de 'a' a 'b' com peso o costo 'peso'
*/
typedef struct { 
	int a, b, peso; 
}Edge;

int n; /* numero de nodos */
int e; /* numero de aristas */
Edge edges[1000000];
int d[100000]; /* distancias d[i]  minimas */
int aristas;
#define INFINITY 10000
/*
*	carga el numero de nodos y aristas de un archivo
*	estructura del archivo:
*	5        	indica el numeor de nodos ejemplo: 
*	10			numero de aristas
*	a 0 1 6		arista del nodo 0 al nodo 1 con peso 6
*	a 0 3 7     arista del nodo 0 al nodo 3 con peso 7
*	.
*	.
*	.
*/
void set_data( char *fl){
	int a,b,p;
	char c[1];
	int num_data;
	FILE *fin = fopen(fl, "r");
	fscanf(fin, "%d", &n);
	fscanf(fin, "%d", &num_data);
	aristas = num_data;
	e = 0;
	for (int i = 0; i < num_data; ++i){
		fscanf(fin, "%c",&c[0]);
		fscanf(fin, "%c %d %d %d", &c[0], &a, &b, &p);	
		//printf("%d %d %d\n", a, b, p);
		edges[e].a = a;
		edges[e].b = b;
		edges[e].peso = p;
		++e;
	}

}

/*
*	imprime las distancias minimas
*	de un nodo al resto de nodos
*/
void printDist() {
	int i;
	printf("distancias:\n");
	printf("n = %d \n", n);
	for (i = 0; i < n; ++i){
		printf("to %d\t : ", i +1);
		printf("%d\t \n", d[i]);
	}
}


void bellman_ford_normal(int s) {
	int i, j;
	for (i = 0; i < n; ++i)
		d[i] = INFINITY;
	d[s] = 0;
	int sum;
	for (i = 0; i < n-1 ; ++i){
		for (j = 0; j < e; ++j){
			sum = d[edges[j].a] + edges[j].peso;
			if ( d[edges[j].b] > sum){
			 	d[edges[j].b] = sum;
			}
		}
	}
}



void bellman_ford_paralelo(int s) {
	int i, j;
	for (i = 0; i < n; ++i)
		d[i] = INFINITY;
	d[s] = 0;
	int sum;
	#pragma omp parallel private (i) shared (d, edges)
			for (i = 0; i < n-1 ; ++i){
				#pragma omp for private(sum) schedule(dynamic, 8)
				for (j = 0; j < e; ++j){
					sum = d[edges[j].a] + edges[j].peso;
					if ( d[edges[j].b] > sum){
					 	d[edges[j].b] = sum;
					}
				}
			}
}



int main(int argc, char *argv[]) {
	/* cargamos aristas del archivo*/
	set_data("dist.in"); //dist.in s4_2.in s8_2.in s10_2.in s15_2.in
	clock_t start = clock();
 	
 	/*
 	ejecucion de bellman ford distancias minimas desde el nodo 0 a los demas nodos
 	*/

 	//bellman_ford_normal(0); 
	bellman_ford_paralelo(0);
    
    printf("Tiempo: %f \n", ((double)clock() - start) / CLOCKS_PER_SEC);
 	printDist();
	
	return 0;
}


/*
paralelo

s4
32  nodos
128 aristas
Tiempo: 0.064893

real	0m0.026s
user	0m0.132s
sys	0m0.000s

----------------
s8
256 nodos
1024 aristas
Tiempo: 0.328639

real	0m0.030s
user	0m0.156s
sys	0m0.000s

----------------
s10
1024 nodos 
4096 aristas
Tiempo: 1.099674

real	0m0.177s
user	0m1.168s
sys	0m0.008s

----------------
s15
32768  nodos
131072 aristas
Tiempo: 627.560578 

real	1m23.743s
user	10m21.664s
sys	0m1.052s


*/
