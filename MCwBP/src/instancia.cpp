#include "instancia.h"
#include <fstream>
#include <sstream>

Instancia::Instancia(const string& path) {
    if (!leer(path)) {
        cerr << "Error: No se pudo leer el archivo de instancia en " << path << endl;
        exit(1);
    }
    calcularMatrizDistancias();
}

bool Instancia::leer(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return false;

    string line;

    file >> numCamiones;
    capacidadesCamiones.resize(numCamiones);


    for (int i = 0; i < numCamiones; ++i) {
        file >> capacidadesCamiones[i];
    }
 
    file >> numTiposLeche;
    cuotasLeche.resize(numTiposLeche);
    ingresosLeche.resize(numTiposLeche);
 
    for (int i = 0; i < numTiposLeche; ++i) {
        file >> cuotasLeche[i];
    }

    for (int i = 0; i < numTiposLeche; ++i) {
        file >> ingresosLeche[i];
    }

    file >> numNodos;


    char tipoChar = 'A';
    for(int i = 0; i < numTiposLeche; ++i) {
        tipoLecheToIndex[tipoChar] = i;
        ingresosPorTipo[tipoChar] = ingresosLeche[i];
        tipoChar++; // 'A' -> 'B' -> 'C'
    }

    // leer cada nodo
    for (int i = 0; i < numNodos; ++i) {
        Nodo n;
        char tipoLecheStr;
        file >> n.id >> n.x >> n.y >> tipoLecheStr >> n.cantidadLeche;
        
        if (tipoLecheStr != '-') {
            n.tipoLeche = tipoLecheStr;
        }

        if (n.id == 1) { // el nodo 1 es la planta
            planta = n;
        } else {
            granjas.push_back(n);
        }
        nodosMap[n.id] = n;
    }

    file.close();
    return true;
}

void Instancia::calcularMatrizDistancias() {
    
    distancias.resize(numNodos + 1, vector<double>(numNodos + 1, 0.0));
    
    for (int i = 1; i <= numNodos; ++i) {
        for (int j = 1; j <= numNodos; ++j) {
            distancias[i][j] = calcularDistancia(getNodo(i), getNodo(j));
        }
    }
}

double Instancia::getDistancia(int idNodo1, int idNodo2) const {
    if (idNodo1 > numNodos || idNodo2 > numNodos || idNodo1 < 1 || idNodo2 < 1) {
        return 0.0; // ID out of range
    }
    return distancias.at(idNodo1).at(idNodo2);
}

const Nodo& Instancia::getNodo(int idNodo) const {
    return nodosMap.at(idNodo);
}