#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <stack>
#include <set>

// Criar uma memoria compartilhada 
// criar um semaforo 
// criar uma pilha 
//criar uma arvore 
//***********************
// criar função pra gerar numeros aleatorios
//criar função para verificar se é primo
//criar função para inserir na pilha
//criar função para inserir na arvore


#define SHARED_MEMORY_NAME "/shared_memory"
#define SEMAPHORE_NAME "/shared_semaphore"
#define STACK_SIZE 10 // definindo tamanho da pilha

class SharedData
{
public:
    int numbers[STACK_SIZE];
    sem_t semaphore;
    SharedData()
    {
        sem_init(&semaphore, 1, 1);
    }
    ~SharedData()
    {
        sem_destroy(&semaphore);
    }
};

void inserir(SharedData *shared, int value)
{
    sem_wait(&shared->semaphore);
    for (int i = 0; i < STACK_SIZE; ++i)
    {
        if (shared->numbers[i] == 0)
        {
            shared->numbers[i] = value;
            sem_post(&shared->semaphore);
            return;
        }
    }
    sem_post(&shared->semaphore);
    std::cout << "A pilha está cheia, não é possível inserir " << value << std::endl;
}

int retirar(SharedData *shared)
{
    sem_wait(&shared->semaphore);
    for (int i = STACK_SIZE - 1; i >= 0; --i)
    {
        if (shared->numbers[i] != 0)
        {
            int value = shared->numbers[i];
            shared->numbers[i] = 0;
            sem_post(&shared->semaphore);
            return value;
        }
    }
    sem_post(&shared->semaphore);
    std::cout << "A pilha está vazia" << std::endl;
    return -1; // Retorna um valor inválido se a pilha estiver vazia
}

bool ePrimo(int x)
{
    if (x <= 1)
    {
        return false; // Números menores ou iguais a 1 não são primos
    }
    for (int i = 2; i * i <= x; ++i)
    {
        if (x % i == 0)
        {
            return false;
        }
    }
    return true;
}

void produtor(SharedData *shared)
{
    srand(time(nullptr));
    while (true)
    {
        int value = rand() % 1000;
        if (ePrimo(value))
        {
            // se for , coloca na pilha
            inserir(shared, value);
            std::cout << "colocou na pilha: " << value << std::endl;
            usleep(800000); //esperar
        }
    }
}

// tem que consumir na pilha e colocar na arvore
void consumidor(std::set<int> *tree, SharedData *shared)
{
    while (true)
    {
        sem_wait(&shared->semaphore);
        if (shared->numbers[0] != 0)
        {
            int value = shared->numbers[0];
            shared->numbers[0] = 0;
            sem_post(&shared->semaphore);
            tree->insert(value);
            std::cout << "inserido na árvore: " << value << std::endl;
        }
        else
        {
            sem_post(&shared->semaphore);
            std::cout << "Não foi possivel acessar agr" << std::endl;
        }
        usleep(800000); // Espera por 800 milissegundos
    }
}

int main()
{
    SharedData *shared = new SharedData();
    std::set<int> *tree = new std::set<int>();

    // Criando threads para produtor e consumidor
    std::thread produtor_thread(produtor, shared); // criara numero , verirficar primo inserir na pilha
    std::thread consumidor_thread(consumidor, tree, shared); // consumir da pilha e inserir na arvore

    produtor_thread.join();
    consumidor_thread.join();

    delete shared;
    delete tree;

    return 0;
}
