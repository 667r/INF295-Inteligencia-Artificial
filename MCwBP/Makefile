CXX = g++

CXXFLAGS = -std=c++17 -Wall -O2 -Iinclude

SRCDIR = src
BINDIR = bin
OBJDIR = obj

#directorio final 
TARGET = $(BINDIR)/mcwb

#detecta archivos cpp en el directorio src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

#genera los nombres de los archivos objeto en el directorio obj
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

#crea el ejecutable  
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

#regla para compilar archivos .cpp a .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

#limpiar 
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean