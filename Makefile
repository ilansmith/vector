CC=gcc
AR=ar
CFLAGS=-Wall -Werror -O0 -g -m64
LFLAGS=-lvector -L.
APP=vector
LIBS=libvector.a
OBJS=test.o

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

lib%.a: %.o
	$(AR) -r $@ $<

.PHONY: all clean cleanall

all: $(APP)

$(APP): $(OBJS) $(LIBS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)

clean:
	@echo "removing executables"
	@rm -f $(APP)
	@echo "removing object files"
	@rm -f *.o *.a

cleanall: clean
	@echo "removing pre compilation files"
	@rm -f *_pre.c
	@echo "removing tag file"
	@rm -f tags

