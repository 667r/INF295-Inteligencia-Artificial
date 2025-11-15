#pragma once
#include <vector>
#include <map>
#include "instancia.h"

using namespace std;

class Solucion {
public:
    // una solución es un vector de rutas y cada ruta es un vector de IDs de granjas.
    vector<vector<int>> rutas;

    vector<int> granjasNoVisitadas;

    // metricas
    double gananciaTotal;
    double costoTransporte;
    double profit;
    bool esFactible;

public:
    // constructor para una solución 
    Solucion(const vector<vector<int>>& rutas, const vector<int>& noVisitadas, const Instancia& inst);

    // f.e.
    void evaluar(const Instancia& inst);

    void imprimirFormatoSalida(long seed, const Instancia& inst) const;
};