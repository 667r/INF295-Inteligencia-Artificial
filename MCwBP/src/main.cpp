#include <iostream>
#include <string>
#include <stdexcept> 
#include <ctime>     
#include <filesystem> 
#include <fstream>    
#include <streambuf>  
#include <functional> 
#include <chrono> // NUEVO: Para medir tiempo

#include "instancia.h" 
#include "algoritmo.h" 
#include "solucion.h"  
#include "graficador.h" 

namespace fs = std::filesystem; 
using namespace std;
using namespace std::chrono; // Para simplificar chrono

int main() {
    const string instanciaDir = "instancias/";
    const string resultsDir = "results/";
    const string graficosDir = "graficos/"; 

    try {
        if (!fs::exists(resultsDir)) fs::create_directory(resultsDir);
        if (!fs::exists(graficosDir)) fs::create_directory(graficosDir);
    } catch (const fs::filesystem_error& e) {
        cerr << "Error creando directorios: " << e.what() << endl;
        return 1;
    }

    double tempInicial = 100000.0;
    double tempFinal = 1.0;
    double tasaEnfriamiento = 0.9999995; 

    for (const auto& entry : fs::directory_iterator(instanciaDir)) {
        const string pathInstancia = entry.path().string();
        const string filename = entry.path().filename().string();

        if (entry.path().extension() != ".txt") continue;

        cout << "Procesando: " << filename << "..." << flush;

        // Variables para medir tiempo total del proceso (Greedy + SA)
        auto inicio_total = high_resolution_clock::now();

        try {
            string pathSalida = resultsDir + filename;
            ofstream outFile(pathSalida);
            auto* coutBuf_original = std::cout.rdbuf(); 
            std::cout.rdbuf(outFile.rdbuf());    

            long seed = time(nullptr) + std::hash<string>{}(filename);
            Instancia instancia(pathInstancia);
            
            cout << "Instancia: " << filename << endl;
            cout << "Nodos: " << instancia.numNodos << ", Camiones: " << instancia.numCamiones << endl;

            Algoritmo algo(instancia, seed);
            
            // 1. Generar y Evaluar Solución Inicial
            Solucion solInicial = algo.generarSolucionInicialGreedy();
            cout << "\n--- Solución Inicial (Greedy) ---" << endl;
            cout << "Profit: " << (long)solInicial.profit << endl;
            cout << "Factible: " << (solInicial.esFactible ? "Si" : "No") << endl;

            // 2. Ejecutar SA (Medimos solo esta parte si quieres precisión algorítmica,
            // pero para el total se suele medir todo el proceso de resolución)
            cout << "\n--- Ejecutando SA ---" << endl;
            Solucion solFinal = algo.ejecutarSimulatedAnnealing(tempInicial, tempFinal, tasaEnfriamiento);
            
            // Medir fin del tiempo
            auto fin_total = high_resolution_clock::now();
            auto duracion = duration_cast<milliseconds>(fin_total - inicio_total);
            double tiempoSegundos = duracion.count() / 1000.0;

            // 3. Reportar Solución Final y TIEMPO
            cout << "\n--- Mejor Solución Final (SA) ---" << endl;
            cout << "Tiempo: " << fixed << setprecision(3) << tiempoSegundos << "s" << endl; // NUEVA LÍNEA
            solFinal.imprimirFormatoSalida(seed, instancia);
            cout << "Factible: " << (solFinal.esFactible ? "Si" : "No") << endl;

            // Guardar CSV de convergencia
            string pathCSV = resultsDir + filename + ".csv";
            ofstream csvFile(pathCSV);
            csvFile << "Iteracion,Profit\n";
            for (const auto& p : algo.historialConvergencia) {
                csvFile << p.first << "," << fixed << setprecision(2) << p.second << "\n";
            }
            csvFile.close();

            string pathSVG = graficosDir + filename + ".svg";
            Graficador::generarSVG(pathCSV, pathSVG);

            std::cout.rdbuf(coutBuf_original);
            outFile.close();
            
            // Mostrar tiempo también en consola
            cout << " -> ¡Listo! (" << tiempoSegundos << "s)" << endl;

        } catch (const exception& e) {
            cerr << " -> ERROR: " << e.what() << endl;
        }
    }

    cout << "\nCompletado. Ejecuta './bin/analisis' en la terminal para generar las estadísticas." << endl;
    return 0;
}