OUTPUT = a.exe

LINK =
INCLUDE = 
CS = -Wall -Wextra -std=c++2a -Wno-unused-parameter -Wno-sign-compare -Wno-unused-value -Wno-shift-op-parentheses
CS += -Werror=vla

debug:
	clang++ $(CS) -g sample.cpp -o $(OUTPUT) $(INCLUDE) $(LINK)

release:
	clang++ $(CS) -s -O3 sample.cpp -o $(OUTPUT) $(INCLUDE) $(LINK)

clean:
	rm $(OUTPUT)
