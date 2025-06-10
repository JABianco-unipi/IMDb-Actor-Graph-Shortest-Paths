#define _GNU_SOURCE   // permette di usare estensioni GNU
#include "xerrori.h"
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa exit() etc ...
#include <stdbool.h>  // gestisce tipo bool
#include <assert.h>   // permette di usare la funzione ass
#include <string.h>   // funzioni per stringhe
#include <errno.h>    // richiesto per usare errno
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <pthread.h>
#define QUI __LINE__, __FILE__


typedef struct {
    volatile sig_atomic_t fase; // 0: costruzione grafo, 1: lettura pipe, 2: termina
} segnale;


typedef struct {
    int codice;         // codice attore 
    char *nome;         // nome attore
    int anno;           // anno di nascita 
    int numcop;         // numero coprotagonisti 
    int *cop;           // array coprotagonisti
} attore;

typedef struct{
    char **buffer;    // buffer di lettura
    int *cindex;     // indice condiviso
    attore *array; // array di attori
    int dim; //dimensione array
    pthread_mutex_t *mutex; // mutex per proteggere l'indice
    sem_t *sem_free_slots; // semaforo per i posti liberi
    sem_t *sem_data_items; // semaforo per gli elementi disponibili
}dati;

typedef struct{// struct per cammini minimi
    attore* array;
    int *coppia;
    int dim; // dimensione dell'array di attori
}cdati;

// deifnizione di fifo per algoritmo di djkstra
typedef struct elem{
    struct elem *padre; // padre dell'elemento
    int attore; // attore contenuto nell'elemento
    struct elem *next; // prossimo elemento nella lista
}elemento;

typedef struct fifo{
    elemento *testa; // prossimo elemento nella lista
    elemento *coda; // ultimo elemento nella lista
}risolti;


// definizione abr per algoritmo di djkstra
typedef struct nodo{
    elemento *attore;// elemento del garbage collector
    int codice; //codice di attore
    struct nodo *sinistra; // figlio sinistro
    struct nodo *destra; // figlio destro
}root;

int ordina (attore *a, attore *b); //funzione per ordinare per ordine di codice attore

elemento *inizializza_elemento(int a); //inizializzazione elemento per la coda FIFO

bool is_empty_fifo(risolti *r); //controllo FIFO vuota

risolti *crea_fifo(elemento *a); //crea FIFO per la BFS

risolti *push_fifo(risolti *r, elemento *e);// mette elemento nella FIFO

elemento *pop_fifo(risolti *r); // estrae elemento dalla FIFO

root *crea_abr(int codice,elemento *e); // crea ABR per BFS

bool cerca_abr(root *a, int codice); // cerca codice nell'ABR

root *aggiungi_abr(root *a, int codice, elemento *e); // aggiunge codice all'ABR

void libera_abr(root *a); // distrugge ABR

int shuffle(int n); //shuffle per inserimento in ABR

int unshuffle(int n); //shuffle per estrazione da ABR

void stampa_attori(elemento *e, attore *array, int dim, FILE *f); // stampa ricorsiva degli attori
