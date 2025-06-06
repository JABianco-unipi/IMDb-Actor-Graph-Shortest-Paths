# EsameLab2
Consegna progetto Lab2
# implementazione della coda FIFO nell'algoritmo della BFS
Nel mio programma, la coda FIFO necessaria per la BFS è implementata tramite una lista collegata (linked list) di puntatori a elementi, dove ciascun elemento è una struttura (struct elem, alias elemento) che rappresenta un nodo della coda.
Ogni elemento contiene:

   - attore: il codice numerico dell’attore associato a quel nodo,

   - padre: un puntatore al nodo predecessore lungo il cammino,

   - next: il puntatore al prossimo elemento nella lista.

typedef struct elem{
    struct elem *padre; // padre dell'elemento
    int attore; // attore contenuto nell'elemento
    struct elem *next; // prossimo elemento nella lista
}elemento;

typedef struct fifo{
    elemento *testa; // prossimo elemento nella lista
    elemento *coda; // ultimo elemento nella lista
}risolti;

La coda stessa (struct fifo, alias risolti) mantiene i puntatori alla testa e alla coda della lista, così da permettere efficientemente le operazioni di inserimento in coda (push) e di estrazione dalla testa (pop), come richiesto da una vera FIFO.
# ricostruzione del cammino minimo
Per ricostruire il cammino minimo, una volta trovato, il programma parte dal nodo di arrivo (risultato) e risale la catena dei puntatori padre fino a raggiungere il nodo sorgente, riconoscibile perché il suo campo padre è impostato a NULL.
A questo scopo è stata implementata la funzione stampa_attori, che risale ricorsivamente il percorso a ritroso e stampa, nell’ordine corretto, tutti gli attori coinvolti nel cammino sul file di output.

