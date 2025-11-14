#include <iostream>
#include <string>
#include <stdexcept> // Para excepciones
#include <ctime>     // Para time()
#include "instancia.h"
#include "algoritmo.h"
#include "solucion.h"

using namespace std;

int main(int argc, char* argv[]) {
    // verificar argumentos de línea de comandos
    if (argc < 2) {
        cerr << "Error: Se requiere el path a la instancia." << endl;
        cerr << "Uso: ./bin/mcwb <path_instancia> [seed]" << endl;
        return 1;
    }

    string pathInstancia = argv[1];
    long seed = (argc > 2) ? stol(argv[2]) : time(nullptr); // semilla 

    try {
        // 1. leer Instancia 
        Instancia instancia(pathInstancia);
        cout << "Instancia " << pathInstancia << " leída correctamente." << endl;
        cout << "Nodos: " << instancia.numNodos << ", Camiones: " << instancia.numCamiones << endl;
        
        // inicializar el algoritmo
        Algoritmo algo(instancia, seed);

        // 2. generar solución inicial greedy 
        cout << "\n--- Generando Solución Inicial (Greedy) ---" << endl;
        Solucion solInicial = algo.generarSolucionInicialGreedy();
        
        // 3. evaluar y mostrar solución inicial 
        solInicial.imprimirFormatoSalida(seed, instancia);
        cout << "Factible: " << (solInicial.esFactible ? "Si" : "No") << endl;


    } catch (const exception& e) {
        cerr << "Ocurrió una excepción: " << e.what() << endl;
        return 1;
    }

    return 0;
}