
import java.util.Set;
import java.util.TreeSet;
import java.util.HashSet;


public class Attori{
int codice;                   // codice univoco dell'attore
String nome;                  // none (e cognome) dell'attore
int anno;                     // anno di nascita
Set<Integer> coprotagonisti;  // codici degli attori che hanno recitato con this 
     public Attori(int cod, String n, int a){
          this.codice=cod;
          this.nome=n;
          this.anno=a;
          this.coprotagonisti = new HashSet<Integer>();
     }
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
     public void aggiungiCoprotagonisti(int a){
          if(a == this.codice) {
               return; // non collego l'attore a se stesso
          }
          this.coprotagonisti.add(a);
     }
     public int getcodice(){
          return this.codice;
     }
     public int getnumeroCoprotagonisti(){
          return this.coprotagonisti.size();
     }
     public Set<Integer> getCoprotagonisti(){
          // ritorna un set ordinato dei coprotagonisti
          Set<Integer> sortedCoprotagonisti = new TreeSet<Integer>(this.coprotagonisti);
          return sortedCoprotagonisti;
     }
     @Override
     public boolean equals(Object o){
          if(o instanceof Attori){
               Attori a = (Attori) o;
               return this.codice == a.getcodice();
          }
          return false;
     }
     @Override
     public String toString() {
          return String.format("%d\t%s\t%d", codice, nome, anno);
     }
}
