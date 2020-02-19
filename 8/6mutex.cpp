
#include <iostream>
#include <thread>
#include <mutex>

#include <unistd.h>
#include <stdio.h>

using namespace std;

int count = 0;
mutex m;

void increment(const int n) {
    // count += n;
    m.lock();
    for (int i = 0; i < n; ++i) ++count;
    m.unlock();
    sleep(1);
}

void decrement(const int n) {
    m.lock();
    for (int i = 0; i < n; ++i) --count;
    m.unlock(); 
    sleep(1);
}

int main() {
    // int n = 999999999;
    int n = 99999;
    thread th1(increment, n), th2(decrement, n);
    cout << "Proceso principal espera que los hilos terminen\n";
    th1.join();
    th2.join();
    cout << "El hilo principal termina\n";
    cout << count;
    cout << endl;
    exit(0);
}