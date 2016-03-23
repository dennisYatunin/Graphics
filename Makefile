RESULT = main
HEADERS = png.h screen.h draw.h matrix.h parser.h
OBJECTS = main.o png.o screen.o draw.o matrix.o parser.o
MAKE_SCRIPT = script

ifeq ($(OS), Windows_NT)
	RESULT := $(RESULT).exe
	MAKE_SCRIPT := $(MAKE_SCRIPT).exe
else
	RESULT := $(RESULT).out
	MAKE_SCRIPT := $(MAKE_SCRIPT).out
endif

%.o: %.c $(HEADERS)
	gcc -c -o $@ $<

$(RESULT): $(OBJECTS)
	gcc -o $@ $^

.PHONY: clean run

clean:
	-rm $(OBJECTS) $(RESULT) $(MAKE_SCRIPT)
	-rm script
	-rm *.png

run:
	gcc make_script.c -o $(MAKE_SCRIPT)
	./$(MAKE_SCRIPT)
	./$(RESULT) script