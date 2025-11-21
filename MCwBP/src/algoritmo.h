#pragma once
#include "instancia.h"
#include "solucion.h"

class Algoritmo {
private:
    const Instancia& inst; // feferencia a la instancia
    long seed; // semilla para aleatoriedad

public:
    Algoritmo(const Instancia& inst, long seed);

    vector<pair<int, double>> historialConvergencia; // <Iteración, Costo/Profit>

    // solución inicial con greedy
    Solucion generarSolucionInicialGreedy();

    // función principal que ejecutará la metaheurística
    Solucion ejecutarSimulatedAnnealing(double tempInicial, double tempFinal, double tasaEnfriamiento);

private:
    // SA
    // generar un vecino de la solución actual
    Solucion generarVecino(const Solucion& solActual);

    // movimientos 
    // (Ej. mover una granja de una ruta a otra)
    vector<vector<int>> movimiento_Reinsertar(vector<vector<int>> rutas);
    // (Ej. intercambiar dos granjas entre rutas)
    vector<vector<int>> movimiento_Swap(vector<vector<int>> rutas);
    // (Ej. invertir un segmento de una ruta)
    vector<vector<int>> movimiento_2Opt(vector<vector<int>> rutas);


    // criterio de aceptación
    bool aceptarSolucion(double nuevoProfit, double profitActual, double temperatura);

    // función para calcular la "energía" de una solución (a minimizar)
    double calcularEnergia(const Solucion& sol);
};