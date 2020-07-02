INCLUDES = -IDmin/ -Igraphs/ -Inumeric/
LIBS = -lgmp
FLAGS += -std=c++17 -DNDEBUG -O3 -Wall -Wpedantic
CXX = g++

OBJECTS	= 	bin/table_Omin.o			\
			bin/common.o				\
			bin/shiloach.o				\
			bin/ftree.o					\
			bin/all_ulab_free_trees.o	\
			bin/rational.o

########################################################################

BIN_DIR = bin

all: bindir table_Omin

bindir: $(BIN_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

table_Omin: $(OBJECTS)
	$(CXX) $(FLAGS) -o table_Omin $(OBJECTS) $(LIBS)

#-----------------------------------------------------------------------

bin/table_Omin.o: table_Omin.cpp Dmin/common.hpp numeric/rational.hpp graphs/ftree.hpp graphs/all_ulab_free_trees.hpp $(BIN_DIR)
	$(CXX) $(FLAGS) -c table_Omin.cpp -o bin/table_Omin.o $(INCLUDES)

########################################################################

bin/common.o: Dmin/common.cpp Dmin/common.hpp
	$(CXX) $(FLAGS) -c Dmin/common.cpp -o bin/common.o $(INCLUDES)

bin/shiloach.o: Dmin/shiloach.cpp Dmin/common.hpp
	$(CXX) $(FLAGS) -c Dmin/shiloach.cpp -o bin/shiloach.o $(INCLUDES)

########################################################################

bin/all_ulab_free_trees.o: graphs/all_ulab_free_trees.cpp graphs/all_ulab_free_trees.hpp graphs/ftree.hpp
	$(CXX) $(FLAGS) -c graphs/all_ulab_free_trees.cpp -o bin/all_ulab_free_trees.o $(INCLUDES)

bin/ftree.o: graphs/ftree.cpp graphs/ftree.hpp
	$(CXX) $(FLAGS) -c graphs/ftree.cpp -o bin/ftree.o $(INCLUDES)

########################################################################

bin/rational.o: numeric/rational.cpp numeric/rational.hpp
	$(CXX) $(FLAGS) -c numeric/rational.cpp -o bin/rational.o $(INCLUDES)

########################################################################

clean:
	rm -rf bin/

distclean:
	make clean
	rm -f table_Omin
