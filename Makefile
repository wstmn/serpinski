all:
	g++ -Isrc/include -Lsrc/lib -o main game.cpp -lmingw32 -lSDL2main -lSDL2
