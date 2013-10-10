OBJS=$(patsubst %.c, %.o, $(wildcard *.c))

DEBUG_LIST= -DLHL_DBG
CFLAGS= -g $(DEBUG_LIST)
LDFLAGS= -lldap

all:app
	./app
app:$(OBJS)
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)
%.o:%.c
	gcc $< -c $(CFLAGS)

clean:
	rm -f *.o app
.PHONY: clean all
