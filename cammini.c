#include "libreria.h"
#include "xerrori.h"
#include <sys/times.h>
#define Buf_size 10 // dimensione buffer di lettura
#define QUI __LINE__, __FILE__

void *tgestore(void *v){
    segnale *stato = (segnale *)v;
    fprintf(stdout,"Il pid è: %d\n", getpid());
    sigset_t mask;// creo maschera di nuovo
    sigemptyset(&mask);// riscrivo maschera dei segnali
    sigaddset(&mask,SIGINT);// aggiungo sigint
    int s;// definisco segnale
    while(true){
        int e = sigwait(&mask,&s);// aspetto segnale
        if(e<0){
            xperror(e,"Errore sigwait");// errore ma non termino
        }
        if(s == SIGINT){
            if(stato->fase == 0){
                fprintf(stdout,"Costruzione del grafo in corso\n");
            }
            else if(stato->fase == 1){
                stato->fase = 2; //terminazione
                break;
            }
        }
    }
    pthread_exit(NULL); // termino il thread
}


void *cbody(void *arg){
    clock_t start = times(NULL);
    cdati *cd = (void *)arg;
    int cod1 = cd->coppia[0]; // codice primo attore
    int cod2 = cd->coppia[1]; // codice secondo attore
    int dim = cd->dim; // dimensione dell'array di attori

    //creo file di output
    char *nomefile = NULL;
    int e = asprintf(&nomefile, "%d.%d",cod1, cod2); //creo nome file
    if (e < 0){
        xtermina("Asprintf fallita", QUI);
    }
    FILE *f = fopen(nomefile, "w"); // apro file in scrittura
    if (f == NULL){
        xtermina("Apertura file fallita", QUI);
    }


    attore *array = cd->array; // array di attori
    attore *a1 =bsearch(&cod1,array, dim, sizeof(attore),(__compar_fn_t) &ordina); // cerco primo attore
    if (a1 == NULL){// se primo attore non trovato
        fprintf(f,"codice %d non valido\n", cod1);
        if(fclose(f) != 0){ // chiudo file
            xtermina("Chiusura file fallita", QUI);
        }
        free(cd->coppia); // libero coppia di codici attori
        free(cd); // libero struttura dati
        free(nomefile); // libero nome file
        pthread_exit(NULL); // termino il thread
    }
    attore *a2 = bsearch(&cod2,array, dim, sizeof(attore),(__compar_fn_t) &ordina); // cerco secondo attore
    if (a2 == NULL){// se secondo attore non trovato
        fprintf(f,"codice %d non valido\n", cod2);
        if(fclose(f) != 0){ // chiudo file
            xtermina("Chiusura file fallita", QUI);
        }
        free(cd->coppia); // libero coppia di codici attori
        free(cd); // libero struttura dati
        free(nomefile); // libero nome file
        pthread_exit(NULL); // termino il thread
    }
    
    elemento *e1 = inizializza_elemento(a1->codice); // inizializzo primo attore

    risolti *r = crea_fifo(e1); // creo fifo con primo attore
    root *explored; // inizializzo albero
    explored = crea_abr(shuffle(a1->codice),e1); 
    elemento *risultato = NULL; // risultato bfs
    while(!(is_empty_fifo(r))){ // ciclo finche fifo è ancora piena
        elemento *e = pop_fifo(r); // setraggo primi elemnto dalla fifo
        attore *trovato = bsearch(&e->attore, array, dim, sizeof(attore), (__compar_fn_t) &ordina);
        if(trovato->codice == a2->codice){
            risultato = e;// salvo il risultato come elemento
            break; // se trovato il secondo attore, esco dal ciclo
        }
        for(int i = 0; i< trovato->numcop; i++){// ciclo coprotagonisti
            int codice = trovato->cop[i]; // prendo l'elemento dalla lista di adiacenza
            if (!(cerca_abr(explored,shuffle(codice)))){
                attore *a = bsearch(&codice, array, dim, sizeof(attore), (__compar_fn_t) &ordina); // cerco attore
                if (a == NULL){ // se attore non trovato
                    xtermina("Attore non trovato", QUI);
                }
                elemento *e2 = inizializza_elemento(a->codice); // inizializzo attore
                explored = aggiungi_abr(explored,shuffle(codice),e2); // aggiungo attore come esplorato
                e2->padre = e; // imposto padre dell'attore
                r = push_fifo(r, e2); // inserisco attore nella fifo
            }
        }
    }
    clock_t end = times(NULL); // prendo tempo di fine
    double tempo = (double)(end - start) / sysconf(_SC_CLK_TCK); // calcolo tempo di esecuzione
    if(risultato == NULL && is_empty_fifo(r)){
        fprintf(f,"non esistono cammini da %d a %d\n", cod1, cod2);
        fprintf(stdout,"%d.%d: Nessun cammino. Tempo di esecuzione: %.2f secondi\n", cod1, cod2, tempo);
        if(fclose(f) != 0){ // chiudo file
            xtermina("Chiusura file fallita", QUI);
        }
        free(nomefile); // libero nome file
        free(cd->coppia); // libero coppia di codici attori
        free(cd); // libero struttura dati
        libera_abr(explored); // libero garbage collector
        free(r); // libero fifo
        pthread_exit(NULL); // termino thread
    }else{
        // calcolo la lunghezza del cammino
        int lunghezza = 0; // lunghezza del cammmino
        elemento *e = risultato; // prendo il risultato
        while(e != NULL){ // ciclo fino a quando non arrivo al primo attore
            lunghezza++; // incremento lunghezza
            e = e->padre; // passo al padre dell'attore
        }
        stampa_attori(risultato,array,dim,f);
        fprintf(stdout,"%d.%d: Lunghezza minima %d. Tempo di elaborazione: %.2f secondi\n", cod1, cod2, lunghezza-1, tempo); 
        if(fclose(f) != 0){ // chiudo file
            xtermina("Chiusura file fallita", QUI);
        }
        free(nomefile); // libero nome file
        free(cd->coppia); // libero coppia di codici attori
        free(cd); // libero struttura dati
        libera_abr(explored); // libero garbage collector
        free(r); // libero fifo
        pthread_exit(NULL); // termino thread
    }
}
   


