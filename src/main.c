/* Contador de palavras
 *
 * Este programa recebera uma serie de caracteres representando palavras em sua
 * entrada. Ao receber um caractere fim de linha ('\n'), deve imprimir na tela o
 * numero de palavras separadas que recebeu e, apos, encerrar.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


/*Variavel global que conta o numero de primos*/
int primos =0;
pthread_mutex_t trava;
int thr_ativa = 0;
pthread_t workers[4];
/* o vetor worker_status avalia no vetor de thread quais estão ativas.
	Se workers[i] estiver ativa= worker_status[i]=1;
	Se workers[i] estiver desativa= worker_status[i]=0;
	*/
int worker_status[4];

typedef struct {
  long int valor_Avaliado;//recebe o valor que deve ser avaliado (primo ou não)
  int thr_ref;//referencia a thread no vertor de threads (pode variar de 0 á 3); 
} thread_args;



/*Função que verifica se um numero é primo*/
int verifica_primo(long int vet){
	int i=0;int result=0;
	long int l=0;
	if(vet==2){ //dois é primo;
		return 1;
	}else if(vet==1){//1 não é primo;
		return 0;
	}else if(vet % 2 ==0){//se numero é par e maior que dois não é primo*/
		return 0;
	}else{//aqui se o numero é diferente de 1 ou 2, e não é par sendo maior que dois verificamos manualmente
		for (l = 2; l<= vet/2; l++) {
			if (vet % l == 0) {//verificaca se o numeor tem difisor
				result++;//se sim resul é incrementado
				break; 		
			}
		}
		if (result == 0) {
			return 1;//se não encontrou divisor retorna 1 (numero é primo);
		}else {
			return 0;// se encontra divisor retorna 0 numero não é primo;
		}
	}
			
}

void* worker(void *arg) {
  thread_args *info = (thread_args *)arg;
  int M = verifica_primo(info->valor_Avaliado);
  pthread_mutex_lock(&trava);
  primos = primos + M;
  thr_ativa -= 1;
  worker_status[info->thr_ref] = 0;
  free(info);
  pthread_mutex_unlock(&trava);
  return NULL;
}

int main(int argc, char **argv) {

	long int x[60];//armazena entrada
	char caracter='\0';
	int i;
	thread_args *parametros;
	int j;
	
	/*inicializar o vetor de entrada em -1, será critério de parada do loop que percerrerá a entrada*/
	for (i=0;i<60;i++){
		x[i]=-1;
	}
	
	/*leitura dos valores*/
	i=0;
	while(caracter!='\n'){
		/*aqui realiza a leitura da entrada*/
		scanf("%li%c", &x[i],&caracter);
		i++;
	}
	i=0;
	
	
	
	i=0;
	while (1) {
		if (x[i]==-1) break;//se totas as entradas já foram, ou estão sendo verificada por alguma thread, sai deste loop
		if (thr_ativa >= 4) {
		//aqui ja estamos com o maximo de threads e temos que agrardar ate que alguma termine
		} else {
		//se podemos criar uma thread  alocamos  uma estrutura com os parametros que serão esados posteriormente.
		  pthread_mutex_lock(&trava);
		  parametros = (thread_args*)malloc(sizeof(thread_args));
		  parametros->valor_Avaliado = x[i];
		  j = 0;
		  /*AQUI VERIFICA QUAL POSIÇÃO DO VETOR DE THREADS DEVE SER USADA PARA CRIAR A THREAD*/
		  while (worker_status[j] == 1){j++;
		  } 
		  parametros->thr_ref = j;//se saiu do loop anterior, temos uma thread disponivel e então á tornamos ativa.
		  worker_status[j] = 1;
		  thr_ativa += 1;//encrementamos a variavel que verifa quantas threads estão ativas
		  pthread_create(& (workers[j]), NULL, worker, (void*) parametros);//criamos a thread para verificar se a entrada é um numero primo
		  pthread_mutex_unlock(&trava);
		  i++;//passamos a verificação de uma nova entrada.
		}
		
	}

  /* Verifica se  todas as threads acabarem */
  for (int i=0; i<4; i++) {
    if (worker_status[i]==1) {
      pthread_join(workers[i], NULL);
    }
  }
  printf("%d\n",primos);//printa o resultado final;
	
	
	return 0;
}
