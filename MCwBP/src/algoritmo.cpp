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
    
    // 1. Copiar granjas
    vector<Nodo> granjasCandidatas = inst.granjas;
    vector<int> granjasNoVisitadasIDs;
    
    // 2. ORDENAMIENTO HEURÍSTICO (La clave para la robustez)
    // Ordenamos las granjas para procesar las más "críticas" primero.
    sort(granjasCandidatas.begin(), granjasCandidatas.end(), [](const Nodo& a, const Nodo& b) {
        // Criterio 1: Calidad de leche (Ascendente: 'A' < 'B' < 'C')
        // Priorizamos cargar la leche de mejor calidad antes para asegurar cuotas 
        // y evitar que la leche mala ocupe espacio prematuramente.
        if (a.tipoLeche != b.tipoLeche) {
            return a.tipoLeche < b.tipoLeche; 
        }
        
        // Criterio 2: Cantidad de leche (Descendente)
        // Es más difícil ubicar granjas grandes al final. Las metemos primero.
        return a.cantidadLeche > b.cantidadLeche;
    });

    // 3. Inserción Greedy (Cheapest Insertion)
    for (const auto& granja : granjasCandidatas) {
        double mejorCostoInsercion = numeric_limits<double>::max();
        int mejorCamion = -1;
        int mejorPosicion = -1;

        // Intentar insertar la granja en el camión que minimice el incremento de distancia
        for (int k = 0; k < inst.numCamiones; ++k) {
            
            // Restricción Dura: Capacidad del camión
            if (cargasActuales[k] + granja.cantidadLeche > inst.capacidadesCamiones[k]) {
                continue; 
            }

            // Probar todas las posiciones posibles en la ruta k
            for (int pos = 0; pos <= rutas[k].size(); ++pos) {
                int nodoPrev = (pos == 0) ? inst.planta.id : rutas[k][pos - 1];
                int nodoSig = (pos == rutas[k].size()) ? inst.planta.id : rutas[k][pos];

                double costoInsercion = inst.getDistancia(nodoPrev, granja.id) +
                                      inst.getDistancia(granja.id, nodoSig) -
                                      inst.getDistancia(nodoPrev, nodoSig);

                // Si encontramos una posición válida con menor costo, la guardamos
                if (costoInsercion < mejorCostoInsercion) {
                    mejorCostoInsercion = costoInsercion;
                    mejorCamion = k;
                    mejorPosicion = pos;
                }
            }
        }

        // 4. Asignación
        if (mejorCamion != -1) {
            rutas[mejorCamion].insert(rutas[mejorCamion].begin() + mejorPosicion, granja.id);
            cargasActuales[mejorCamion] += granja.cantidadLeche;
        } else {
            // Si llegamos aquí, es porque la instancia está muy ajustada de capacidad
            // y la heurística no logró encajarla.
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

        if (fmod(temperatura, 100) == 0) {
            // Guardamos el Profit de la MEJOR solución hasta el momento, o la actual.
            // Generalmente se grafica la "Mejor encontrada" para ver la meseta.
            historialConvergencia.push_back({temperatura, mejorSolucion.profit});
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

    // Si no hay rutas y tampoco no visitados, no se puede hacer nada
    if (nuevasRutas.empty() && nuevasNoVisitadas.empty()) return solActual;

    // Elegir entre Intra-Ruta (0) o Inter-Ruta (1)
    int tipoMovimiento = rand() % 2;

    // --- MOVIMIENTO 1: INTRA-RUTA (2-OPT) ---
    // Mejora la calidad de una ruta individual invirtiendo un segmento.
    if (tipoMovimiento == 0) {
        // Elegir una ruta aleatoria que tenga al menos 2 nodos
        int k = rand() % nuevasRutas.size();
        size_t intentos = 0;
        while (nuevasRutas[k].size() < 2 && intentos < 10) {
            k = rand() % nuevasRutas.size();
            intentos++;
        }
        
        if (nuevasRutas[k].size() >= 2) {
            int i = rand() % (nuevasRutas[k].size() - 1);
            int j = (rand() % (nuevasRutas[k].size() - 1 - i)) + i + 1;
            std::reverse(nuevasRutas[k].begin() + i, nuevasRutas[k].begin() + j + 1);
        }
    } 
    // --- MOVIMIENTO 2: INTER-RUTA (SHIFT GENERALIZADO: Move/Add/Drop) ---
    // Mueve una granja de [Ruta X o NoVisitados] a [Ruta Y o NoVisitados]
    else {
        // 1. Seleccionar ORIGEN (¿De dónde sacamos la granja?)
        // -1 representa la lista "No Visitados", 0 a N-1 son las rutas.
        int origenIdx = -1;
        
        // Probabilidad de sacar de "No Visitados" vs "Rutas"
        bool sacarDeNoVisitados = (!nuevasNoVisitadas.empty()) && (rand() % 2 == 0);
        
        if (!sacarDeNoVisitados) {
            // Sacar de una ruta existente
            origenIdx = rand() % nuevasRutas.size();
            // Buscar una ruta que no esté vacía
            size_t intentos = 0;
            while (nuevasRutas[origenIdx].empty() && intentos < 10) {
                origenIdx = rand() % nuevasRutas.size();
                intentos++;
            }
            if (nuevasRutas[origenIdx].empty()) return solActual; // No hay nada que mover
        }

        // 2. Seleccionar DESTINO (¿A dónde va la granja?)
        int destinoIdx = -1;
        
        // No permitimos mover de "No Visitados" a "No Visitados"
        if (origenIdx == -1) {
            destinoIdx = rand() % nuevasRutas.size(); // Debe ir a una ruta
        } else {
            // Puede ir a otra ruta o a "No Visitados"
            // Damos alta probabilidad a ir a otra ruta para fomentar el intercambio
            if (rand() % 10 < 8) { 
                destinoIdx = rand() % nuevasRutas.size();
            } else {
                destinoIdx = -1; // Drop
            }
        }

        // Evitar movimiento nulo (mismo origen y destino)
        if (origenIdx == destinoIdx) return solActual;

        // 3. Ejecutar el movimiento
        int granjaId;
        
        // A. Extraer del origen
        if (origenIdx == -1) {
            int pos = rand() % nuevasNoVisitadas.size();
            granjaId = nuevasNoVisitadas[pos];
            nuevasNoVisitadas.erase(nuevasNoVisitadas.begin() + pos);
        } else {
            int pos = rand() % nuevasRutas[origenIdx].size();
            granjaId = nuevasRutas[origenIdx][pos];
            nuevasRutas[origenIdx].erase(nuevasRutas[origenIdx].begin() + pos);
        }

        // B. Insertar en el destino
        if (destinoIdx == -1) {
            nuevasNoVisitadas.push_back(granjaId);
        } else {
            int pos = nuevasRutas[destinoIdx].empty() ? 0 : rand() % (nuevasRutas[destinoIdx].size() + 1);
            nuevasRutas[destinoIdx].insert(nuevasRutas[destinoIdx].begin() + pos, granjaId);
        }
    }

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
            penalizacion += (inst.cuotasLeche[i] - lecheFinalPorTipo[i]) * 500.0; // penalización media
        }
    }

    for (int granjaId : sol.granjasNoVisitadas) {
        // penalización muy alta, proporcional a la cantidad de leche no recogida
        penalizacion += inst.getNodo(granjaId).cantidadLeche * 50.0; 
    }
    
    return profit - penalizacion;
}