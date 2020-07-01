OBJECTS = permutation.o undirected_graph.o shiloach.o chung.o common.o probability_of_crossing_knowing_edge_lengths.o fast_prediction_of_crossings_at_constant_sum_of_lengths.o frequency_spectrum.o


#################################################

COMPILE = g++ -Wall -c -O3 

LINK = g++ -o

#################################################

all: analyze_treebanks

analyze_treebanks: analyze_treebanks.o $(OBJECTS)
	$(LINK) analyze_treebanks analyze_treebanks.o $(OBJECTS)

%.o: %.cpp
	$(COMPILE) $<

#################################################

clean:
	rm analyze_treebanks *.o

