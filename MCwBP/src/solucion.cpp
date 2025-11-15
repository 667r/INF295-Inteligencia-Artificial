#include "solucion.h"
#include <iostream>
#include <iomanip>

Solucion::Solucion(const vector<vector<int>>& r, const vector<int>& noVisitadas, const Instancia& inst) 
    : rutas(r), granjasNoVisitadas(noVisitadas) {
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
    
    if (!granjasNoVisitadas.empty()) {
        esFactible = false;
    }

    // calcular ganancia neta
    profit = gananciaTotal - costoTransporte;

}

void Solucion::imprimirFormatoSalida(long seed, const Instancia& inst) const {
    cout << "Seed: " << seed << endl;
    
    cout << fixed << setprecision(0) << gananciaTotal << " "
         << costoTransporte << " " << profit << endl;

    for (size_t k = 0; k < rutas.size(); ++k) {
        const auto& ruta = rutas[k];
        if (ruta.empty()) continue;

        double costoRuta = 0;
        int lecheTotal = 0;
        char calidadFinal = 'A';
        int nodoActual = 1; // ID de la planta

        string rutaStr = "0";
        for (int granjaId : ruta) {
            rutaStr += "-" + to_string(granjaId);
            const Nodo& granja = inst.getNodo(granjaId); // usa la instancia pasada
            
            costoRuta += inst.getDistancia(nodoActual, granjaId); // usa la instancia pasada
            nodoActual = granjaId;
            lecheTotal += granja.cantidadLeche;
            if (granja.tipoLeche > calidadFinal) {
                calidadFinal = granja.tipoLeche;
            }
        }
        costoRuta += inst.getDistancia(nodoActual, 1); // usa la instancia pasada
        rutaStr += "-0";

        cout << rutaStr << " " << (int)round(costoRuta) << " " << lecheTotal << calidadFinal << endl;
    }
}