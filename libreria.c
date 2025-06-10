#define _GNU_SOURCE
#include "libreria.h"


int ordina (attore *a, attore *b){
    if (a->codice < b-> codice) return -1;
    else if (a->codice > b-> codice) return 1;
    return 0;
}
// funzioni fifo per algoritmo di djkstra

elemento *inizializza_elemento(int a){
    elemento *e = malloc(sizeof(elemento)); // alloco memoria per l'elemento
    if (e == NULL){
        xtermina("Malloc fallita", QUI);
    }
    e->padre = NULL; // inizializzo padre a NULL
    e->attore = a; // inizializzo attore a NULL
    e->next = NULL; // inizializzo next a NULL
    return e; // ritorno l'elemento inizializzato
}

bool is_empty_fifo(risolti *r){
    return r->testa == NULL; // ritorno true se la fifo è vuota
}

risolti *crea_fifo(elemento *a){
    risolti *r = malloc(sizeof(risolti)); // alloco memoria per la fifo
    if (r == NULL){
        xtermina("Malloc fallita", QUI);
    }
    r->testa = a; // inizializzo head
    r->coda = a; // inizializzo tail
    return r; // ritorno la fifo inizializzata

}
risolti *push_fifo(risolti *r, elemento *e){
    if(r->testa == NULL){ // se la fifo è vuota
        r->testa = e; // inserisco il primo elemento
        r->coda = e; // aggiorno tail al primo elemento
    } else {
        r->coda->next = e; // inserisco il nuovo elemento alla fine della lista
        r->coda = e; // aggiorno tail al nuovo elemento
    }
    e->next = NULL; // imposto il prossimo elemento a NULL
    return r; // ritorno la fifo aggiornata
}
elemento *pop_fifo(risolti *r){
    if (r->testa == NULL){ // se la fifo è vuota
        return NULL;
    }
    elemento *estratto = r->testa; // prendo il primo elemento della fifo
    r->testa = r->testa->next; // aggiorno head al prossimo elemento
    if (r->testa == NULL){ // se la fifo diventa vuota
        r->coda = NULL; // aggiorno tail a NULL
    }
    estratto->next = NULL; // imposto il prossimo elemento a NULL
    return estratto; // ritorno l'elemento estratto

}

// funzioni abr per algoritmo di djkstra

root *crea_abr(int codice,elemento *e){
    root *a = malloc(sizeof(root)); // alloco memoria per il nodo
    if (a == NULL){
        xtermina("Malloc fallita", QUI);
    }
    a->attore = e;
    a->codice = codice; // inizializzo codice
    a->sinistra = NULL; // inizializzo figlio sinistro a NULL
    a->destra = NULL; // inizializzo figlio destro a NULL
    return a; // ritorno il nodo inizializzato
}
bool cerca_abr(root *a, int codice){
    if(a == NULL){
        return false;
    }
    if(a->codice == codice){
        return true;
    }
    if (codice < a->codice){
        return cerca_abr(a->sinistra, codice); //ricorsione sinistra
    }
    if (codice > a->codice){
        return cerca_abr(a->destra, codice); //ricorsione destra
    }
    return false; // ritorno falso altrimenti
}

root *aggiungi_abr(root *a, int codice, elemento *e){
    if (a == NULL){ // se l'albero è vuoto, creo il nodo
        return crea_abr(codice, e);
    }
    // altrimenti, aggiungo il nodo nell'albero
    if (codice < a->codice){
        a->sinistra = aggiungi_abr(a->sinistra, codice, e); // chiamo ricorsivamente la funzione per il figlio sinistro
    } else if (codice >= a->codice){
        a->destra = aggiungi_abr(a->destra, codice, e); // chiamo ricorsivamente la funzione per il figlio destro
    }
    return a; // ritorno l'albero aggiornato
}


void libera_abr(root *a){
    if(a != NULL){
        libera_abr(a->sinistra);
        libera_abr(a->destra);
        free(a->attore);// libero garbage collector
        free(a); // libero il nodo
    }
}

// funzioni di inserimento e ricerca in abr

int shuffle(int n) {
  return ((((n & 0x3F) << 26) | ((n >> 6) & 0x3FFFFFF)) ^ 0x55555555);
}
int unshuffle(int n) {
  return ((((n >> 26) & 0x3F) | ((n & 0x3FFFFFF) << 6)) ^ 0x55555555);
}


void stampa_attori(elemento *e, attore *array, int dim, FILE *f){
    if (e != NULL){
        stampa_attori(e->padre,array,dim,f);
        attore *trovato = bsearch(&e->attore, array, dim, sizeof(attore), (__compar_fn_t) &ordina);
        fprintf(f,"%d\t%s\t%d\n", trovato->codice, trovato->nome, trovato->anno); // stampo attore
    }
    return;
}