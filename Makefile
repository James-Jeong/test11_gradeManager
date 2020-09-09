include makefile.conf

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(WOPTION) -c $(SRCS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

