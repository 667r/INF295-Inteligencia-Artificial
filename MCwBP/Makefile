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

all: $(TARGET)

# crea el ejecutable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "Compilación completada. Ejecutable en: $(TARGET)"

# regla para compilar archivos .cpp a .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# regla para limpiar el proyecto
clean:
	@echo "Limpiando proyecto..."
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Listo."

.PHONY: all clean