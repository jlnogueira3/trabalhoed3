# José Eduardo Constantino Nordemann - N usp: 16877738
.PHONY: all run clean

all:
	gcc -Wall -Wextra -IHeaders -o programaTrab \
	Source/*.c

run:
	./programaTrab

clean:
	rm -f programaTrab