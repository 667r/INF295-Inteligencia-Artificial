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

        // 1. definir los parámetros de SA (puedes ajustarlos)
        double tempInicial = 100000.0;
        double tempFinal = 1.0;
        double tasaEnfriamiento = 0.9999;

        cout << "\n--- Ejecutando Simulated Annealing ---" << endl;
        cout << "Temp. Inicial: " << tempInicial << ", Temp. Final: " << tempFinal << ", Tasa: " << tasaEnfriamiento << endl;

        // 2. ejecutar el algoritmo
        Solucion solFinal = algo.ejecutarSimulatedAnnealing(tempInicial, tempFinal, tasaEnfriamiento);
        
        // 3. evaluar y mostrar la mejor solución encontrada
        cout << "\n--- Mejor Solución Encontrada (SA) ---" << endl;
        solFinal.imprimirFormatoSalida(seed, instancia);
        cout << "Factible: " << (solFinal.esFactible ? "Si" : "No") << endl;



    } catch (const exception& e) {
        cerr << "Ocurrió una excepción: " << e.what() << endl;
        return 1;
    }

    return 0;
}