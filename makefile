.PHONY: all run clean

all:
	gcc -Wall -Wextra -IHeaders -o programaTrab \
	Source/*.c \
	Source/auxiliares/*.c \
	Source/implementadas/*.c

run:
	./programaTrab

clean:
	rm -f programaTrab