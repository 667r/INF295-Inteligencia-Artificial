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

    vector<int> granjasNoVisitadasIDs;
    
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
        } else {
            // si no se pudo insertar, marcar como no visitada
            granjasNoVisitadasIDs.push_back(granja.id);
        }        
    }

    return Solucion(rutas, granjasNoVisitadasIDs, inst);
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

// devuelve una nueva solución aplicando un movimiento aleatorio
Solucion Algoritmo::generarVecino(const Solucion& solActual) {
    vector<vector<int>> nuevasRutas = solActual.rutas;
    vector<int> nuevasNoVisitadas = solActual.granjasNoVisitadas;
    

    if (nuevasRutas.empty()) {
        return Solucion(nuevasRutas, nuevasNoVisitadas, inst);
    }

    // elegir un tipo de movimiento aleatorio
    int tipoMovimiento = rand() % 5; // 0, 1, 2, 3, 4

    if (tipoMovimiento == 3 && nuevasNoVisitadas.empty()) {
        tipoMovimiento = 0; // si no hay granjas para añadir, reinserta
    }
    if (tipoMovimiento == 4) {
        bool hayGranjasParaRemover = false;
        for(const auto& r : nuevasRutas) {
            if (!r.empty()) {
                hayGranjasParaRemover = true;
                break;
            }
        }
        if (!hayGranjasParaRemover) {
            tipoMovimiento = 0; // si no hay granjas para remover, reinserta
        }
    }

    if (tipoMovimiento == 0) {
        // reinsertar granja entre rutas
        int k_origen = rand() % nuevasRutas.size();
        size_t intentos = 0;
        while (nuevasRutas[k_origen].empty() && intentos < nuevasRutas.size()) {
            k_origen = (k_origen + 1) % nuevasRutas.size();
            intentos++;
        }
        if (nuevasRutas[k_origen].empty()) return Solucion(nuevasRutas, nuevasNoVisitadas, inst);

        int pos_origen = rand() % nuevasRutas[k_origen].size();
        int granjaId = nuevasRutas[k_origen][pos_origen];
        nuevasRutas[k_origen].erase(nuevasRutas[k_origen].begin() + pos_origen);
        int k_destino = rand() % nuevasRutas.size();
        int pos_destino = nuevasRutas[k_destino].empty() ? 0 : rand() % (nuevasRutas[k_destino].size() + 1);
        nuevasRutas[k_destino].insert(nuevasRutas[k_destino].begin() + pos_destino, granjaId);

    } else if (tipoMovimiento == 1) {
        // swap, intercambia granjar
        int k1 = rand() % nuevasRutas.size();
        size_t intentos = 0;
        while (nuevasRutas[k1].empty() && intentos < nuevasRutas.size()) {
            k1 = (k1 + 1) % nuevasRutas.size();
            intentos++;
        }
        if (nuevasRutas[k1].empty()) return Solucion(nuevasRutas, nuevasNoVisitadas, inst);
        int pos1 = rand() % nuevasRutas[k1].size();
        
        int k2 = rand() % nuevasRutas.size();
        intentos = 0;
        while (nuevasRutas[k2].empty() && intentos < nuevasRutas.size()) {
            k2 = (k2 + 1) % nuevasRutas.size();
            intentos++;
        }
        if (nuevasRutas[k2].empty()) return Solucion(nuevasRutas, nuevasNoVisitadas, inst);
        int pos2 = rand() % nuevasRutas[k2].size();

        swap(nuevasRutas[k1][pos1], nuevasRutas[k2][pos2]);
        
    } else if (tipoMovimiento == 2) {
        // 2 opt (invesrión de segmento de ruta)
        int k = rand() % nuevasRutas.size();
        size_t intentos = 0;
        while (nuevasRutas[k].size() < 2 && intentos < nuevasRutas.size()) {
            k = (k + 1) % nuevasRutas.size();
            intentos++;
        }
        if (nuevasRutas[k].size() < 2) return Solucion(nuevasRutas, nuevasNoVisitadas, inst);

        int rutaSize = nuevasRutas[k].size();
        int i = rand() % (rutaSize - 1);
        int j = (rand() % (rutaSize - 1 - i)) + i + 1;
        std::reverse(nuevasRutas[k].begin() + i, nuevasRutas[k].begin() + j + 1);

    } else if (tipoMovimiento == 3) {
        // añadir granja no visitada
        int idx = rand() % nuevasNoVisitadas.size();
        int granjaId = nuevasNoVisitadas[idx];
        nuevasNoVisitadas.erase(nuevasNoVisitadas.begin() + idx);
        int k_destino = rand() % nuevasRutas.size();
        int pos_destino = nuevasRutas[k_destino].empty() ? 0 : rand() % (nuevasRutas[k_destino].size() + 1);
        nuevasRutas[k_destino].insert(nuevasRutas[k_destino].begin() + pos_destino, granjaId);

    } else { 
        // quitar una granja de la ruta y ponerla en no visitadas
        int k_origen = rand() % nuevasRutas.size();
        size_t intentos = 0;
        while (nuevasRutas[k_origen].empty() && intentos < nuevasRutas.size()) {
            k_origen = (k_origen + 1) % nuevasRutas.size();
            intentos++;
        }
        if (nuevasRutas[k_origen].empty()) return Solucion(nuevasRutas, nuevasNoVisitadas, inst);

        int pos_origen = rand() % nuevasRutas[k_origen].size();
        int granjaId = nuevasRutas[k_origen][pos_origen];
        nuevasRutas[k_origen].erase(nuevasRutas[k_origen].begin() + pos_origen);
        nuevasNoVisitadas.push_back(granjaId);
    }

    // devuelve la nueva solución creada
    return Solucion(nuevasRutas, nuevasNoVisitadas, inst);
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

// función de "energía" para SA, se quiere maximizar el profit.
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
            penalizacion += (carga - inst.capacidadesCamiones[k]) * 15.0; // penalización alta
        }
    }

    // 2. penalización por cuotas
    // (re-evaluar la leche final, ya que 'sol.esFactible' es solo un booleano)
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
            penalizacion += (inst.cuotasLeche[i] - lecheFinalPorTipo[i]) * 100.0; // penalización media
        }
    }

    for (int granjaId : sol.granjasNoVisitadas) {
        // penalización muy alta, proporcional a la cantidad de leche no recogida
        penalizacion += inst.getNodo(granjaId).cantidadLeche * 50.0; 
    }
    
    return profit - penalizacion;
}