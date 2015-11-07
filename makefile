all:
	g++ -O2 CVserver.cpp -o CVserver `pkg-config --cflags --libs opencv`
	g++ -O2 CVclient.cpp -o CVclient `pkg-config --cflags --libs opencv`
