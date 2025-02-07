# Dossiers
OBJDIR = obj
SRCDIR = src
BINDIR = bin
INCDIR = include
EXTDIR = external

# Compilateur et options
CC = g++
CFLAGS = -Wall -std=c++17 -I$(INCDIR) -I$(EXTDIR)
LDFLAGS = -lboost_system -lboost_thread -lpthread -ljsoncpp -lssl -lcrypto

# Fichiers source et objets
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Nom de l'exécutable
EXEC = $(BINDIR)/a.out

# Compilation finale
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compilation des fichiers .cpp en .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Dépendances suplémentaires
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(INCDIR)/obswebsocket.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/obswebsocket.o: $(INCDIR)/websocket_endpoint.hpp
$(OBJDIR)/websocket_endpoint.o: $(INCDIR)/connection_metadata.hpp

# Création du dossier obj s'il n'existe pas
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Création du dossier obj s'il n'existe pas
$(BINDIR):
	mkdir -p $(BINDIR)

# Exécution
run:
	$(EXEC)

runv:
	valgrind $(EXEC)

rung:
	gdb $(EXEC)

# Nettoyage
clean:
	rm -rf $(OBJDIR)/*.o $(EXEC)
