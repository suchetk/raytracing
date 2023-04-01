main:
	g++ -o ray main.cpp -std=c++11

run:
	./ray > out.ppm && open out.ppm