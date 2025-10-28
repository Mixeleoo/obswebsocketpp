# Dossiers
OBJDIR = obj
SRCDIR = src
BINDIR = bin
LIBDIR = lib
INCDIR = include
EXTDIR = external
BUIDIR = build

# Compilateur et options
CC = g++
CFLAGS = -Wall -std=c++17 -I$(INCDIR) -I$(EXTDIR)
LDFLAGS = -lboost_system -lboost_thread -lpthread -ljsoncpp -lssl -lcrypto

# Fichiers source et objets
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Nom de la bibliothèque
STATIC_LIB = $(LIBDIR)/libobswebsocket++.a

# Compilation finale
all: $(STATIC_LIB)

$(BINDIR)/%.exe: %.cpp
	$(CC) $(CFLAGS) -L$(LIBDIR) $< -lobswebsocket++ $(LDFLAGS) -o $@

# Création de la bibliothèque statique
$(STATIC_LIB): $(OBJS)
	ar rcs $@ $^

# Compilation des fichiers .cpp en .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp | $(OBJDIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OBJDIR)/obswebsocket.o: $(INCDIR)/websocket_endpoint.hpp
$(OBJDIR)/websocket_endpoint.o: $(INCDIR)/connection_metadata.hpp

# Création du dossier obj s'il n'existe pas
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Création du dossier obj s'il n'existe pas
$(BINDIR):
	mkdir -p $(BINDIR)

# Nettoyage
clean:
	rm -rf $(OBJDIR)/*.o $(EXEC)
