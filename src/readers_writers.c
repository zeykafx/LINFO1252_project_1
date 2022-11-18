#include "../headers/readers_writers.h"
//
//pthread_mutex_t mutex_readcount; //protège readcount
//pthread_mutex_t mutex_writecount; //protège writecount
//
//sem_t wsem;  // accès exclusif à la db
//sem_t rsem;  // pour bloquer les writers
//
//int readcount=0; // nombre de readers
//int writecount = 0; // nombre de writers
//
//sem_init(&wsem, 0, 1);
//sem_init(&rsem, 0, 1);
//
//
//void writer(void)
//{
//    while(true) {
//        prepare_data();
//
//        pthread_mutex_lock(&mutex_writecount);
//        writecount++;
//        if (writecount == 1) {
//            sem_wait(&rsem)
//        }
//        pthread_mutex_unlock(&mutex_writecount);
//
//        sem_wait(&wsem);
//        write_database();
//        sem_post(&wsem);
//
//        pthread_mutex_lock(&mutex_writecount);
//        writecount--;
//        if (writecount == 0) {
//            sem_post(&rsem)
//        }
//        pthread_mutex_unlock(&mutex_writecount);
//    }
//}
//
//
//void reader(void)
//{
//    while(true) {
//        pthread_mutex_lock(z);
//        sem_wait(&rsem);
//        pthread_mutex_lock(&mutex_readcount);
//        // section critique
//        readcount++;
//        if (readcount==1)
//        { // arrivée du premier reader
//            sem_wait(&wsem);
//        }
//        pthread_mutex_unlock(&mutex_readcount);
//        sem_post(&rsem);
//        pthread_mutex_unlock(z);
//
//        read_database();
//
//        pthread_mutex_lock(&mutex_readcount);
//        // section critique
//        readcount--;
//        if(readcount==0)
//        { // départ du dernier reader
//            sem_post(&wsem);
//        }
//        pthread_mutex_unlock(&mutex_readcount);
//
//        process_data();
//
//    }
//}
