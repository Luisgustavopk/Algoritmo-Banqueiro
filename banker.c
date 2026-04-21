
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

#define NUMBER_OF_CYCLES 10 



/* o montante disponível de cada recurso */
int available[NUMBER_OF_RESOURCES];

/* a demanda máxima de cada cliente */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* o montante correntemente alocado a cada cliente */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* a necessidade remanescente de cada cliente */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* Mutex Pthreads para proteger acesso concorrente (Seção 5.9.4) */
pthread_mutex_t mutex;

/* =========================================================================
 * Utilitários de impressão
 * ========================================================================= */

void print_vector(const char *label, int *v, int n) {
    printf("%s[", label);
    for (int i = 0; i < n; i++)
        printf("%d%s", v[i], i < n - 1 ? ", " : "");
    printf("]");
}

void print_state(void) {
    printf("\n========== Estado atual do sistema ==========\n");
    print_vector("Disponível: ", available, NUMBER_OF_RESOURCES);
    printf("\n");
    printf("Alocação:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("  Cliente %d: ", i);
        print_vector("", allocation[i], NUMBER_OF_RESOURCES);
        printf("\n");
    }
    printf("Need:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("  Cliente %d: ", i);
        print_vector("", need[i], NUMBER_OF_RESOURCES);
        printf("\n");
    }
    printf("=============================================\n\n");
}

/* =========================================================================
 * Algoritmo de Segurança — Seção 7.5.3.1
 * Retorna 1 se seguro, 0 se inseguro.
 * ========================================================================= */

int is_safe_state(void) {
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];


    memcpy(work, available, sizeof(available));
    memset(finish, 0, sizeof(finish));


    int count = 0;
    while (count < NUMBER_OF_CUSTOMERS) {
        int found = 0;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (finish[i]) continue;

        
            int can_finish = 1;
            for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                if (need[i][j] > work[j]) { can_finish = 0; break; }
            }

            if (can_finish) {
                
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
                    work[j] += allocation[i][j];
                finish[i] = 1;
                found = 1;
                count++;
            }
        }
        if (!found) break;
    }

  
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        if (!finish[i]) return 0;
    return 1;
}

/* =========================================================================
 * request_resources — (Seção 7.5.3)
 * Retorna  0 se bem sucedido
 * Retorna -1 se não houver êxito
 * ========================================================================= */

int request_resources(int customer_num, int request[]) {
    pthread_mutex_lock(&mutex);

    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            printf("[Cliente %d] ERRO: solicitação excede o need declarado. Negada.\n",
                   customer_num);
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            printf("[Cliente %d] Recursos indisponíveis. Negada.\n", customer_num);
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }

    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i]                -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i]       -= request[i];
    }

  
    if (is_safe_state()) {
        printf("[Cliente %d] Solicitação APROVADA — estado seguro.\n", customer_num);
        pthread_mutex_unlock(&mutex);
        return 0;
    } else {
    
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i]                += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i]       += request[i];
        }
        printf("[Cliente %d] Solicitação NEGADA — estado inseguro.\n", customer_num);
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}

/* =========================================================================
 * Retorna  0 se bem-sucedido
 * Retorna -1 se não houver êxito
 * ========================================================================= */

int release_resources(int customer_num, int release[]) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] > allocation[customer_num][i]) {
            printf("[Cliente %d] ERRO: liberação excede o alocado.\n", customer_num);
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i]                += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i]       += release[i];
    }

    printf("[Cliente %d] Liberação concluída.\n", customer_num);
    pthread_mutex_unlock(&mutex);
    return 0;
}

/* =========================================================================
 * Thread de cliente
 * ========================================================================= */

void *customer_thread(void *arg) {
    int id = *(int *)arg;

    for (int cycle = 0; cycle < NUMBER_OF_CYCLES; cycle++) {
        int request[NUMBER_OF_RESOURCES];

        pthread_mutex_lock(&mutex);
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            int max_req = need[id][i];
            request[i] = (max_req > 0) ? (rand() % max_req) + 1 : 0;
        }
        pthread_mutex_unlock(&mutex);

        printf("[Cliente %d | Ciclo %d] Solicitando recursos...\n", id, cycle + 1);

        if (request_resources(id, request) == 0) {
           
            usleep((rand() % 300 + 100) * 1000); 

            printf("[Cliente %d | Ciclo %d] Liberando recursos...\n", id, cycle + 1);
            release_resources(id, request);
        } else {
      
            usleep((rand() % 200 + 50) * 1000); 
        }
    }

    printf("[Cliente %d] Finalizou todos os ciclos.\n", id);
    return NULL;
}


void initialize(int argc, char *argv[]) {

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        available[i] = atoi(argv[i + 1]);

    srand((unsigned)time(NULL));

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            maximum[i][j]    = (rand() % available[j]) + 1;
            allocation[i][j] = 0;
            need[i][j]       = maximum[i][j];
        }
    }

    pthread_mutex_init(&mutex, NULL);

    printf("Sistema iniciado: %d clientes, %d tipos de recursos.\n",
           NUMBER_OF_CUSTOMERS, NUMBER_OF_RESOURCES);
    print_vector("Disponível: ", available, NUMBER_OF_RESOURCES);
    printf("\nMaximum por cliente:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("  Cliente %d: ", i);
        print_vector("", maximum[i], NUMBER_OF_RESOURCES);
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[]) {
    if (argc != NUMBER_OF_RESOURCES + 1) {
        fprintf(stderr, "Uso: %s", argv[0]);
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
            fprintf(stderr, " <rec%d>", i + 1);
        fprintf(stderr, "\nExemplo: %s 10 5 7\n", argv[0]);
        return 1;
    }

    initialize(argc, argv);
    print_state();

    pthread_t threads[NUMBER_OF_CUSTOMERS];
    int ids[NUMBER_OF_CUSTOMERS];

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, customer_thread, &ids[i]);
    }


    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        pthread_join(threads[i], NULL);

    printf("\nTodos os clientes finalizaram.\n");
    print_state();

    pthread_mutex_destroy(&mutex);
    return 0;
}