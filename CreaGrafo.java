import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class CreaGrafo{
    public static void main(String[] args){
        if(args.length !=2) {
        System.out.println("Uso: java CreaGrafo name.basics.tsv title.principals.tsv");
        System.exit(1);
        }
        // creazione  lista di attori
        Map<Integer,Attori> attori = new HashMap<Integer,Attori>();
        try{
            BufferedReader br = new BufferedReader(new FileReader(args[0]));
            String Linea;
            String[] controllo;
            br.readLine(); // salta la prima riga (intestazione)
            while((Linea = br.readLine()) != null) {
                // salta linee vuote o commenti
                if(Linea.length() == 0)
                continue;
                controllo = Linea.split("\t");
                if(controllo.length != 6)
                    throw new IllegalArgumentException("Riga con !=6 campi: " + Linea);
                if(controllo[2].trim().equals("\\N")){
                    continue; // salta gli attori senza anno di nascita
                }
                controllo = controllo[4].split(",");
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
                // creo un attore e lo aggiungo all'elenco
                Attori a = new Attori(Linea);
                if(attori.containsKey(a.getcodice())) {
                    System.out.println("Attore duplicato: " + a);
                } else {
                    attori.put(a.getcodice(), a);
                }

            }
            br.close(); // chiudo il file
        }
        catch(Exception e){
            System.err.println("Errore: " + e);
            e.printStackTrace();
            System.exit(2);
        }
        //voglio attori ordinati per codice
        List<Integer> codici = new ArrayList<Integer>(attori.keySet());
        codici.sort(null); // ordino i codici degli attori
        // creazione file nomi.txt
        try{
            BufferedWriter bw = new BufferedWriter(new FileWriter("nomi.txt"));
            for(int cod : codici){
                Attori a = attori.get(cod);
                // scrivo il nome dell'attore nel file
                bw.write(a.toString());
                bw.newLine();
            }
            bw.close(); // chiudo il file
        } catch(Exception e){
            System.err.println("Errore: " + e);
            e.printStackTrace();
            System.exit(3);
        }



        // fine prima parte
        // inizio parte creazione coattori
        Map<Integer,HashSet<Integer>> cast = new HashMap<Integer,HashSet<Integer>>();
        try {
            BufferedReader br = new BufferedReader(new FileReader(args[1]));
            String Linea;
            br.readLine(); // salta la prima riga (intestazione)
            // inizio lettura del file title.principals.tsv
            while((Linea = br.readLine()) != null) {
                // salto linee vuote 
                if(Linea.length() == 0)
                    continue;
                String[] campi = Linea.split("\t");
                if(campi.length != 6)
                    throw new IllegalArgumentException("Riga con !=6 campi: " + Linea);
                String cod = campi[2].trim();
                cod = cod.substring(2); // rimuovo il prefisso nm
                int codice = Integer.parseInt(cod);
                if(!attori.containsKey(codice)) {
                    continue; // salta gli attori non presenti nella lista
                }
                // controllo e vedo se il film è già presente
                String film = campi[0].trim();
                film = film.substring(2); // rimuovo il prefisso tt
                int codicefilm = Integer.parseInt(film);
                if(!cast.containsKey(codicefilm)){
                    Set<Integer> coattori = new HashSet<Integer>();
                    coattori.add(codice); // aggiungo l'attore al set dei coattori
                    cast.put(codicefilm, (HashSet<Integer>) coattori); // creo un nuovo set di coattori per il film
                }
                else {
                    // se il film è già presente mi basta aggiungere l'attore
                    cast.get(codicefilm).add(codice); // aggiungo l'attore al set dei coattori
                }
            }
            br.close(); // chiudo il file
        }
        catch (Exception e) {
            System.err.println("Errore: " + e);
            e.printStackTrace();
            System.exit(4);
        }
        // fine parte creazione cast
        // inizio collegamento coattori
        for (HashSet<Integer> coattori : cast.values()){
            // per ogni film, prendo gli attori e li collego fra loro
            List<Integer> coattoriList = new ArrayList<Integer>(coattori);
            for(int i = 0; i < coattoriList.size(); i++){
                // per ogni attore nel film, collego tutti gli altri attori come coprotagonisti
                for(int j = i + 1; j < coattoriList.size(); j++){
                    int a1 = coattoriList.get(i);
                    int a2 = coattoriList.get(j);
                    // collego a1 e a2 come coprotagonisti
                    attori.get(a1).aggiungiCoprotagonisti(a2); // aggiungo a2 a coprotagonisti di a1
                    attori.get(a2).aggiungiCoprotagonisti(a1); // aggiungo a1 a coprotagonisti di a2
                }
            }
        }
        // fine collegamento coattori
        // inizio scrittura grafo.txt
        try {
            BufferedWriter bf = new BufferedWriter(new FileWriter("grafo.txt"));
            for(int cod : codici) {
                Attori a = attori.get(cod); // prendo l'attore dalla mappa
                // scrivo il codice dell'attore, il numero di coprotagonisti e i coprotagonisti
                int codice = a.getcodice();
                int ncoprotagonisti = a.getnumeroCoprotagonisti();
                Set<Integer> coprotagonisti = a.getCoprotagonisti();
                StringBuilder insieme = new StringBuilder();
                for (int c : coprotagonisti) {
                    if (insieme.length() > 0) insieme.append("\t");
                    insieme.append(c);
                }
                if (coprotagonisti.isEmpty()) {
                    bf.write(String.format("%d\t%d", codice, ncoprotagonisti));
                } else {
                    bf.write(String.format("%d\t%d\t%s", codice, ncoprotagonisti, insieme));
                }
                bf.newLine(); // cambio riga
            }
            bf.close(); // chiudo il file
        }
        catch (Exception e) {
            System.err.println("Errore: " + e);
            e.printStackTrace();
            System.exit(5);
        }
    }
}
