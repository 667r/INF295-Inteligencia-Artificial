#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <regex>

using namespace std;
namespace fs = std::filesystem;

struct Resultado {
    string instancia;
    long profitIni = 0;
    bool factibleIni = false;
    long profitFin = 0;
    bool factibleFin = false;
    double tiempo = 0.0; // NUEVO: Tiempo
    
    bool leidoInstancia = false;
    bool leidoProfitIni = false;
    bool leidoFactibleIni = false;
    bool leidoProfitFin = false;
    bool leidoFactibleFin = false;
    bool leidoTiempo = false; // NUEVO: Flag de tiempo

    bool estaCompleto() const {
        return leidoInstancia && leidoProfitIni && leidoFactibleIni && 
               leidoProfitFin && leidoFactibleFin && leidoTiempo;
    }
};

string leerArchivo(const string& path) {
    ifstream f(path);
    if (!f.is_open()) return "";
    stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

string limpiarString(string s) {
    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
    return s;
}

void generarGraficoBarrasSVG(double pctIni, double pctFin, const string& rutaSalida) {
    ofstream svg(rutaSalida);
    double width = 600;
    double height = 400;
    double padding = 50;
    double barWidth = 100;
    double chartHeight = height - 2 * padding;

    double hIni = (pctIni / 100.0) * chartHeight;
    double hFin = (pctFin / 100.0) * chartHeight;

    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    svg << "<svg width=\"" << width << "\" height=\"" << height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"white\" />\n";
    svg << "<text x=\"" << width/2 << "\" y=\"" << padding/2 << "\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"18\">Factibilidad: Greedy vs SA</text>\n";
    svg << "<line x1=\"" << padding << "\" y1=\"" << padding << "\" x2=\"" << padding << "\" y2=\"" << height - padding << "\" stroke=\"black\" stroke-width=\"2\"/>\n";
    svg << "<line x1=\"" << padding << "\" y1=\"" << height - padding << "\" x2=\"" << width - padding << "\" y2=\"" << height - padding << "\" stroke=\"black\" stroke-width=\"2\"/>\n";
    svg << "<text x=\"" << padding - 10 << "\" y=\"" << height - padding << "\" text-anchor=\"end\" font-family=\"Arial\">0%</text>\n";
    svg << "<text x=\"" << padding - 10 << "\" y=\"" << padding << "\" text-anchor=\"end\" font-family=\"Arial\">100%</text>\n";

    double x1 = padding + 100;
    double y1 = height - padding - hIni;
    svg << "<rect x=\"" << x1 << "\" y=\"" << y1 << "\" width=\"" << barWidth << "\" height=\"" << hIni << "\" fill=\"#ff9999\" stroke=\"black\" />\n";
    svg << "<text x=\"" << x1 + barWidth/2 << "\" y=\"" << height - padding + 20 << "\" text-anchor=\"middle\" font-family=\"Arial\">Greedy</text>\n";
    svg << "<text x=\"" << x1 + barWidth/2 << "\" y=\"" << y1 - 10 << "\" text-anchor=\"middle\" font-family=\"Arial\" font-weight=\"bold\">" << fixed << setprecision(1) << pctIni << "%</text>\n";

    double x2 = x1 + barWidth + 100;
    double y2 = height - padding - hFin;
    svg << "<rect x=\"" << x2 << "\" y=\"" << y2 << "\" width=\"" << barWidth << "\" height=\"" << hFin << "\" fill=\"#66b3ff\" stroke=\"black\" />\n";
    svg << "<text x=\"" << x2 + barWidth/2 << "\" y=\"" << height - padding + 20 << "\" text-anchor=\"middle\" font-family=\"Arial\">SA</text>\n";
    svg << "<text x=\"" << x2 + barWidth/2 << "\" y=\"" << y2 - 10 << "\" text-anchor=\"middle\" font-family=\"Arial\" font-weight=\"bold\">" << fixed << setprecision(1) << pctFin << "%</text>\n";

    svg << "</svg>";
    svg.close();
    cout << "Grafico guardado en: " << rutaSalida << endl;
}

int main() {
    const string resultsDir = "results/";
    const string graficosDir = "graficos/";
    vector<Resultado> resultados;
    
    // Regex para capturar el tiempo (ej. "Tiempo: 12.45s")
    regex regex_tiempo("Tiempo:\\s*([0-9\\.]+)\\s*s");

    if (!fs::exists(resultsDir)) {
        cerr << "Error: No existe el directorio results/" << endl;
        return 1;
    }

    int totalCount = 0;
    int factibleIniCount = 0;
    int factibleFinCount = 0;
    double tiempoTotalAcumulado = 0.0;

    for (const auto& entry : fs::directory_iterator(resultsDir)) {
        if (entry.path().extension() != ".txt") continue;
        
        ifstream file(entry.path());
        string line;
        Resultado res;
        int mode = 0; 

        // Leemos todo el contenido para aplicar regex de tiempo
        string full_content = leerArchivo(entry.path().string());
        smatch m_time;
        if (regex_search(full_content, m_time, regex_tiempo)) {
            res.tiempo = stod(m_time[1].str());
            res.leidoTiempo = true;
        }

        // Volver al inicio del archivo para leer línea por línea los datos estructurados
        file.clear();
        file.seekg(0, ios::beg);

        while (getline(file, line)) {
            line = limpiarString(line);
            if (line.empty()) continue;

            if (line.find("Instancia:") != string::npos) {
                size_t pos = line.find(":");
                res.instancia = line.substr(pos + 1);
                while(!res.instancia.empty() && res.instancia[0] == ' ') 
                    res.instancia.erase(0, 1);
                res.leidoInstancia = true;
            }

            if (line.find("Solución Inicial") != string::npos || line.find("Solucion Inicial") != string::npos) {
                mode = 1; continue;
            }
            if (line.find("Solución Final") != string::npos || line.find("Solucion Final") != string::npos || line.find("Mejor Solución Encontrada") != string::npos) {
                mode = 2; continue;
            }

            if (mode == 1) {
                if (line.find("Profit:") != string::npos) {
                    stringstream ss(line);
                    string label;
                    ss >> label >> res.profitIni;
                    res.leidoProfitIni = true;
                }
                if (line.find("Factible:") != string::npos) {
                    res.factibleIni = (line.find("Si") != string::npos);
                    res.leidoFactibleIni = true;
                }
            }

            if (mode == 2) {
                if (line.find("Factible:") != string::npos) {
                    res.factibleFin = (line.find("Si") != string::npos);
                    res.leidoFactibleFin = true;
                }
                if (isdigit(line[0]) || (line[0] == '-' && line.length() > 1 && isdigit(line[1]))) {
                    stringstream ss(line);
                    long rev, cost, profit;
                    if (ss >> rev >> cost >> profit) {
                        string resto;
                        if (!(ss >> resto)) { 
                            res.profitFin = profit;
                            res.leidoProfitFin = true;
                        }
                    }
                }
            }
        }

        if (res.estaCompleto()) {
            resultados.push_back(res);
            totalCount++;
            if (res.factibleIni) factibleIniCount++;
            if (res.factibleFin) factibleFinCount++;
            tiempoTotalAcumulado += res.tiempo;
        } else {
            cerr << "Advertencia: Parseo incompleto en " << entry.path().filename() << endl;
        }
    }

    if (totalCount == 0) {
        cout << "No se encontraron resultados validos." << endl;
        return 0;
    }

    double pctIni = (double)factibleIniCount / totalCount * 100.0;
    double pctFin = (double)factibleFinCount / totalCount * 100.0;

    cout << "\nResultados Procesados: " << totalCount << endl;
    generarGraficoBarrasSVG(pctIni, pctFin, graficosDir + "comparacion_factibilidad.svg");

    cout << "\n--- TABLA LATEX ---\n" << endl;
    cout << "\\begin{table}[H]" << endl;
    cout << "\\centering" << endl;
    cout << "\\begin{tabular}{|l|c|c|c|c|c|}" << endl; // Columna extra para tiempo
    cout << "\\hline" << endl;
    cout << "\\textbf{Instancia} & \\textbf{Profit Ini.} & \\textbf{Fact. Ini.} & \\textbf{Profit Fin.} & \\textbf{Fact. Fin.} & \\textbf{Tiempo (s)} \\\\ \\hline" << endl;

    sort(resultados.begin(), resultados.end(), [](const Resultado& a, const Resultado& b) {
        return a.instancia < b.instancia;
    });

    for (const auto& r : resultados) {
        string nombreTex = r.instancia; 
        size_t pos = 0;
        while ((pos = nombreTex.find("_", pos)) != string::npos) {
            nombreTex.replace(pos, 1, "\\_");
            pos += 2;
        }

        cout << nombreTex << " & " 
             << r.profitIni << " & " 
             << (r.factibleIni ? "Si" : "No") << " & " 
             << "\\textbf{" << r.profitFin << "} & " 
             << "\\textbf{" << (r.factibleFin ? "Si" : "No") << "} & " 
             << fixed << setprecision(2) << r.tiempo << " \\\\" << endl;
    }
    
    // Fila de TOTALES
    cout << "\\hline" << endl;
    cout << "\\textbf{Total} & - & - & - & - & \\textbf{" << fixed << setprecision(2) << tiempoTotalAcumulado << "} \\\\" << endl;

    cout << "\\hline" << endl;
    cout << "\\end{tabular}" << endl;
    cout << "\\caption{Comparación detallada: Greedy vs SA (" << totalCount << " instancias).}" << endl;
    cout << "\\label{tab:resultados}" << endl;
    cout << "\\end{table}" << endl;

    return 0;
}