#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// Estructura simple para puntos
struct Punto {
    double x, y;
};

class Graficador {
public:
    static void generarSVG(const std::string& archivoCSV, const std::string& archivoSalida) {
        std::vector<Punto> datos = leerCSV(archivoCSV);
        if (datos.empty()) return;

        std::ofstream svg(archivoSalida);
        
        // Configuración del lienzo
        double width = 800;
        double height = 400;
        double padding = 60;

        // Calcular máximos y mínimos
        double minX = datos.front().x, maxX = datos.back().x;
        double minY = datos[0].y, maxY = datos[0].y;

        for (const auto& p : datos) {
            if (p.y < minY) minY = p.y;
            if (p.y > maxY) maxY = p.y;
        }
        
        // Evitar división por cero si es una línea recta
        if (maxY == minY) maxY += 1.0;

        // Lambdas para escalar coordenadas
        auto scaleX = [&](double val) {
            return padding + (val - minX) / (maxX - minX) * (width - 2 * padding);
        };
        auto scaleY = [&](double val) { // Y invertido porque SVG 0,0 es arriba-izquierda
            return height - padding - (val - minY) / (maxY - minY) * (height - 2 * padding);
        };

        // --- Escribir SVG ---
        svg << "<svg width=\"" << width << "\" height=\"" << height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
        
        // Fondo blanco y borde
        svg << "<rect width=\"100%\" height=\"100%\" fill=\"white\" />\n";
        
        // Ejes
        svg << "<line x1=\"" << padding << "\" y1=\"" << height - padding << "\" x2=\"" << width - padding << "\" y2=\"" << height - padding << "\" stroke=\"black\" stroke-width=\"2\"/>\n"; // X
        svg << "<line x1=\"" << padding << "\" y1=\"" << padding << "\" x2=\"" << padding << "\" y2=\"" << height - padding << "\" stroke=\"black\" stroke-width=\"2\"/>\n"; // Y

        // Título y etiquetas
        svg << "<text x=\"" << width/2 << "\" y=\"" << padding/2 << "\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"16\">Convergencia del Profit</text>\n";
        
        // Línea de datos (Polyline)
        svg << "<polyline points=\"";
        for (const auto& p : datos) {
            svg << scaleX(p.x) << "," << scaleY(p.y) << " ";
        }
        svg << "\" fill=\"none\" stroke=\"blue\" stroke-width=\"2\" />\n";

        // Valores en los ejes (Min/Max)
        svg << "<text x=\"" << padding - 10 << "\" y=\"" << scaleY(minY) << "\" text-anchor=\"end\" font-family=\"Arial\" font-size=\"12\">" << (int)minY << "</text>\n";
        svg << "<text x=\"" << padding - 10 << "\" y=\"" << scaleY(maxY) << "\" text-anchor=\"end\" font-family=\"Arial\" font-size=\"12\">" << (int)maxY << "</text>\n";
        svg << "<text x=\"" << width - padding << "\" y=\"" << height - padding + 20 << "\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"12\">Iter: " << (int)maxX << "</text>\n";
        
        svg << "</svg>";
        svg.close();
    }

private:
    static std::vector<Punto> leerCSV(const std::string& path) {
        std::vector<Punto> puntos;
        std::ifstream file(path);
        std::string line, valX, valY;
        
        std::getline(file, line); // Saltar header
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::getline(ss, valX, ',');
            std::getline(ss, valY, ',');
            try { puntos.push_back({std::stod(valX), std::stod(valY)}); } catch (...) {}
        }
        return puntos;
    }
};