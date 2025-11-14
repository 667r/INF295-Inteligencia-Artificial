#pragma once
#include <cmath>
#include <vector>

using namespace std;

// estructura para almacenar la información de un nodo (planta o granja)
struct Nodo {
    int id;
    double x, y;
    char tipoLeche = '-'; // 'A', 'B', 'C'
    int cantidadLeche = 0;
};

// función para calcular la distancia euclidiana entre dos nodos
inline double calcularDistancia(const Nodo& a, const Nodo& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}