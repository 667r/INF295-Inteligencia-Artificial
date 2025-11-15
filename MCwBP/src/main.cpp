#include <iostream>
#include <string>
#include <stdexcept> // Para excepciones
#include <ctime>     // Para time()
#include <filesystem>
#include <fstream>    
#include <streambuf>  
#include <functional>
#include "instancia.h"
#include "algoritmo.h"
#include "solucion.h"

namespace fs = std::filesystem;
using namespace std;

int main() {
const string instanciaDir = "instancias/";
    const string resultsDir = "results/";

    // asegurarse que la carpeta results/ existe
    try {
        if (!fs::exists(resultsDir)) {
            fs::create_directory(resultsDir);
            cout << "Carpeta 'results/' creada." << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error creando carpeta 'results': " << e.what() << endl;
        return 1;
    }

    // parámetros de SA
    double tempInicial = 100000.0;
    double tempFinal = 1.0;
    double tasaEnfriamiento = 0.9999; // itera al rededor de 230000 veces por instancia

    // iterar sobre todos los archivos en el directorio de instancias
    for (const auto& entry : fs::directory_iterator(instanciaDir)) {
        const string pathInstancia = entry.path().string();
        const string filename = entry.path().filename().string();

        // saltar archivos que no sean .txt
        if (entry.path().extension() != ".txt") {
            continue;
        }

        // mostrar progreso en la consola
        cout << "Procesando: " << filename << "..." << flush;

        try {
            // redirección a archivo de salida

            string pathSalida = resultsDir + filename;
            ofstream outFile(pathSalida);
            auto* coutBuf_original = std::cout.rdbuf(); // guardar buffer original de cout
            cout.rdbuf(outFile.rdbuf());    // redirigir cout al archivo

            // algoritmo 
            
            // generar una seed única por instancia
            long seed = time(nullptr) + hash<string>{}(filename);
            
            Instancia instancia(pathInstancia);
            
            // imprimir info de la instancia (al archivo)
            cout << "Instancia " << pathInstancia << " leída correctamente." << endl;
            cout << "Nodos: " << instancia.numNodos << ", Camiones: " << instancia.numCamiones << endl;

            Algoritmo algo(instancia, seed);

            cout << "\n--- Ejecutando Simulated Annealing ---" << endl;
            cout << "Temp. Inicial: " << tempInicial << ", Temp. Final: " << tempFinal << ", Tasa: " << tasaEnfriamiento << endl;

            Solucion solFinal = algo.ejecutarSimulatedAnnealing(tempInicial, tempFinal, tasaEnfriamiento);
            
            cout << "\n--- Mejor Solución Encontrada (SA) ---" << endl;
            solFinal.imprimirFormatoSalida(seed, instancia);
            cout << "Factible: " << (solFinal.esFactible ? "Si" : "No") << endl;

            // restaura el cout
            std::cout.rdbuf(coutBuf_original);
            outFile.close();
            
            // confirmar en la consola que se terminó
            cout << " -> ¡Listo! Guardado en " << pathSalida << endl;

        } catch (const exception& e) {
            // Si algo falla, restaurar cout y reportar en consola
            cerr << " -> ERROR procesando " << filename << ": " << e.what() << endl;
        }
    }

    cout << "\nProceso completado. Todos los resultados están en la carpeta 'results/'." << endl;
    return 0;
}