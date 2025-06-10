# definizione del compilatore e dei flag di compilazione
# che vengono usate dalle regole implicite
CC=gcc
CFLAGS=-std=c11 -Wall -g -O3 -pthread
LDLIBS=-lm -lrt -pthread
# Compilatore Java
JAVAC=javac
# File sorgenti
SOURCES = CreaGrafo.java Attori.java

# su https://www.gnu.org/software/make/manual/make.html#Implicit-Rules
# sono elencate le regole implicite e le variabili 
# usate dalle regole implicite 

# Variabili automatiche: https://www.gnu.org/software/make/manual/make.html#Automatic-Variables
# nei comandi associati ad ogni regola:
#  $@ viene sostituito con il nome del target
#  $< viene sostituito con il primo prerequisito
#  $^ viene sostituito con tutti i prerequisiti

# elenco degli eseguibili da creare
EXECS= cammini.out


# primo target: gli eseguibili sono precondizioni del target
# quindi verranno tutti creati
all: $(EXECS) java

java : $(SOURCES)
	$(JAVAC) $^

# regola per la creazione degli eseguibili utilizzando xerrori.o
%.out: %.o libreria.o xerrori.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# regola per la creazione di file oggetto che dipendono da xerrori.h
%.o: %.c libreria.h xerrori.h
	$(CC) $(CFLAGS) -c $<

# Pulizia dei .class
# esempio di target che non corrisponde a una compilazione
# ma esegue la cancellazione dei file oggetto e degli eseguibili
clean: 
	rm -f *.o $(EXECS)
	rm -f *.class
