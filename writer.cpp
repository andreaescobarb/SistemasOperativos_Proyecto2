#include <pthread.h>
#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctime>

using namespace std;

#define STRING_SIZE 20
#define TABLE_SIZE 50

int getIntRandom (int range1, int range2) {
  srand(time(NULL));
  return range2 + rand() % (range1 - range2);
}

double getDoubleRandom (int min, int max) {
  srand(time(NULL));
  return ( (double) min + (rand() % (max - min) ));
}

struct sharedMemory {
  int orden[TABLE_SIZE];
  int products[TABLE_SIZE];
  double total[TABLE_SIZE];
  char client[TABLE_SIZE][STRING_SIZE];
  int date [3][TABLE_SIZE] ;
  int counter = 0;
  bool checker;
  sem_t mutex [TABLE_SIZE];
};

sharedMemory *memory;
char clients [5] [STRING_SIZE] = { "Dario", "Arles", "Cristhian", "Harold", "Ana" };

int main () {
  key_t key = 2630;
  //sem_init(&memory->mutex, 1, 1);
  int shmid;
  if ((shmid = shmget(key, sizeof(struct sharedMemory), 0644 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }
  memory = (sharedMemory *)shmat(shmid, NULL, 0);
  if (memory == (sharedMemory *)(-1)) {
    perror("shmat");
    exit(1);
  }
  if (!memory->checker) {
    for (int i = 0; i < TABLE_SIZE; i++) {
      sem_init(&memory->mutex[i], 1, 1);
    }
    memory->checker = true;
    cout << "Ha cambiado los semaforos " << endl << endl;
  }
  while (true) {
    int option = getIntRandom(2, 0);
    if (option == 0) { // Agregando ordenes
      if (memory->counter < TABLE_SIZE - 1) {
        sem_wait(&memory->mutex[memory->counter]);
        cout << "Hola Agregando Orden" << endl;
        // Agregando cliente
        strcpy(memory->client[memory->counter], clients[getIntRandom(5, 0)]);
        //Agregando numero de orden
        memory->orden[memory->counter] = getIntRandom(2019, 1000);
        // Agregando la cantidad de productos
        memory->products[memory->counter] = getIntRandom(20, 1);
        // Agregando total
        memory->total[memory->counter] = getDoubleRandom(100, 499);
        // Agregando fecha
        memory->date[0][memory->counter] = getIntRandom(30,1);
        memory->date[1][memory->counter] = getIntRandom(12,1);
        memory->date[2][memory->counter] = getIntRandom(2019, 2015);
        memory->counter++;
        cout << "Un escritor agrego una orden" << endl << endl;
        sem_post(&memory->mutex[memory->counter - 1]);
      } else { // memoria esta llena
        cout << "Memooria llena." << endl << endl;
      }
    } else { // eliminando ordenes
      if (memory->counter > 0) {
        sem_wait(&memory->mutex[memory->counter]);
        memory->counter--;
        cout << "Un escritor ha eliminado una orden" << endl << endl;
        sem_post(&memory->mutex[memory->counter+1]);
      }
    }
    sleep(1);
  }
  return 0;
}