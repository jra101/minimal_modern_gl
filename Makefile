MAIN = triangle
CC = g++
CFLAGS = -DLINUX -std=c++0x -Wall -Wextra -Werror -g
LFLAGS = -lX11 -lGL

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(OBJS) -o $(MAIN) $(LFLAGS)

-include $(DEPS)

%.o : %.cpp
	$(CC) -c $(CFLAGS) -MMD $< -o $@

clean:
	rm -f $(OBJS) $(DEPS)
	rm -f *~ $(MAIN)
