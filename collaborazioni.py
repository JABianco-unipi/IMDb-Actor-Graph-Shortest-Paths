#!/usr/bin/env python3
import sys #per leggere da riga d comando

def main(args):
    confronto = list(map(int, args[3:]))
    catalogo = {} #dizionario attore/film
    film = set() # inserimento di attori che hanno partecipato a lo stesso film
    with open(args[1],"r") as f:
        for linea in f:
            film = set()
            a = linea.strip().split("\t") #splitto la linea per ottenere una lista
            if int(a[0]) not in confronto:
                continue
            film.update(a[2:]) # metto tutti i film nel set
            catalogo[int(a[0])] = film.copy() #aggiungo al dizionario tutti i film
    collaborazioni = {}
    for i in range(len(confronto)-1):
        attore1 = confronto[i]   # primo attore
        attore2 = confronto[i+1] # secondo attore
        film1 = catalogo.get(attore1) #prendo il set dei film dell'attore1
        film2 = catalogo.get(attore2) #prendo il set dei film dell'attore2
        trovata = False
        for f in film1:
            if f in film2:
                if (attore1, attore2) not in collaborazioni:
                    collaborazioni[(attore1, attore2)] = {}
                collaborazioni[(attore1, attore2)][int(f)] = None
                trovata = True
        if not trovata:
            collaborazioni[(attore1, attore2)] = None
    with open(args[2],"r") as g:
        next(g)  # salta l'intestazione
        for linea in g:
            b = linea.strip().split("\t")
            for diz in collaborazioni.values():  # cerco la riga con gli attori 
                codice_intero = int(b[0][2:])  # rimuove 'tt' e converte in intero
                if diz is not None and codice_intero in diz:
                    diz[codice_intero] = b[3]  # salva il titolo del film con codice intero
    for (a1,a2) in collaborazioni:
        if collaborazioni[(a1, a2)] == None:
            print(f"{int(a1)}.{int(a2)} nessuna collaborazione")
        else:
            collab = collaborazioni[(a1, a2)]
            print(f"{int(a1)}.{int(a2)}: {len(collab)} collaborazioni:")
            for code, title in sorted(collab.items()): #prendo attributi chiave vaolre
                print(f" {code} {title}")
    return


if len(sys.argv) > 3:
    main(sys.argv)
else:
    print(f"uso: {sys.argv[0]} partecipazioni.txt title.basics.tsv codiciattori")