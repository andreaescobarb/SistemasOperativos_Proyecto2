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

double getDoubleRandom (double min, double max) {
  srand(time(NULL));
  double number = (double) rand();
  return min + number * (max - min);
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
  int shmid;
  if ((shmid = shmget(key, sizeof(struct sharedMemory), 0644)) == -1) {
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
    if (memory->counter > 0) {

      int option = getIntRandom(4, 0);
      if (option == 0) { //Imprimir las ordernes por año
        cout << "Imprimir las ordenes por año" << endl;
        int year = getIntRandom(2019, 2015);
        cout << "Anio: " << year << endl;
        int counter = 0;
        for (int i = 0; i < memory->counter; i++) {
          sem_wait(&memory->mutex[i]);
          if (memory->date[2][i] == year) {
            cout << "Orden: " << memory->orden[i] << endl;
            cout << "Cliente: " << memory->client[i] << endl;
            cout << "Cantidad de productos: " << memory->products[i] << endl;
            cout << "Total: " << memory->total[i] << endl << endl;
            counter++;
          }
          sem_post(&memory->mutex[i]);
        }
        if (counter == 0) {
          cout << "No hay ordenes en esa fecha" << endl << endl;
        }
      } else if (option == 1) { // Imprimiendo todas las ordenes
        cout << "Imprimiendo Todas las ordenes" << endl << endl;
        for (int i = 0; i < memory->counter; i++) {
          sem_wait(&memory->mutex[i]);
          cout << "Orden: " << memory->orden[i] << endl;
          cout << "Cliente: " << memory->client[i] << endl;
          cout << "Fecha: " << memory->date[0][i] << "/" << memory->date[1][i] << "/" << memory->date[2][i] << endl;
          cout << "Cantidad de productos: " << memory->products[i] << endl;
          cout << "Total: " << memory->total[i] << endl << endl;
          sem_post(&memory->mutex[i]);
        }
        cout << "Fin de todas las impresiones" << endl << endl;
      } else if (option == 2) { // Calcular el total de todas las ordenes
        cout << "Suma del total de todas las ordenes" << endl;
        double sum = 0;
        for (int i = 0; i < memory->counter; i++) {
          sem_wait(&memory->mutex[i]);
          sum += memory->total[i];
          sem_post(&memory->mutex[i]);
        }
        cout << "El total de las ordenes es " << sum << endl << endl;
      } else { // Imprimir las ordenes de un cliente
        cout << "Imprimiendo las Ordenes de un Cliente" << endl;
        char client [STRING_SIZE];
        strcpy(client, clients[getIntRandom(5, 0)]);
        cout << "Cliente: " << client << endl;
        int counter = 0;
        for (int i = 0; i <= memory->counter; i++) {
          sem_wait(&memory->mutex[i]);
          if (strcmp(memory->client[i], client)) {
            cout << "Orden: " << memory->orden[i] << endl;
            cout << "Cantidad de productos: " << memory->products[i] << endl;
            cout << "Fecha: " << memory->date[0][i] << "/" << memory->date[1][i] << "/" << memory->date[2][i] << endl;
            cout << "Total: " << memory->total[i] << endl << endl;
            counter++;
          }
          sem_post(&memory->mutex[i]);
        }
        if (counter == 0) {
          cout << "Ese cliente no tiene orden" << endl << endl;
        }
      }

    }
    sleep(1);
  }
  return 0;
}