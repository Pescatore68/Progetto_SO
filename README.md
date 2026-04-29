# Progetto_SO
Progetto di sistemi operativi di Shuhui Chen e Chiara Protopapa

SHUHUI CHEN 		
CHIARA PROTOPAPA	

﻿Progetto Sistemi Operativi: Simulatore di Algoritmi di Paging

1. Descrizione Generale del Progetto
L'obiettivo di questo progetto è sviluppare un simulatore di diversi algoritmi di rimpiazzo delle pagine.
Il progetto prevede l'utilizzo di linguaggio C per la logica di simulazione e di scripting Bash per l'orchestrazione, la compilazione e l'analisi dei risultati.

Struttura del Progetto:
* Programma C:
   * Implementa la struttura della memoria fisica (suddivisa in frame di pagina).
   * Legge file di traccia dove ogni riga contiene un indirizzo virtuale (espresso in byte).
   * Calcola il numero di pagina virtuale a partire dall'indirizzo virtuale, assumendo una dimensione di pagina di 4KB (4096 byte).   
   * Gestisce le richieste di accesso alle pagine virtuali (derivate dagli indirizzi) in ordine sequenziale come appaiono nel file di traccia.
   * Implementa almeno due algoritmi di rimpiazzo delle pagine (e.g., FIFO, LRU, Clock, Ottimale, …). Si consideri una politica di rimpiazzo globale (i page frame sono condivisi tra tutti i processi).
   * Colleziona statistiche come il numero di page fault e page hit.
   * Se vengono implementati algoritmi che utilizzano i bit R (Referenced) e M (Modified) e richiedono un loro reset periodico (e.g., Second Chance), il programma dovrà accettare un parametro per l'intervallo di reset (espresso in numero di riferimenti processati o "tick" logici).
   * Gestione dei Riferimenti:
      * Accetta dalla linea di comando un elenco di uno o più percorsi a file di traccia.
      * Definisce una macro QUANTUM all'interno del codice C. Questo QUANTUM specifica il numero massimo di righe (riferimenti di indirizzo) da processare da ciascun file di traccia prima di passare al file successivo in modalità round-robin.
      * Il programma apre tutti i file di traccia. In un ciclo principale, itera sui file: per file1 processa fino a QUANTUM righe, poi per file2 fino a QUANTUM righe, e così via, tornando ciclicamente al primo file. Questo ciclo continua finché tutti i riferimenti da tutti i file non sono stati processati.
      * Lo stato della memoria fisica (contenuto dei frame) e il contatore di tempo logico persistono attraverso la processazione di questi "quanta" e attraverso i diversi file.
      * Le statistiche vengono aggregate su tutti i riferimenti processati da tutti i file in quella esecuzione.

* Script Bash:
   * Funge da orchestratore per l'intero processo.
   * Verifica la presenza del codice sorgente C e lo compila per generare l'eseguibile, gestendo eventuali errori.
   * Input: Dimensione della RAM (espressa in numero di page frame disponibili), il percorso a una cartella contenente i file di traccia dei processi (ognuno con indirizzi virtuali), e l'eventuale parametro per l'intervallo di reset dei bit R/M per algoritmi specifici.
   * Esecuzione:
      * Lancia l'eseguibile C compilato, fornendogli i parametri necessari (numero di frame, lista dei file traccia, l'algoritmo da usare, ed eventuale intervallo di reset). Esegue il programma C due volte, una per ciascun algoritmo di rimpiazzo.
   * Post-elaborazione:
      * Controlla che il programma C sia terminato correttamente per ogni esecuzione.
      * Cattura l'output (possibili metodi: Il programma C stampa su file; il programma bash cattura lo stdout del programma C) del programma C (che conterrà le statistiche per quel file di traccia e quell'algoritmo).
      * Stampa sullo standard output i risultati.
   * Rimuove eventuali file temporanei creati durante l'esecuzione.


PROGRAMMA IN C 
Main.c
Nel main i parametri inseriti servono a leggere gli argomenti passati da bash.
Dopo aver inizializzato le variabili e allocato al memoria condivisa controlliamo che il numero di argomenti passati al main sia corretto.
Successivamente estraiamo dagli argomenti il numero di frame della memoria condivisa.
Successivamente verifichiamo quale algoritmo è stato scelto e per entrambi gli algoritmi allochiamo la struttura thread_args contenente i 
parametri per i thread e inizializziamo i vari campi con i dati
necessari e verificando che non ci siano errori nella creazione.
successivamente si attende la terminazione di tutti i thread (se dovesse fallire viene stampato un messaggio di errore)
infine stampa il numero di page hit e di page fault totali.


