assembler:assembler.o pro_assembler.o first_pass.o labels_function.o action_function.o data_function.o second_pass.o
	gcc -ansi -pedantic -Wall -g assembler.o pro_assembler.o first_pass.o labels_function.o action_function.o data_function.o second_pass.o -o assembler
assembler.o: assembler.c assem.h
	gcc -g -c -ansi -pedantic -Wall assembler.c -o assembler.o
pro_assembler.o:pro_assembler.c assem.h
	gcc -g -c -ansi -pedantic -Wall pro_assembler.c -o pro_assembler.o
first_pass.o: first_pass.c fpass.h assem.h action.h
	gcc -ansi -pedantic -Wall -g -c first_pass.c -o first_pass.o
labels_function.o:labels_function.c fpass.h assem.h labels.h
	gcc -g -c -ansi -pedantic -Wall labels_function.c -o labels_function.o
action_function.o:action_function.c fpass.h action.h
	gcc -g -c -ansi -pedantic -Wall action_function.c -o action_function.o
data_function.o:data_function.c fpass.h data.h
	gcc -g -c -ansi -pedantic -Wall data_function.c -o data_function.o
second_pass.o:second_pass.c fpass.h spass.h
	gcc -g -c -ansi -pedantic -Wall second_pass.c -o second_pass.o

