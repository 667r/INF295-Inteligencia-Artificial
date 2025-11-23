CXX = g++

# flags de compilación
CXXFLAGS = -std=c++17 -Wall -O2 -Iinclude

# flags de enlace: añadir soporte para sistema de archivos
LDFLAGS = -lstdc++fs

# directorios
SRCDIR = src
BINDIR = bin
OBJDIR = obj

# ejecutable final
TARGET = $(BINDIR)/mcwb

# detecta automáticamente todos los archivos .cpp en el directorio src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# genera los nombres de los archivos objeto en el directorio obj
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# objetivo principal (compila todo)
all: $(TARGET) $(BINDIR)/analisis

# regla para el programa principal (mcwb)
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	# Excluimos analisis.o del linkeo de mcwb para evitar conflicto de main()
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(filter-out $(OBJDIR)/analisis.o, $(OBJECTS)) $(LDFLAGS)
	@echo "Compilación MCWB completada."

# regla específica para el programa de análisis
$(BINDIR)/analisis: $(OBJDIR)/analisis.o
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/analisis $(OBJDIR)/analisis.o $(LDFLAGS)
	@echo "Compilación completada."

# regla genérica para compilar .cpp a .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# regla para limpiar el proyecto
clean:
	@echo "Limpiando proyecto..."
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Listo."

.PHONY: all clean