#include "funct.h"

pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

void *FIFO(void* arg){
    struct thread_args* args = (struct thread_args*)arg;
    struct node** mem = args->mem;
    char *argv = args->argv;
    int* pgf = args->pgf;
    int* pgh = args->pgh;
    int frame = args->frame;
    int pid = args->thread;
    FILE *fp = fopen(argv, "r");
    if (fp == NULL) {
        perror("Errore apertura file");
        return NULL;
    }

    int nPages = contaRighe(fp);
    if (nPages==0) {
        fprintf(stderr, "Error: il file %s è vuoto\n", argv);
        fclose(fp);
        return (void*)1;
    }

    int* Pages = (int*)malloc(nPages * sizeof(int));
    if (Pages == NULL) {
        printf("Errore allocazione memoria.\n");
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    Indirizzi(fp, nPages, Pages, argv);
    fclose(fp);

    int pagina;
    for (int i=0; i<nPages; i++) {
        pagina = Pages[i];
        Pthread_mutex_lock(&mem_mutex);
        if (controlla(*mem, pid, pagina)) {
            (*pgh)++;
        } else {
            (*pgf)++;
            suf_insert(mem, pagina, pid, frame);
        }
        Pthread_mutex_unlock(&mem_mutex);
    }
    free(args);
    free(Pages);

    return NULL;
}


void* LRU(void* arg) {
    struct thread_args* args = (struct thread_args*)arg;
    struct node** mem = args->mem;
    char *argv = args->argv;
    int* pgf = args->pgf;
    int* pgh = args->pgh;
    int frame = args->frame;
    int pid = args->thread;

    FILE* fp = fopen(argv, "r");
    if (fp == NULL) {
        perror("Errore apertura file");
        return NULL;
    }

    int nPages = contaRighe(fp);
    rewind(fp);
    if (nPages==0) {
        fprintf(stderr, "Error: il file %s è vuoto\n", argv);
        fclose(fp);
        return (void*)1;
    }

    int* Pages = malloc(nPages * sizeof(int));
    if (Pages == NULL) {
        printf("Errore allocazione memoria.\n");
        fclose(fp);
        return NULL;
    }

    Indirizzi(fp, nPages, Pages, argv);
    fclose(fp);
    for (int i = 0; i < nPages; i++) {
        int page = Pages[i];
            pthread_mutex_lock(&mem_mutex);

        int found = sposta_in_coda(mem, page, pid);
        if (found) {
            (*pgh)++;
        } else {
            (*pgf)++;
            suf_insert(mem, page, pid, frame);
        }


        Pthread_mutex_unlock(&mem_mutex);
    }
    free(args);
    free(Pages);
    return NULL;
}


