#pragma once  // instrucción para incluir en compilación una sola vez
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "utils.h"

using namespace std;

class Instancia {
public:
    int numCamiones;
    int numTiposLeche;
    int numNodos; // total de nodos (planta + granjas)

    Nodo planta;
    vector<Nodo> granjas;
    
    vector<int> capacidadesCamiones;
    vector<int> cuotasLeche;
    vector<double> ingresosLeche;

    // mapeos para fácil acceso
    map<char, int> tipoLecheToIndex; // 'A' -> 0, 'B' -> 1, 'C' -> 2
    map<char, double> ingresosPorTipo; // 'A' -> 1.0, 'B' -> 0.7...

    // matriz de distancias precalculada
    vector<vector<double>> distancias;

public:
    // constructor que llama al parser
    Instancia(const string& path);

    // función para leer y parsear el archivo
    bool leer(const string& path);

    // función auxiliar para obtener la distancia entre dos IDs de nodo
    double getDistancia(int idNodo1, int idNodo2) const;
    
    // función auxiliar para obtener un nodo por su ID
    const Nodo& getNodo(int idNodo) const;

private:
    // mapa para buscar nodos por ID rápidamente
    map<int, Nodo> nodosMap;
    void calcularMatrizDistancias();
};