

all: parser main type place
		g++ -Wall -pedantic *.o -o pnr.elf 

main:	main.cpp
		g++ -Wall -pedantic -g -c main.cpp 

parser:	parser.cpp parser.h 
		g++ -Wall -pedantic -g -c parser.cpp 
		
type:	type.cpp type.h
		g++ -Wall -pedantic -g -c type.cpp 

place:	place.cpp place.h
		g++ -Wall -pedantic -g -c place.cpp 
		
clean:
	rm *.o *.elf
