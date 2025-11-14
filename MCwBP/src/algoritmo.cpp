#include "algoritmo.h"
#include <iostream>
#include <vector>
#include <algorithm> // Para shuffle
#include <random> // Para default_random_engine
#include <limits> // Para DBL_MAX
#include <cmath> // Para exp()

using namespace std;

Algoritmo::Algoritmo(const Instancia& inst, long s) : inst(inst), seed(s) {
    srand(seed); // inicializar generador de números aleatorios
}

Solucion Algoritmo::generarSolucionInicialGreedy() {
    vector<vector<int>> rutas(inst.numCamiones);
    vector<int> cargasActuales(inst.numCamiones, 0);
    
    // copiar granjas para marcarlas como no visitadas
    vector<Nodo> granjasNoVisitadas = inst.granjas;
    
    // mezclar aleatoriamente las granjas para introducir variabilidad
    shuffle(granjasNoVisitadas.begin(), granjasNoVisitadas.end(), default_random_engine(seed));

    for (const auto& granja : granjasNoVisitadas) {
        double mejorCostoInsercion = numeric_limits<double>::max();
        int mejorCamion = -1;
        int mejorPosicion = -1;

        // intentar insertar la granja en la mejor posición posible
        for (int k = 0; k < inst.numCamiones; ++k) {
            // verificar si cabe en el camión
            if (cargasActuales[k] + granja.cantidadLeche > inst.capacidadesCamiones[k]) {
                continue; // no cabe en este camión
            }

            // probar en cada posición de la ruta k (incluyendo el final)
            for (int pos = 0; pos <= rutas[k].size(); ++pos) {
                int nodoPrev = (pos == 0) ? inst.planta.id : rutas[k][pos - 1];
                int nodoSig = (pos == rutas[k].size()) ? inst.planta.id : rutas[k][pos];

                double costoInsercion = inst.getDistancia(nodoPrev, granja.id) +
                                      inst.getDistancia(granja.id, nodoSig) -
                                      inst.getDistancia(nodoPrev, nodoSig);

                if (costoInsercion < mejorCostoInsercion) {
                    mejorCostoInsercion = costoInsercion;
                    mejorCamion = k;
                    mejorPosicion = pos;
                }
            }
        }

        // si se encontró un lugar, insertar la granja
        if (mejorCamion != -1) {
            rutas[mejorCamion].insert(rutas[mejorCamion].begin() + mejorPosicion, granja.id);
            cargasActuales[mejorCamion] += granja.cantidadLeche;
        } 
    }

    return Solucion(rutas, inst);
}


// Simulated Annealing 

Solucion Algoritmo::ejecutarSimulatedAnnealing(double tempInicial, double tempFinal, double tasaEnfriamiento) {
    
    cout << "Iniciando Simulated Annealing..." << endl;
    
    Solucion solActual = generarSolucionInicialGreedy();
    Solucion mejorSolucion = solActual;

    double temperatura = tempInicial;
    double energiaActual = calcularEnergia(solActual);
    double mejorEnergia = energiaActual;

    while (temperatura > tempFinal) {
        // 1. generar un vecino
        Solucion solVecina = generarVecino(solActual);
        double energiaVecina = calcularEnergia(solVecina);

        // 2. decidir si aceptar el vecino
        if (aceptarSolucion(energiaVecina, energiaActual, temperatura)) {
            solActual = solVecina;
            energiaActual = energiaVecina;
        }

        // 3. actualizar la mejor solución encontrada
        if (energiaActual > mejorEnergia) {
            mejorSolucion = solActual;
            mejorEnergia = energiaActual;
        }

        // 4. enfriar (se enfriará cada iteración)
        temperatura *= tasaEnfriamiento;
    }

    cout << "Simulated Annealing terminado." << endl;
    return mejorSolucion;
}

