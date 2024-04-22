OUTPUT = a.exe

LINK =
INCLUDE = 
CS = -Wall -Wextra -std=c++2a -Wno-unused-parameter -Wno-sign-compare
CS += -Werror=vla

debug:
	clang++ $(CS) -g test.cpp -o $(OUTPUT) $(INCLUDE) $(LINK)

release:
	clang++ $(CS) -s -O3 test.cpp -o $(OUTPUT) $(INCLUDE) $(LINK)

clean:
	rm $(OUTPUT)
