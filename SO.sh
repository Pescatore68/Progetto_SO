#!/bin/bash

# Controlla il numero di parametri
if [ $# -ne 2 ]; then
    echo "Uso: $0 <num_frame> <cartella_tracce>"
    exit 1
fi

num_frame="$1"
cartella_tracce="$2"

# Algoritmi supportati
alg_supportati=("LRU" "FIFO")

# Verifica che il file C esista
if [ ! -f main.c ]; then
    echo "Errore: file main.c non trovato."
    exit 1
fi

# Verifica che la cartella esista
if [ ! -d "$cartella_tracce" ]; then
    echo "Errore: cartella $cartella_tracce non trovata."
    exit 1
fi

# Compila
gcc main.c funct.c functSO.c  -o main 
if [ $? -ne 0 ]; then
    echo "Errore nella compilazione."
    exit 1
fi

echo "Compilazione riuscita. Avvio simulazioni..."
echo ""

algoritmi=(FIFO LRU)


traccia=( "$cartella_tracce"/*.txt )

# Controlla se la directory contiene file .txt
if [ "${traccia[0]}" = "$cartella_tracce/*.txt" ]; then
    echo "Errore: nessun file .txt trovato nella cartella '$cartella_tracce'."
    exit 1
fi
    
    # Per ogni algoritmo
    for algoritmo in "${algoritmi[@]}"; do
        echo " Algoritmo: $algoritmo"
	echo ""
            ./main "$num_frame" "$algoritmo" "${traccia[@]}"> "out.tmp"

	hit=0
	fault=0

        # Controlla esito
        if [ $? -eq 0 ]; then
            echo "  → Risultati:"
            cat out.tmp
        else
            echo "  → Errore nell'esecuzione."
        fi

	while read -r line; do
    	case $line in
        	"Pagina hit:"*)
            	hit=${line##*: }  
            	;;
        	"Pagina fault:"*)
            	fault=${line##*: }
            	;;
    	esac
	done < out.tmp

	total=$((hit + fault))

	if [ "$total" -gt 0 ]; then
	    rate=$((100 * fault / total))
	    echo "→ Fault Rate: $rate%"
	fi

        echo
    done

rm -f out.tmp
