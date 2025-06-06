# EsameLab2
Consegna progetto Lab2
# implementazione della coda FIFO nell'algoritmo della BFS
Nel mio programma, la coda FIFO necessaria per la BFS è implementata tramite una lista collegata (linked list) di puntatori a elementi, dove ciascun elemento è una struttura (struct elem, alias elemento) che rappresenta un nodo della coda.
Ogni elemento contiene:

   - attore: il codice numerico dell’attore associato a quel nodo,

   - padre: un puntatore al nodo predecessore lungo il cammino,

   - next: il puntatore al prossimo elemento nella lista.

La coda stessa (struct fifo, alias risolti) mantiene i puntatori alla testa e alla coda della lista, così da permettere efficientemente le operazioni di inserimento in coda (push) e di estrazione dalla testa (pop), come richiesto da una vera FIFO.
# ricostruzione del cammino minimo
Per poter ricostruire il cammino minimo il programma parte, se viene trovato, dal nodo di arrivo <mark>risultato</mark> e da quest'ultimo vengono percorsi i puntatori a padre che risalgono fino al nodo sorgente che è quello che ha il padre a NULL; per questo è stata implementata la funzione <mark>stampa_attori</mark> che ha lo scopo di risalire ricorsivamente il cammino e stampare in ordine tale cmmmino sul file di output.

