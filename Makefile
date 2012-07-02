CFLAGS=`pkg-config --cflags gtk+-3.0` -g
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
BIN=libmiranda.so

$(BIN): $(OBJS)
	gcc -shared -fPIC -DPIC -o $(BIN) $(OBJS) `pkg-config --libs gtk+-3.0` -g
	strip $(BIN)

clean:
	rm -f $(BIN) $(OBJS)
