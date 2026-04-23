#ifndef FUNCT_H
#define FUNCT_H
#endif FUNCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#define PageDim 4096

struct node {
    int pagina;
    int processo;
    struct node *next;
};

struct thread_args {
    struct node** mem;
    char *argv;
    int *pgf;
    int *pgh;
    int frame;
    int thread;
};

void Pthread_mutex_lock(pthread_mutex_t *mtx);
void Pthread_mutex_unlock(pthread_mutex_t *mtx);

void suf_insert(struct node **ptr, int val, int pid, int frame);
int controlla (struct node* ptr, int pid, int pg);
void pre_delete (struct node** ptr);
int sposta_in_coda(struct node** ptr, int val, int pid);

int contaRighe(FILE *fp);
void Indirizzi(FILE *fp, int righe, int *pages, char *file);

void *FIFO(void* arg);
void *LRU(void* arg);