all: P4_Scenario1 P4_Scenario2

P4_Scenario1.o: P4_Scenario1.c
	gcc -Wall -pthread -c P4_Scenario1.c -o P4_Scenario1.o

P4_Scenario1: P4_Scenario1.o
	gcc -pthread P4_Scenario1.o -o P4_Scenario1

P4_Scenario2.o: P4_Scenario2.c
	gcc -Wall -pthread -c P4_Scenario2.c -o P4_Scenario2.o

P4_Scenario2: P4_Scenario2.o
	gcc -pthread P4_Scenario2.o -o P4_Scenario2

clean:
	rm *.o
	rm P4_Scenario1 P4_Scenario2
