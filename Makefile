main:
	g++ src/main.cpp -o ray -I include -L lib -l SDL2-2.0.0 -std=c++11

test:
	g++ src/sdltest.cpp -o sdltest -I include -L lib -l SDL2-2.0.0 -std=c++11