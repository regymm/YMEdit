all: ymedit

ymedit: myedit.o hstring.o myeditio.o myeditvar.o myeditstr.o myeditterm.o
	$(CC) $^ -o $@

clean:
	-rm myedit *.o

.PHONY: all, clean

sources = myedit.c myeditio.c myeditstr.c myeditvar.c myeditterm.c hstring.c

include $(sources:.c=.d)
%.d: %.c
	set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$; rm -f $@

CC = gcc
CFLAGS = -Wall

