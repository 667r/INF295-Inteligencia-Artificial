#include "solucion.h"
#include <iostream>
#include <iomanip>

Solucion::Solucion(const vector<vector<int>>& r, const Instancia& inst) : rutas(r) {
    evaluar(inst); // evaluar la solución tan pronto como se crea
}

void Solucion::evaluar(const Instancia& inst) {
    costoTransporte = 0;
    gananciaTotal = 0;
    esFactible = true;

    vector<int> lecheFinalPorTipo(inst.numTiposLeche, 0);
    vector<int> cargasActuales(inst.numCamiones, 0);

    for (int k = 0; k < rutas.size(); ++k) {
        const auto& ruta = rutas[k];
        if (ruta.empty()) continue;

        double costoRuta = 0;
        int nodoActual = inst.planta.id;
        
        char calidadMasBaja = 'A'; // el tipo de leche A es el mejor en todas las instancias
        int lecheTotalEnCamion = 0;

        // calcular costo y calidad de la ruta
        for (int granjaId : ruta) {
            const Nodo& granja = inst.getNodo(granjaId);
            costoRuta += inst.getDistancia(nodoActual, granjaId);
            nodoActual = granjaId;

            lecheTotalEnCamion += granja.cantidadLeche;
            
            // regla del blending
            if (granja.tipoLeche > calidadMasBaja) {
                calidadMasBaja = granja.tipoLeche;
            }
        }

        // costo de regreso a la planta
        costoRuta += inst.getDistancia(nodoActual, inst.planta.id);
        costoTransporte += costoRuta;

        // calcular ganancia de esta ruta
        gananciaTotal += lecheTotalEnCamion * inst.ingresosPorTipo.at(calidadMasBaja);
        
        // registrar la leche recolectada por su tipo final
        lecheFinalPorTipo[inst.tipoLecheToIndex.at(calidadMasBaja)] += lecheTotalEnCamion;

        // validar capacidad del camión 
        if (lecheTotalEnCamion > inst.capacidadesCamiones[k]) {
            esFactible = false; 
        }
    }

    // validar cuotas mínimas
    for (int i = 0; i < inst.numTiposLeche; ++i) {
        if (lecheFinalPorTipo[i] < inst.cuotasLeche[i]) {
            esFactible = false;
        }
    }

    // calcular ganancia neta
    if (!esFactible) {
        profit = -1e9; // penalización grande para soluciones no factibles
    } else {
        profit = gananciaTotal - costoTransporte;
    }
}

void Solucion::imprimirFormatoSalida(long seed, const Instancia& inst) const {
    // formato de salida
    cout << "Seed: " << seed << endl;
    
    // imprimir Ganancia final | costo total | ganancia total
    cout << fixed << setprecision(0) << gananciaTotal << " "
         << costoTransporte << " " << profit << endl;

    // imprimir rutas
    for (const auto& ruta : rutas) {
        if (ruta.empty()) {
            cout << "0-0 0 0X" << endl; // ruta vacía
            continue;
        }

        double costoRuta = 0;
        int lecheTotal = 0;
        char calidadFinal = 'A';
        int nodoActual = 1; // ID de la planta

        string rutaStr = "0";
        for (int granjaId : ruta) {
            rutaStr += "-" + to_string(granjaId);
            const Nodo& granja = inst.getNodo(granjaId);
            
            costoRuta += inst.getDistancia(nodoActual, granjaId);
            nodoActual = granjaId;
            lecheTotal += granja.cantidadLeche;
            if (granja.tipoLeche > calidadFinal) {
                calidadFinal = granja.tipoLeche;
            }
        }
        costoRuta += inst.getDistancia(nodoActual, 1);
        rutaStr += "-0";

        cout << rutaStr << " " << (int)costoRuta << " " << lecheTotal << calidadFinal << endl;
    }
}