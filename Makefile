RESULT = main
HEADERS = png.h screen.h draw.h coords.h parser.h
OBJECTS = main.o png.o screen.o draw.o coords.o parser.o

ifeq ($(OS), Windows_NT)
	RESULT := $(RESULT).exe
else
	RESULT := $(RESULT).out
endif

%.o: %.c $(HEADERS)
	gcc -c -o $@ $<

$(RESULT): $(OBJECTS)
	gcc -o $@ $^

.PHONY: clean run

clean:
	-rm $(OBJECTS) $(RESULT)
	-rm *.png

run:
	./$(RESULT) script
