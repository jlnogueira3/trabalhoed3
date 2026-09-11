.PHONY: all run clean

all:
	gcc -Wall -Wextra -IHeaders -o programaTrab Source/*.c Source/aux/*.c

run:
	./programaTrab

clean:
	rm -f programaTrab