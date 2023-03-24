app:
	g++ main.cpp -o main -I include -L lib -l SDL2-2.0.0 -std=c++11

test:
	g++ sdltest.cpp -o sdltest -I include -L lib -l SDL2-2.0.0 -std=c++11