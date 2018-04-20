all: i2c_functions.o i2c_d6t.o run_d6t_camera finish

i2c_functions.o: i2c_functions.c
	gcc -c i2c_functions.c -o i2c_functions.o

i2c_d6t.o: i2c_d6t.cpp
	g++ -c i2c_d6t.cpp -o i2c_d6t.o

run_d6t_camera: main.cpp i2c_functions.o i2c_d6t.o
	g++ main.cpp -o run_d6t_camera i2c_d6t.o i2c_functions.o -std=c++11 `pkg-config --libs opencv`

finish:
	echo "Finished compiling! Execute ./run_d6t_camera !"

clean:
	rm -rf ./run_d6t_camera ./i2c_d6t.o ./i2c_function.o
