#include "funct.h"


int main(int argc, char *argv[]) {
    int page_fault = 0, page_hit = 0;
    //inizializza un puntatore alla lista che simula le pagine contenute in mem
    struct node *mem = NULL;
    int frame=atoi(argv[1]);
    pthread_t threads[argc-2];

    if (strcmp(argv[2], "FIFO") == 0) {
        for (int i=0; i<argc-3; i++) {
            struct thread_args *args = malloc(sizeof(struct thread_args));
            args->mem = &mem;
            args->argv = argv[3+i];
            args->pgf = &page_fault;
            args->pgh = &page_hit;
            args->frame = frame;
            args->thread = i;
            pthread_create(&threads[i], NULL, FIFO, (void*)args);
        }
    } else if (strcmp(argv[2], "LRU") == 0) {
        for (int i=0; i<argc-3; i++) {
            struct thread_args *args = malloc(sizeof(struct thread_args));
            args->mem = &mem;
            args->argv = argv[3+i];
            args->pgf = &page_fault;
            args->pgh = &page_hit;
            args->frame = frame;
            args->thread = i;
            int rc = pthread_create(&threads[i], NULL, LRU, (void*)args);
            if (rc != 0) {
                fprintf(stderr, "Errore creazione thread %d: %s\n", i, strerror(rc));
                exit(EXIT_FAILURE);
            }
        }
    }
        for (int i = 0; i < argc - 3; i++) {
            int rc = pthread_join(threads[i], NULL);
            if (rc != 0) {
                fprintf(stderr, "Errore join thread %d: %s\n", i, strerror(rc));
            }
        }
        printf("Pagina hit: %d\n", page_hit);
        printf("Pagina fault: %d\n", page_fault);

        return 0;
}
