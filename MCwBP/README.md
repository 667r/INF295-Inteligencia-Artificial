# Implementación de solver para el Milk Collection with Blending Problem (MCwBP) utilizando Greedy + Simulated Annealing

Este proyecto implementa una solución metaheurística híbrida para resolver el problema de recolección de leche con mezcla. El algoritmo combina una heurística constructiva **Greedy Robusto** con **Simulated Annealing** para optimizar las rutas de recolección, maximizando el profit y asegurando el cumplimiento de cuotas de calidad y capacidad.

## Requisitos del Sistema

El proyecto está desarrollado en **C++ estándar (C++17)** para garantizar eficiencia y portabilidad.

* **Compilador:** `g++` con soporte para C++17.
* **Build System:** `make`.
* **Sistema Operativo:** Fedora Linux.

**Nota sobre Librerías:** La única libreria externa utilizada es `pandas` para la generación de gráficos más legibles:

```bash
pip3 install pandas
```

## Compilación

Para compilar el proyecto, simplemente ejecute el siguiente comando en la raíz del directorio MCwB:

```bash
make
````

Esto generará dos ejecutables en la carpeta `bin/`:

1.  `bin/mcwb`: El solver principal.
2.  `bin/analisis`: Herramienta de post-procesamiento y estadística.

Para limpiar los archivos compilados (resetear el build):

```bash
make clean
```

## Ejecución y Uso

El flujo de trabajo está automatizado para procesar múltiples instancias secuencialmente.

### 1\. Ejecutar el Solver

Este comando lee automáticamente todos los archivos `.txt` ubicados en la carpeta `instancias/`, ejecuta el algoritmo Greedy+SA para cada uno, y genera los resultados.

```bash
./bin/mcwb
```

**Salidas generadas:**

  * **Resultados de texto:** En la carpeta `results/` (formato: `nombre_instancia.txt`). Incluye la solución inicial, la final y el detalle de rutas.
  * **Datos de convergencia:** En la carpeta `results/` (formato: `.csv`).
  * **Gráficos de convergencia:** En la carpeta `graficos/` (formato: `.svg`). Puede abrirlos con cualquier navegador web.

### 2\. Generar Reporte de Análisis

Una vez ejecutado el solver, utilice este comando para analizar la factibilidad global y generar la tabla resumen en formato LaTeX:

```bash
./bin/analisis
```

**Salidas generadas:**

  * Resumen en consola con porcentajes de factibilidad (Greedy vs SA).
  * Gráfico comparativo en `graficos/comparacion_factibilidad.svg`.
  * Tabla LaTeX impresa en terminal (lista para copiar al informe).

### EXTRA

**Plotter en Python**

  * Para efectos de generar mejores gráficos para el informe, se hizo un script graficador en Python, el cual genera gráficos específicamente para las instancias ejemplificadas en el informe. Para generar estos gráficos, ejecute el siguiente comando en el root del proyecto:

```bash
python3 src/plotter.py
```

## Configuración de Parámetros

Para modificar el comportamiento del algoritmo o probar con diferentes parámetros, edite las constantes al inicio del archivo `src/main.cpp` y recompile con `make`.

```cpp
// src/main.cpp
double tempInicial = 100000.0;    // temperatura inicial
double tempFinal = 1.0;           // criterio de parada
double tasaEnfriamiento = 0.9999995; // velocidad de enfriamiento
```

Para modificar o agregar nuevas instancias de prueba, simplemente añada los archivos `.txt` (respetando el formato estándar del problema) en la carpeta `instancias/`.

## Estructura del Proyecto

```text
.
├── bin/                # ejecutables compilados
├── graficos/           # gráficos SVG generados automáticamente
├── instancias/         # archivos de entrada (.txt)
├── results/            # archivos de salida (.txt y .csv)
├── src/                # código fuente C++
│   ├── algoritmo.h/cpp # lógica greedy y Simulated Annealing
│   ├── instancia.h/cpp # parser de archivos
│   ├── solucion.h/cpp  # representación y evaluación
│   ├── graficador.h    # generador de SVG 
│   ├── analisis.cpp    # script de estadísticas
│   └── plotter.py      # script generador de gráficos para el informe
│   └── main.cpp        # punto de entrada
└── Makefile            # script de compilación
```

## Autor

**Mariano Varas Ramos**
Inteligencia Artificial - Universidad Técnica Federico Santa María

Se utilizó el LLM Google Gemini para la realizar los siguientes archivos:

* 'Makefile'
* 'graficador .h'
* Estructuras de guardado de resultados en 'main.cpp'
* analisis.cpp

La completitud del código relacionado a la implementación de estructuras de datos/algoritmos/main simple fue desarrollado y comentado por mi.

