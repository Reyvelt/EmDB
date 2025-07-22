TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"
	./$(TARGET) -f ./mynewdb.db -a "Agatha N.,42 Devil Ln.,5120"
	./$(TARGET) -f ./mynewdb.db -a "Gertie A.,92 Anthem St.,80"
	./$(TARGET) -f ./mynewdb.db -l 
	./$(TARGET) -f ./mynewdb.db -s "Agatha N." -d
	./$(TARGET) -f ./mynewdb.db -l 
	./$(TARGET) -f ./mynewdb.db -s "Gertie A." -l
default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude


