RESULT = mdl
SOURCES = generate.c symtab.c print_pcode.c draw.c stack.c matrix.c screen.c png.c
INPUT = animate.mdl

ifeq ($(OS), Windows_NT)
	RESULT := $(RESULT).exe
else
	RESULT := $(RESULT).out
endif

OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h)

%.o: %.c
	gcc -c -o $@ $<

$(RESULT): lex.yy.c y.tab.c y.tab.h $(OBJECTS)
	gcc -o $@ lex.yy.c y.tab.c $(OBJECTS) -lm
	mkdir anim

lex.yy.c: mdl.l y.tab.h
	flex -I mdl.l

y.tab.c: mdl.y symtab.h parser.h
	bison -d -y mdl.y

y.tab.h: mdl.y
	bison -d -y mdl.y

.PHONY: clean run convert

clean:
	-rm $(OBJECTS) $(RESULT)
	-rm -r anim
	-rm *.gif
	-rm lex.yy.c y.tab.c y.tab.h

run:
	./$(RESULT) $(INPUT)

convert:
	convert anim/*.png -set delay 10 -reverse anim/*.png -set delay 10 -loop 0 animation.gif