FunctSO.c
In functSo.c abbiamo implementato le due principali funzioni di simulazione degli algoritmi di rimpiazzo delle pagine, cioè la FIFO e la LRU.
Per ognuna delle due funzioni creiamo la struttura dei parametri da passare ai thread, e successivamente estraiamo i parametri necessari da essa. 
Successivamente verifichiamo che non ci siano problemi nell’apertura del file da cui leggere gli indirizzi e in caso contrario stampiamo un messaggio di errore.
Dopo utilizziamo la funzione contaRighe per contare gli indirizzi presenti nel file, e utilizziamo rewind per riportare il puntatore del file all’inizio di esso.
Memorizziamo le pagine lette in una memoria dinamica dopo aver verificato che non ci siano errori nell’allocazione dello spazio di memoria.
Successivamente entriamo nel ciclo for che simula l’accesso alle pagine.

FIFO:
All’interno del for come prima cosa utilizziamo mutex per garantire la mutua esclusione in quanto si sta facendo accesso alla sezione critica.
Per ogni pagina letta, tramite la funzione controlla, verifichiamo se la pagina è già presente in memoria, in caso positivo aumentiamo i page hit, 
in caso negativo aumentiamo i page fault e tramite suf_insert inseriamo la nuova pagina in coda (e se la memoria è già tutta occupata suf_insert elimina la pagina in testa cioè la più vecchia).
Infine rilasciamo il mutex in quanto usciamo dalla sezione critica.

LRU:
All’interno del for come prima cosa utilizziamo mutex per garantire la mutua esclusione in quanto si sta facendo accesso alla sezione critica.
Per ogni pagina letta utilizziamo una variabile found che prende il  valore di sposta_in_coda con parametri mem page e pid.
Il valore di found sarà 1 se la pagina è già presente nella memoria condivisa (page hit) e 0 se non è presente (page fault). 
Inoltre se la pagina viene trovata viene spostata in coda (tramite sposta_in coda), se invece non viene trovata si inserisce la nuova pagina in coda tramite suf_insert 
(che nel caso di memoria già piena eliminerà il nodo in testa).
Infine rilasciamo il mutex in quanto usciamo dalla sezione critica. 

Funct.c
Contiene tutte le funzioni chiamate dalle funzioni FIFO e LRU.

sposta_in_coda: 
scorre la lista, se trova un nodo contenente la pagina richiesta (con lo stesso pid) la rimuove dalla sua posizione corrente e la inserisce in fondo alla lista
Restituisce 1 se la pagina è stata trovata 0 altrimenti.

contaRighe: 
Conta il numero di righe all’interno del file.Tiene in considerazione solo quelle contenenti numeri interi.
 
Indirizzi: 
Converte gli indirizzi logici letti in numero di pagina. Se trova un indirizzo non valido stampa un messaggio di errore

suf_insert: 
inserisce un nuovo nodo in coda alla lista, conta i nodi presnti e se il numero di nodi è maggiore del numero di frame chiama pre_delete per eliminare il nodo in testa

pre_delete: 
Elimina il nodo in testa alla lista e sposta il puntatore sul secondo nodo.

controlla:
scorre la lista e controlla se un nodo è presente in essa, restituisce 1 se lo trova, 0 altrimenti.


SCRIPT IN BASH 
Controlla il numero di parametri passati dal terminale, procede solo se i parametri passati sono due.
Assegna i due argomenti alle variabili:
- num_frame: numero di frame disponibili
- cartella_tracce: percorso della cartella contenente i file di traccia
Verifica la presenza del main.c.
Verifica la presenza della cartella.
Compila i file sorgenti main.c, funct.c e fuctSO.c, creando un eseguibile (main).
Controlla se la compilazione è andata a buon fine, se il main torna valore diverso da 0 allora stampa errore e termina.
Crea un array di traccia passandogli il nome della directory, /*.txt trova tutti i file che hanno formato txt al suo interno.
Controlla se sono presenti file.txt nella directory in caso contrario termina con errore.
Esegue il programma una volta per ogni algoritmo.
Passa al main i parametri: 
- num_frame 
- algoritmo 
- ${traccia[@]} (array contente i path ai file txt).
Salva i risultati in un file temporaneo.
Controlla l’esito dell’esecuzione e stampa a schermo i risultati. 
Estrae da out.tmp i valori di hit e fault, li assegna a due variabili e calcola il fault rate direttamente in Bash.
