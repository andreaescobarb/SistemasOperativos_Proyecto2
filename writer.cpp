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
#include <fstream>
#include <string>

using namespace std;

#define strSize 20
#define tblSize 50

int getIntRandom(int limite1, int limite2);

double getDoubleRandom(int limite1, int limite2);

struct memoriaCompartida
{
  int orden[tblSize];
  int products[tblSize];
  double total[tblSize];
  char client[tblSize][strSize];
  int date[3][tblSize];
  int counter = 0;
  bool checker;
  sem_t mutex[tblSize];
};

memoriaCompartida *memory;

char clients[10][strSize] = {"Dennis", "Luis", "Anderson", "David", "Carlos", "Katy", "Rene", "Cano", "Gerson", "Negro"};

int main()
{
  // int count = 0;
  // std::ifstream file("usuarios.txt");
  // if (file.is_open())
  // {
  //   std::string line;
  //   while (getline(file, line))
  //   {
  //     clients[count][20] = line[1];
  //     printf("%s\n", clients[count]);
  //     count++;
  //   }
  //   count = 0;
  //   file.close();
  // }

  key_t llave = 2630;
  int shmid; //variable que identificara el id de la memoria
  //shmget retorna el identificador de memoria compartida asociada al key
  if ((shmid = shmget(llave, sizeof(struct memoriaCompartida), 0644 | IPC_CREAT)) == -1)
  {
    perror("shmget");
    exit(1);
  }
  //shmget retorna el identificador de memoria compartida asociada al key
  memory = (memoriaCompartida *)shmat(shmid, NULL, 0);
  if (memory == (memoriaCompartida *)(-1))
  {
    perror("shmat");
    exit(1);
  }
  if (!memory->checker)
  {
    for (int i = 0; i < tblSize; i++)
    {
      sem_init(&memory->mutex[i], 1, 1);
    }
    memory->checker = true;
    cout << "SEMAFARO CAMBIADO" << endl
         << endl;
  }
  while (true)
  {
    int option = getIntRandom(2, 0);
    if (option == 0)
    { // Agregando ordenes
      if (memory->counter < tblSize - 1)
      {
        sem_wait(&memory->mutex[memory->counter]);
        cout << " ++++++++++++++++++++++++++++ AGRAGANDO ORDEN ++++++++++++++++++++++++++++" << endl;
        // Agregando cliente
        strcpy(memory->client[memory->counter], clients[getIntRandom(10, 0)]);
        //Agregando numero de orden
        memory->orden[memory->counter] = getIntRandom(2019, 1000);
        // Agregando la cantidad de productos
        memory->products[memory->counter] = getIntRandom(20, 1);
        // Agregando total
        memory->total[memory->counter] = getDoubleRandom(100, 499);
        // Agregando fecha de dia
        memory->date[0][memory->counter] = getIntRandom(30, 1);
        // Agregando fecha de mes
        memory->date[1][memory->counter] = getIntRandom(12, 1);
        // Agregando fecha de ANO
        memory->date[2][memory->counter] = getIntRandom(2019, 2015);
        memory->counter++;
        cout << "*-*-*-*-*-*-*-*-*-*-* ORDEN AGREGADA POR UN ESCRITOR *-*--*-*-*-*-*-*-*-*-*-*-*" << endl
             << endl;
        sleep(3);
        sem_post(&memory->mutex[memory->counter - 1]);
      }
      else
      { // memoria esta llena
        cout << "MEMORIA LLENA... ESPERE" << endl
             << endl;
      }
    }
    else
    { // eliminando ordenes
      if (memory->counter > 0)
      {
        sem_wait(&memory->mutex[memory->counter]);
        memory->counter--;
        cout << "------------------------ ORDEN ELIMINADA POR UN ESCRITOR ---------------------" << endl
             << endl;
        sleep(3);
        sem_post(&memory->mutex[memory->counter + 1]);
      }
    }
    sleep(1);
  }
  return 0;
}

int getIntRandom(int limite1, int limite2)
{
  srand(time(NULL));
  return limite2 + rand() % (limite1 - limite2);
}

double getDoubleRandom(int limite1, int limite2)

{
  srand(time(NULL));
  return ((double)limite1 + (rand() % (limite2 - limite1)));
}