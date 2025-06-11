# EsameLab2
Consegna progetto Lab2
# parsing delle righe del file di testo name.basics.tsv
Per memorizzare le informazioni prese dal file name.basics.tsv è stata utilizzata una `HashMap`, che viene popolata all’interno della classe `CreaGrafo`. Ad ogni riga letta dal file viene chiamata la funzione `split`, che crea l’array di `controllo` utile per verificare se i campi sono esattamente 6 e per capire, come da indicazioni del testo, quali righe non devono essere considerate.
In particolare:  
nomi che contengono `\N` nel campo data di nascita (birthYear):
```java
if(controllo[2].trim().equals("\\N")){
   continue; // salta gli attori senza anno di nascita
}
```
nomi che non contengono le parole `actor` o `actress` nel quinto campo (primaryProfession):
```java
boolean trovato = false;
// controllo se l'attore è un attore o un'attrice
for(String x : controllo) {
   if(x.trim().equals("actor") || x.trim().equals("actress")) {
      trovato = true; // l'attore è un attore o un'attrice
      break;
   }
}
if(!trovato) {
   continue; // salta gli attori che non sono attori o attrici
}
```
Dopo le opportune verifiche sulle righe lette, viene creata una nuova istanza della classe `Attori`, che contiene, oltre al costruttore principale, anche uno alternativo dedicato al caso in cui si voglia istanziare un nuovo attore a partire da una `String` (la riga letta da file). Qui infatti vengono riempite le variabili che rappresentano l’oggetto.
Viene rifatto il controllo sulla lunghezza della riga splittata e poi vengono riempite le variabili di istanza: `codice`, `nome`, `anno` e, chiaramente, viene dichiarata la collezione `coprotagonisti`, che servirà per raccogliere le informazioni provenienti dal parsing di `title.principals.tsv`.  
Costruttore della classe `Attori`:
```java
public Attori(String a){
   String[] campi = a.split("\t");
   if(campi.length != 6)
      throw new IllegalArgumentException("Riga con !=6 campi: " + a);      
   String cod = campi[0].trim();
   cod = cod.substring(2); // rimuovo il prefisso nm
   this.codice = Integer.parseInt(cod);
   this.nome = campi[1].trim();
   this.anno = Integer.parseInt(campi[2].trim());
   this.coprotagonisti = new HashSet<Integer>();
}
```
Alla fine, se il codice dell'attore appena stanziato, preso con `getcodice()`, non era già presenta nell'HashMap `attori` viene aggiunto alla collezione, come si può vedere da qui:  
```java
Attori a = new Attori(Linea);
if(attori.containsKey(a.getcodice())) {
   System.out.println("Attore duplicato: " + a);
} else {
   attori.put(a.getcodice(), a);
}
```
# implementazione della coda FIFO nell'algoritmo della BFS
Nel programma, la coda FIFO necessaria per la BFS è implementata tramite una lista collegata (linked list) di puntatori a elementi, dove ciascun elemento è una struttura (struct elem, alias elemento) che rappresenta un nodo della coda.
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
Per ricostruire il cammino minimo, una volta trovato, il programma parte dal nodo di arrivo (risultato) e risale la catena dei puntatori padre fino a raggiungere il nodo sorgente, riconoscibile perché il suo campo padre è impostato a `NULL`.
A questo scopo è stata implementata la funzione `stampa_attori` in `libreria.c`, che risale ricorsivamente il percorso a ritroso e stampa, nell’ordine corretto, tutti gli attori coinvolti nel cammino sul file di output.
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
Il `tgestore` nel programma gestisce i segnali facendo uso di una variabile di tipo `volatile sig_atomic_t` (per evitare race condition e ottimizzazioni indesiderate nel controllo del suo valore), contenuta all’interno della struct `segnale` in `libreria.h`:
```c
typedef struct {
    volatile sig_atomic_t fase; // 0: costruzione grafo, 1: lettura pipe, 2: termina
} segnale;
```
Lo scopo di tale variabile è quello di tenere traccia delle fasi cronologiche del programma durante l’esecuzione.
Inizialmente, la variabile viene impostata a 0 (fase iniziale); se in questa fase dovesse arrivare un segnale `SIGINT`, il thread gestore, come da consegna, scrive: `Costruzione del grafo in corso`. 
```c
// NEL THREAD GESTORE
if(s == SIGINT){
   if(stato->fase == 0){
      fprintf(stdout,"Costruzione del grafo in corso\n");
   }
```
prima della creazione della `named pipe`, la fase viene aggiornata da `0` a `1`:
```c
stato.fase = 1; // aggiorno la fase del programma
// CREAZIONE DELLA PIPE
```
Successivamente, il main entra in un ciclo di lettura controllato dalla condizione `stato.fase != 2`: 
```c
while(stato.fase != 2){
// LETTURA DALLA PIPE CON CONTROLLI
}
```
Se durante questa fase arriva un `SIGINT`, il segnale viene intercettato dal thread gestore che aggiorna il valore di `stato->fase` a `2`, effettua il break dal ciclo di attesa su `sigwait`, e ritorna `NULL`, permettendo al main di eseguire la `pthread_join` per la terminazione ordinata:
```c
// NEL THREAD GESTORE
else if(stato->fase == 1){
   stato->fase = 2; //terminazione
   break;
}
```
Aggiornando il valore del campo `fase` di `stato`, si esce dal ciclo `while` e quindi si interrompe la lettura dalla pipe.
Tuttavia, se nessun `SIGINT` viene inviato mentre il programma è nella fase di lettura, una volta usciti dal ciclo while per fine lettura (ovvero quando la pipe viene chiusa dal lato di scrittura), il programma provvede autonomamente a inviare a sé stesso un segnale `SIGINT` tramite `kill()`, in modo da forzare la terminazione del thread gestore e consentire la `pthread_join` finale:
```c
if(stato.fase != 2){
   kill(getpid(), SIGINT);// mando a me stesso il segnale per la join
}
```
Come da consegna ,dopo che aver atteso `20s`, vengono deallocate le strutture dati, viene fatta la `pthread_join` e viene distrutta la `named pipe` con `unlink`:
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

