run: main.o messages.pb.o
	g++ main.o messages.pb.o -L/usr/SFML-2.5.1/lib/ -lprotobuf -lsfml-network -lsfml-system -o run

main.o: main.cpp messages.pb.h
	g++ -c -I/usr/SFML-2.5.1/include/ main.cpp

messages.pb.o: messages.pb.cc
	g++ -c messages.pb.cc

messages.pb.cc messages.pb.h: messages.proto
	protoc --cpp_out=. messages.proto

.PHONY: clean
clean:
	rm -f *.o
	rm -f run
