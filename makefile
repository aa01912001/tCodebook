all:
	gcc -o tcb.out tcb.c  tcb_gui.c -lncurses -ljson-c
clean:
	rm -f *.out