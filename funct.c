#include "funct.h"

//blocca e sblocca mutex
void Pthread_mutex_lock(pthread_mutex_t *mtx) {
    int err;
    if ( (err=pthread_mutex_lock(mtx)) != 0 ) {
        errno=err;
        perror("lock");
        pthread_exit(&errno);
    }
}
void Pthread_mutex_unlock(pthread_mutex_t *mtx) {
    int err;
    if ( (err=pthread_mutex_unlock(mtx)) != 0 ) {
        errno=err;
        perror("unlock");
        pthread_exit(&errno);
    }
}

//conta solo le righe che hanno valori int
int contaRighe(FILE *fp) {
    int righe = 0;
    char b[256];
    while ((fgets(b, sizeof(b), fp)) != NULL ){
        int val;
        if (sscanf(b, "%d", &val)==1) righe++;
    }
    return righe;
}

//converte indirizzi nelle pagine virtuali e stampa errore se l'indirizzo non è valido
void Indirizzi(FILE *fp, int righe, int *pages, char *file) {
    int i=0, ind;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (sscanf(buffer, "%d", &ind) != 1) {
            fprintf(stderr, "Error: indirizzo non valido alla riga %d del file %s\n", i + 1, file);
            continue;
        } else {
            pages[i] = ind / PageDim;
            i++;
        }
    }
    return;
}

//crea nodo
struct node* crea (int pg, int pid) {
    struct node *temp = (struct node *)malloc(sizeof(struct node));
    temp->pagina = pg;
    temp->processo = pid;
    temp->next = NULL;
    return temp;
}

//inserimento in coda
void suf_insert(struct node **ptr, int val, int pid, int frame) {
    struct node *new = crea(val, pid);
    struct node **curr = ptr;
    int i = 0;
    while (*curr != NULL) {
        curr = &((*curr)->next);
        i++;
    }
    *curr = new;            //inserisce il nuovo nodo in fondo

    if (i >= frame) {       //se supera il numero massimo di frame, elimina dalla testa
        pre_delete(ptr);
    }
    return;
}

//elimina sempre il nodo in cima
void pre_delete (struct node** ptr) {
    if (*ptr == NULL) return;
    struct node* current=*ptr;
    *ptr = (*ptr)->next;
    free(current);
    return;
}

//controlla se la pagina è presente in lista, processo e pagina
int controlla (struct node* ptr, int pid, int pg) {
    while ( ptr != NULL) {
        if (ptr->processo==pid && ptr->pagina==pg) return 1;
        ptr=ptr->next;
    }
    return 0;
}

//sposta in coda se la pagina è presente in lista, se sì lo sposta in coda alla lista
int sposta_in_coda(struct node** ptr, int val, int pid) {
    struct node* corrente = *ptr;
    struct node* prec = NULL;

    while (corrente != NULL) {
        if (corrente->pagina == val && corrente->processo == pid) {
            if (prec == NULL) {
                *ptr = corrente->next;
            } else {
                prec->next = corrente->next;
            }

            struct node* tail = *ptr;
            if (tail == NULL) {
                corrente->next = NULL;
                *ptr = corrente;
            } else {
                while (tail->next != NULL) tail = tail->next;
                corrente->next = NULL;
                tail->next = corrente;
            }

            return 1;
        }

        prec = corrente;
        corrente = corrente->next;
    }

    return 0;
}
