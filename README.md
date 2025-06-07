# EsameLab2
Consegna progetto Lab2
# implementazione della coda FIFO nell'algoritmo della BFS
Nel mio programma, la coda FIFO necessaria per la BFS è implementata tramite una lista collegata (linked list) di puntatori a elementi, dove ciascun elemento è una struttura (struct elem, alias elemento) che rappresenta un nodo della coda.
Ogni elemento contiene:

   - attore: il codice numerico dell’attore associato a quel nodo,

   - padre: un puntatore al nodo predecessore lungo il cammino,

   - next: il puntatore al prossimo elemento nella lista.
```c
typedef struct elem{
    struct elem *padre; // padre dell'elemento
    int attore; // attore contenuto nell'elemento
    struct elem *next; // prossimo elemento nella lista
}elemento;

typedef struct fifo{
    elemento *testa; // prossimo elemento nella lista
    elemento *coda; // ultimo elemento nella lista
}risolti;
```
La coda stessa (struct fifo, alias risolti) mantiene i puntatori alla testa e alla coda della lista, così da permettere efficientemente le operazioni di inserimento in coda (push) e di estrazione dalla testa (pop), come richiesto da una vera FIFO.
# ricostruzione del cammino minimo
Per ricostruire il cammino minimo, una volta trovato, il programma parte dal nodo di arrivo (risultato) e risale la catena dei puntatori padre fino a raggiungere il nodo sorgente, riconoscibile perché il suo campo padre è impostato a NULL.
A questo scopo è stata implementata la funzione `stampa_attori`, che risale ricorsivamente il percorso a ritroso e stampa, nell’ordine corretto, tutti gli attori coinvolti nel cammino sul file di output.
```c
void stampa_attori(elemento *e, attore *array, int dim, FILE *f){
    if (e != NULL){
        stampa_attori(e->padre,array,dim,f);
        attore *trovato = bsearch(&e->attore, array, dim, sizeof(attore), (__compar_fn_t) &ordina);
        fprintf(f,"%d\t%s\t%d\n", trovato->codice, trovato->nome, trovato->anno); // stampo attore
    }
    return;
}
```
# funzionamento del thread gestore di segnali
Il `tgestore` nel programma gestisce i segnali tenendo presente di una varibile di tipo `volatile sig_atomic_t`(per evitare race condition e per evitare ottimizazioni nel controlo di essa) che è contenuta all'internio della struct `segnale`.
```c
typedef struct {
    volatile sig_atomic_t fase; // 0: costruzione grafo, 1: lettura pipe, 2: termina
} segnale;
```
Lo scopo di tale variabile è quello di tenere traccia delle fasi cronologiche del programma durante l'esecuzione.
Inizialmente infatti la variabile, essendo nella fase iniziale, viene inizializzata a 0 e, in questo caso, se arrivasse un `SIGINT` il thread, come da consegna, restituisce `Costruzione del grafo in corso`. 
```c
// NEL THREAD GESTORE
if(s == SIGINT){
   if(stato->fase == 0){
      fprintf(stdout,"Costruzione del grafo in corso\n");
      fflush(stdout);// flusho per scrivere mentre il main esegue
   }
```

prima della creazione della `named pipe` la fase viene cambiata da `0` a `1` facendo `stato.fase = 1`.
```c
stato.fase = 1; // aggiorno la fase del programma
// CREAZIONE DELLA PIPE
```
Dopodiche il main inizia il proprio ciclo di lettura rappresentato da un `while` che contiene nella propria guardia il controllo `stato.fase != 2` 
```c
while(stato.fase != 2){
// LETTURA DALLA PIPE CON CONTROLLI
}
```
Se in questa fase arriva il `SIGINT` il segnale viene gestito dal thread aggiornando il valore a di `stato->fase` a `2` per poi fare break dal ciclo di attesa di `sigwait` e ritornare `NULL` per fare poi la `pthread_join` nel main per farlo terminare definitivamente.
```c
// NEL THREAD GESTORE
else if(stato->fase == 1){
   stato->fase = 2; //terminazione
   break;
}
```
Aggiornando il valore del del campo `fase` di `stato` si esce dal ciclo `while` e quindi si smette di leggere dalla pipe.
Se invece il valore non viene aggiornato durante il ciclo di lettura, inviando il `SIGINT`, allora il programma si manda a se stesso il segnale, con `kill()` per far si che il thread gestore termini.
```c
if(stato.fase != 2){
   kill(getpid(), SIGINT);// mando a me stesso il segnale per la join
}
```
Come da consegna ,dopo che aver aspettato `20s`, vengono deallocate le strutture dati, viene fatta la `pthread_join` e viene distrutta la `named pipe` con la chiamata a `unlink`.
```c
// CODICE MAIN
    xpthread_join(tg, NULL, QUI);
    // Cancellazione della named pipe dal filesystem
    if (unlink(nome) == -1) {
        xtermina("eliminazione pipe fallita", QUI);
    }
    //dealloco array attori
    for(int i = 0; i < messi; i++){
        free(attori[i].nome); // libero nome attore
        free(attori[i].cop); // libero array di coprotagonisti
    }
    free(attori); // libero array di attori
    return 0;
}
```



