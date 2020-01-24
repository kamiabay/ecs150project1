all: sshell

sshell: sshell.o
	gcc -o sshell sshell.o
sshell.o: sshell.c
	gcc -c -o sshell.o sshell.c
clean:
	rm -f sshell shell.o