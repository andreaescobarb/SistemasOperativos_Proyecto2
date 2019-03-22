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
  key_t llave = 2630;
  int shmid;
  if ((shmid = shmget(llave, sizeof(struct memoriaCompartida), 0644)) == -1)
  {
    perror("shmget");
    exit(1);
  }
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
    cout << "SEMAFAROS CAMBIADOS" << endl
         << endl;
  }
  while (true)
  {
    if (memory->counter > 0)
    {

      int vamosnos = getIntRandom(4, 0);
      if (vamosnos == 0)
      { //Imprimir las ordernes por año
        cout << " +++++++++++++++++++++++++++++++ ORDENES POR YEAR ++++++++++++++++++++++++++++++++++++" << endl;
        int year = getIntRandom(2019, 2015);
        cout << "Anio: " << year << endl;
        int counter = 0;
        for (int i = 0; i < memory->counter; i++)
        {
          sem_wait(&memory->mutex[i]);
          if (memory->date[2][i] == year)
          {
            cout << "Orden: " << memory->orden[i] << endl;
            cout << "Cliente: " << memory->client[i] << endl;
            cout << "Cantidad de productos: " << memory->products[i] << endl;
            cout << "Total: " << memory->total[i] << endl
                 << endl;
            counter++;
          }
          sem_post(&memory->mutex[i]);
        }
        if (counter == 0)
        {
          cout << "----------------------- LA FECHA NO POSEE ORDENES ---------------------------------" << endl
               << endl
               << endl
               << endl;
        }
        sleep(3);
      }
      else if (vamosnos == 1)
      { // En este metodo se imprimen todas las ordenes
        //encerrados endos lineas de asteriscos
        cout << " *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* LISTANDO TODAS LAS ORDENES TOTALES -*-*-*-*-*-*-*-*-*-*-*-*" << endl
             << endl;
        for (int i = 0; i < memory->counter; i++)
        {
          sem_wait(&memory->mutex[i]);
          cout << "Orden: " << memory->orden[i] << endl;
          cout << "Cliente: " << memory->client[i] << endl;
          cout << "Fecha: " << memory->date[0][i] << "/" << memory->date[1][i] << "/" << memory->date[2][i] << endl;
          cout << "Cantidad de productos: " << memory->products[i] << endl;
          cout << "Total: " << memory->total[i] << endl
               << endl;
          sem_post(&memory->mutex[i]);
        }
        cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-* FINAL DE IMPRESION *-*-*-*-*-*-*-*-*-*--*-*-*-*-*" << endl
             << endl
             << endl
             << endl;
        sleep(3);
      }

      else if (vamosnos == 2)
      { // Calcular el total de todas las ordenes
        cout << "....................... CALCULANDO EL TOTAL DE ORDENES ....................................." << endl;
        double sum = 0;
        for (int i = 0; i < memory->counter; i++)
        {
          sem_wait(&memory->mutex[i]);
          sum += memory->total[i];
          sem_post(&memory->mutex[i]);
        }
        cout << "....................... TOTAL DE LAS ORDENES ..................................................." << sum << endl
             << endl
             << endl
             << endl;
        sleep(3);
      }
      else
      { // Imprimir las ordenes de un cliente
        cout << "--------------------------------------------- ORDENES POR CLIENTES ----------------------------------" << endl;
        char client[strSize];
        strcpy(client, clients[getIntRandom(10, 0)]);
        printf("+++ cliente: %s ++++", client);
        cout << "" << endl;
        int counter = 0;
        for (int i = 0; i <= memory->counter; i++)
        {
          sem_wait(&memory->mutex[i]);
          if (strcmp(memory->client[i], client))
          {
            cout << "Orden: " << memory->orden[i] << endl;
            cout << "Cantidad de productos: " << memory->products[i] << endl;
            cout << "Fecha: " << memory->date[0][i] << "/" << memory->date[1][i] << "/" << memory->date[2][i] << endl;
            cout << "Total: " << memory->total[i] << endl
                 << endl;
            counter++;
          }
          sem_post(&memory->mutex[i]);
        }
        if (counter == 0)
        {
          cout << " ------------------------------------------ NO HAY ORDENENES PARA ESTE CLIENTE --------------------------" << endl
               << endl;
        }
      }
    }
    sleep(1);
  }
  return 0;
}

int getIntRandom(int range1, int range2)
{
  srand(time(NULL));
  return range2 + rand() % (range1 - range2);
}

double getDoubleRandom(double min, double max)
{
  srand(time(NULL));
  double number = (double)rand();
  return min + number * (max - min);
}