// devuelve una nueva solución aplicando un movimiento 2-Opt 
Solucion Algoritmo::generarVecino(const Solucion& solActual) {
    vector<vector<int>> nuevasRutas = solActual.rutas;

    // si no hay rutas, no se puede hacer nada
    if (nuevasRutas.empty()) {
        return Solucion(nuevasRutas, inst);
    }

    // 2opt

    // 1. elegir una ruta al azar (k) que tenga al menos 2 granjas.
    int k = rand() % nuevasRutas.size();
    int intentos = 0;
    while (nuevasRutas[k].size() < 2 && intentos < nuevasRutas.size()) {
        k = (k + 1) % nuevasRutas.size(); // Probar la siguiente ruta
        intentos++;
    }

    // si no se encontró ninguna ruta válida, devolver la copia sin cambios.
    if (nuevasRutas[k].size() < 2) {
        return Solucion(nuevasRutas, inst);
    }

    // 2. elegir dos índices aleatorios (i, j) en la ruta k, asegurando que i < j.
    int rutaSize = nuevasRutas[k].size();
    
    int i = rand() % (rutaSize - 1); // indice 1: de 0 a (tamaño - 2)
    int j = (rand() % (rutaSize - 1 - i)) + i + 1; // indice 2: de (i + 1) a (tamaño - 1)

    // 3. invertir el segmento de la ruta entre i y j (inclusive).
    // reverse(inicio, fin) -> 'fin' es exclusivo.
    // queremos invertir desde el iterador de 'i' hasta el iterador *después* de 'j'.
    reverse(nuevasRutas[k].begin() + i, nuevasRutas[k].begin() + j + 1);

    // 4. devolver la nueva solución vecina
    return Solucion(nuevasRutas, inst);
}

// criterio de aceptación para SA
bool Algoritmo::aceptarSolucion(double nuevoProfit, double profitActual, double temperatura) {
    // si la nueva solución es mejor, siempre se acepta
    if (nuevoProfit > profitActual) {
        return true;
    }
    // si es peor, se acepta con una probabilidad
    double prob = exp((nuevoProfit - profitActual) / temperatura);
    return (double)rand() / RAND_MAX < prob;
}

// función de "Energía" para SA, queremos maximizar el profit.
// incluye penalizaciones por infactibilidad.
double Algoritmo::calcularEnergia(const Solucion& sol) {
    double profit = sol.profit;
    double penalizacion = 0.0;

    // 1. penalización por capacidad
    for (int k = 0; k < sol.rutas.size(); ++k) {
        int carga = 0;
        for (int granjaId : sol.rutas[k]) {
            carga += inst.getNodo(granjaId).cantidadLeche;
        }
        if (carga > inst.capacidadesCamiones[k]) {
            penalizacion += (carga - inst.capacidadesCamiones[k]) * 100.0; // Penalización alta
        }
    }

    // 2. penalización por cuotas
    // (Re-evaluar la leche final, ya que 'sol.esFactible' es solo un booleano)
    vector<int> lecheFinalPorTipo(inst.numTiposLeche, 0);
    for (const auto& ruta : sol.rutas) {
        if(ruta.empty()) continue;
        char calidadMasBaja = 'A';
        int lecheTotalEnCamion = 0;
        for (int granjaId : ruta) {
            const Nodo& granja = inst.getNodo(granjaId);
            lecheTotalEnCamion += granja.cantidadLeche;
            if (granja.tipoLeche > calidadMasBaja) calidadMasBaja = granja.tipoLeche;
        }
        lecheFinalPorTipo[inst.tipoLecheToIndex.at(calidadMasBaja)] += lecheTotalEnCamion;
    }

    for (int i = 0; i < inst.numTiposLeche; ++i) {
        if (lecheFinalPorTipo[i] < inst.cuotasLeche[i]) {
            penalizacion += (inst.cuotasLeche[i] - lecheFinalPorTipo[i]) * 10.0; // penalización media
        }
    }
    
    return profit - penalizacion;
}