void *tbody(void *arg){
    dati *d = (dati *)arg;
    do{
        xsem_wait(d->sem_data_items, QUI); // attendo un posto libero
        xpthread_mutex_lock(d->mutex, QUI);// blocco mutex per proteggere l'indice
        char *linea = d->buffer [*(d->cindex) % Buf_size]; // leggo la linea dal buffer
        *(d->cindex) += 1; // incremento l'indice del consumatore
        xpthread_mutex_unlock(d->mutex, QUI); // sblocco mutex
        xsem_post(d->sem_free_slots, QUI); // segnalo che c'è un elemento disponibile
        if(linea == NULL){ // se la linea è NULL, significa che il produttore ha terminato
            break; // esco dal ciclo
        }
        char *stato; // stato per strtok_r
        char *s = strtok_r(linea,"\t",&stato); // leggo codice attore
        int codice = atoi(s); //codice da cercare
        attore *a = bsearch(&codice, d->array, d->dim, sizeof(attore), (__compar_fn_t) &ordina);
        if (a == NULL) {
            fprintf(stderr, "Attore codice %d non trovato.\n", codice);
        }
        s = strtok_r(NULL,"\t",&stato); // leggo numero coprotagonisti
        a->numcop = atoi(s); // converto numero coprotagonisti in int
        int capacità = atoi(s); // capacità dell'array di coprotagonisti
        int messi = 0; // numero coaprotagonisti messi nell'array
        a->cop= malloc(capacità * sizeof(int)); // alloco memoria per l'array di coprotagonisti
        if (a->cop == NULL){
            xtermina("Malloc fallita", QUI);
        }
        s = strtok_r(NULL,"\t",&stato); // leggo lista coprotagonisti
        while(s != NULL){
            a->cop[messi] = atoi(s); // converto coprotagonista in int
            messi++; // incremento numero coprotagonisti messi nell'array
            s = strtok_r(NULL,"\t",&stato); // leggo prossimo coprotagonista
        }
        assert(messi == capacità);
        free(linea); // libero la linea letta dal buffer
    }while(true); // ciclo fino a quando non ci sono più linee da leggerea
    pthread_exit(NULL); // termino il thread
}

