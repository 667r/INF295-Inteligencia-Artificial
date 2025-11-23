#include <iostream>
#include <string>
#include <stdexcept> 
#include <ctime>     
#include <filesystem> 
#include <fstream>    
#include <streambuf>  
#include <functional> 
#include <chrono>

#include "instancia.h" 
#include "algoritmo.h" 
#include "solucion.h"  
#include "graficador.h" 

namespace fs = std::filesystem; 
using namespace std;
using namespace std::chrono;

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

        // variables para medir tiempo total del proceso (Greedy + SA)
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
            
            // 1. generar y evaluar solución inicial
            Solucion solInicial = algo.generarSolucionInicialGreedy();
            cout << "\n--- Solución Inicial (Greedy) ---" << endl;
            cout << "Profit: " << (long)solInicial.profit << endl;
            cout << "Factible: " << (solInicial.esFactible ? "Si" : "No") << endl;

            // 2. ejecutar SA 
            cout << "\n--- Ejecutando SA ---" << endl;
            Solucion solFinal = algo.ejecutarSimulatedAnnealing(tempInicial, tempFinal, tasaEnfriamiento);
            
            // medir fin del tiempo
            auto fin_total = high_resolution_clock::now();
            auto duracion = duration_cast<milliseconds>(fin_total - inicio_total);
            double tiempoSegundos = duracion.count() / 1000.0;

            // 3. resultados finales y tiempos
            cout << "\n--- Mejor Solución Final (SA) ---" << endl;
            cout << "Tiempo: " << fixed << setprecision(3) << tiempoSegundos << "s" << endl;
            solFinal.imprimirFormatoSalida(seed, instancia);
            cout << "Factible: " << (solFinal.esFactible ? "Si" : "No") << endl;

            // guardado datos de convergencia para graficar
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
            
            // mostrar mensaje de completado con tiempo
            cout << " -> ¡Listo! (" << tiempoSegundos << "s)" << endl;

        } catch (const exception& e) {
            cerr << " -> ERROR: " << e.what() << endl;
        }
    }

    cout << "\nCompletado. Ejecuta './bin/analisis' en la terminal para generar las estadísticas." << endl;
    return 0;
}