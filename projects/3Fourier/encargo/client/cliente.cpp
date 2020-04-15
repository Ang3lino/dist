#include "Solicitud.h"

#include <iostream>
#include <vector>

#include <cstring>
#include <string>
#define _USE_MATH_DEFINES  // use M_PI
#include <math.h>

using namespace std;


int puerto = 7200;
int periodo = 64;
char *ip = "127.0.0.1";

template<typename T>
std::vector<double> linspace(T start_in, T end_in, int num_in) {
    std::vector<double> linspaced;
    double start = static_cast<double>(start_in);
    double end = static_cast<double>(end_in);
    double num = static_cast<double>(num_in);
    if (num == 0) { return linspaced; }
    if (num == 1) {
        linspaced.push_back(start);
        return linspaced;
    }
    double delta = (end - start) / (num - 1);
    for (int i = 0; i < num-1; ++i) {
          linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end); 
    return linspaced;
}

const int L = periodo / 2;
// T = 64, A = 16, d = 32
double coeficiente_fourier(double x, unsigned n) {
    const double cte = -4*L / (M_PI * M_PI);
    return cte * cos((2*n + 1)*M_PI*x / L) / ((2*n + 1)*(2*n + 1));
    // return (64 / M_PI) * sin((M_PI * x / 32)*(2*n - 1));
}


void iniciar_x(double *dst, int n) {
    auto src = linspace(-periodo / 2, periodo + periodo / 2, n);
    copy(src.begin(), src.end(), dst);
}

void mostrar_arreglo(double *arr, int n) {
    cout << "[ " ;
    for (int i = 0; i < n; ++i) {
        cout << arr[i];
        cout << ", ";
    }
    cout << " ]" << endl;
}

int main(int argc, char *argv[]) {
    int cantidad_muestras = TAM_MAX_DATA / 8;
    double x[cantidad_muestras], y[cantidad_muestras];
    int n, muestrasPorEnviar = 10000;  
    Solicitud solicitud;

    // inicia los vectores discretos x, y
    iniciar_x(x, cantidad_muestras);
    fill_n(y, cantidad_muestras, L/2); 

    solicitud.doOperation(ip, puerto, ENVIAR_X, (char *) &x[0]);
    for (n = 0; n <= muestrasPorEnviar; ++n) {
        for (int i = 0; i < cantidad_muestras; ++i) y[i] += coeficiente_fourier(x[i], n); 
        solicitud.doOperation(ip, puerto, ENVIAR_Y, (char *) &y[0]);
    }
    mostrar_arreglo(y, cantidad_muestras);

    return 0;
}