int main(int argc, char *argv[]){
    if( argc!= 4){
        xtermina ("Uso: cammini filenomi filegrafo numconsumatori", QUI);
    }
    // definizione di gestione segnali
    sigset_t mask;// creo maschera segnali
    sigemptyset(&mask);// inizializzo la maschera
    sigaddset(&mask,SIGINT);// metto nella maschera il segnale da gestire
    pthread_sigmask(SIG_BLOCK,&mask,NULL); // blocco l'arrivo per tutti di SIGINT
    pthread_t tg;
    segnale stato;// dichiare voariabile per gestire comportamento programma
    stato.fase = 0;// inizializzo a fase iniziale
    xpthread_create(&tg,NULL,tgestore,&stato,QUI);  // thread gestore segnali

    FILE *f = xfopen(argv[1],"r", QUI);
    attore *attori;
    int capacità = 20; // numero attori
    int messi = 0; // numero attori messi nell'array
    // alloco memoria per l'array di attori
    attori = malloc(capacità * sizeof(attore));
    if (attori == NULL){
        xtermina("Malloc fallita", QUI);
    }
    //ciclo di lettura file
    // inizializzo buffer di lettura
    char *buffer = NULL;
    size_t n = 0;
    while(true){
        ssize_t e = getline(&buffer, &n, f);
        if (e< 0){
            free(buffer);
            break; // fine file
        }
        if(capacità == messi){
            capacità = 2*capacità; // raddoppio capacità
            attori = realloc(attori, capacità *sizeof(attore));
            if (attori == NULL){
                xtermina("Realloc fallita", QUI);
            }
        }
        char *s = strtok(buffer,"\t"); // leggo codice attore
        attori[messi].codice = atoi(s); // copio nome attore
        s = strtok(NULL,"\t"); // leggo nome attore
        attori[messi].nome = strdup(s); // converto anno in int
        s = strtok(NULL,"\t"); // leggo anno
        attori[messi].anno = atoi(s); // converto anno in int
        messi++; // incremento numero attori messi nell'array
    }
    if(fclose(f) != 0){
        xtermina("Chiusura file fallita", QUI);
    }
    attori = realloc(attori, messi * sizeof(attore));
    if (attori == NULL){
        xtermina("Realloc fallita", QUI);
    }
    //fine ciclo lettura file

    // ordino l'array di attori
    qsort(attori, messi, sizeof(attore), (__compar_fn_t) &ordina);

    // inizio parte lettura grafo.txt
    FILE *f2 = xfopen(argv[2],"r", QUI);
    char *linea = NULL; // linea letta dal file
    size_t nlinea = 0; // dimensione buffer di lettura
    int numcons = atoi(argv[3]); // numero di consumatori
    if (numcons <= 0){
        xtermina("Numero consumatori non valido", QUI);
    }
    sem_t sem_free_slots; // semaforo per i posti liberi
    sem_t sem_data_items; // semaforo per gli elementi disponibili
    int pindex = 0; // indice produttore
    int cindex = 0; // indice consumatore
    char *buff[Buf_size];
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    xsem_init(&sem_free_slots, 0, numcons, QUI); // semaforo per i posti liberi
    xsem_init(&sem_data_items, 0, 0, QUI); // semaforo per gli elementi disponibili
    pthread_t t[numcons]; // array di thread
    dati d[numcons]; // array di dati per i thread

    for (int i = 0; i < numcons; i++){
        d[i].buffer = buff;
        d[i].dim = messi; // dimensione presa da sopra
        d[i].cindex = &cindex; // indice condiviso
        d[i].array = attori; // array di attori
        d[i].mutex = &mu; // mutex per proteggere l'indice
        d[i].sem_free_slots = &sem_free_slots; // semaforo per i posti liberi
        d[i].sem_data_items = &sem_data_items; // semaforo per gli elementi disponibili
        xpthread_create(&t[i], NULL, tbody, d+i, QUI);
    }
    //inizio parte produttore
    while(true){

        ssize_t e = getline(&linea, &nlinea, f2);
        if (e < 0){
            free(linea);
            break; // fine file
        }
        char *copia = strdup(linea); // copio la linea letta
        if (copia == NULL){
            xtermina("Strdup fallita", QUI);
        }
        xsem_wait(&sem_free_slots, QUI); // attendo un posto libero
        buff[pindex ++ % Buf_size] = copia; // copio la linea nel buffer
        xsem_post(&sem_data_items, QUI); // segnalo che c'è un elemento disponibile
    }
    //ciclo di terminazione
    for (int i = 0; i < numcons; i++){
        xsem_wait(&sem_free_slots, QUI); 
        buff[pindex++ % Buf_size] = NULL; // segno la fine del file
        xsem_post(&sem_data_items, QUI); 
    }
    for(int i = 0; i < numcons; i++){
        xpthread_join(t[i], NULL, QUI); // attendo la terminazione dei thread
    }
    // fine parte produttore


    // chiudo i semafori
    xpthread_mutex_destroy(&mu,__LINE__,__FILE__);
    xsem_destroy(&sem_data_items,__LINE__,__FILE__);
    xsem_destroy(&sem_free_slots,__LINE__,__FILE__);
    // chiudo file
    if(fclose(f2) != 0){
        xtermina("Chiusura file grafo fallita", QUI);
    }
    // stampo gli attori


    // inizio parte di creazione pipe
    stato.fase = 1; // aggiorno la fase del programma
    char *nome = "cammini.pipe"; // nome della pipe
    int esito = mkfifo(nome, 0660);
    if(esito == 0){
        fprintf(stderr, "Named pipe creata\n");
    }
    else if(errno == EEXIST){
        fprintf(stderr, "Named pipe già esistente\n");
    }
    else{
        xtermina("Creazione named pipe fallita", QUI);
    }
    // apro  la pipe in lettura
    int fd = open(nome, O_RDONLY);
    if(fd < 0){
        xtermina("Apertura named pipe fallita", QUI);
    }

    while(stato.fase != 2){
        pthread_t t;
        int *letti = malloc(sizeof(int) * 2); // alloco memoria per la coppia di attori
        if (letti == NULL){
            xtermina("Malloc fallita", QUI);
        }
        ssize_t e = read(fd, letti, sizeof(int) * 2);
        if(e < 0){
            xtermina("Lettura named pipe fallita", QUI);
        }
        if(e == 0){
            free(letti); // dealloco qui perché non entro nel thread
            break; // fine pipe
        }
        cdati *d = malloc(sizeof(cdati)); // alloco memoria per i dati del thread
        if (d == NULL){
            xtermina("Malloc fallita", QUI);
        }
        d->array = attori; // array di attori
        d->coppia = letti; // coppia di attori
        d->dim = messi; // dimensione dell'array di attori
        xpthread_create(&t, NULL, cbody, d, QUI); // creo un thread per gestire la lettura
        pthread_detach(t); // lascio lavorare thread senza join
    }
    xclose(fd, QUI); // chiudo la pipe
    sleep(20); // attendo il tempo di esecuzione dei thread
    if(stato.fase != 2){
        kill(getpid(), SIGINT);// mando a me stesso il segnale per la join
    }
    //faccio la join del gestore di segnali
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
