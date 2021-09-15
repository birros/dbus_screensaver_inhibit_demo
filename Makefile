all: run

prog: main.c
	gcc main.c -o prog `pkg-config --cflags --libs dbus-1`

run: prog
	@./prog
