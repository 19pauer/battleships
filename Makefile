OBJS := src/design.o
DEPS := $(OBJS:.o=.d)

OUTPUT := battleships

export CC  := cc
export CXX := c++
CCFLAGS    := -O2 -g -pthread -Wall -Wextra
CXXFLAGS   := -std=c++17 \
              -fno-rtti -nostdinc++
CFLAGS     := -std=c11
CPPFLAGS   := -Isrc/

LDFLAGS :=
LIBS    :=

all: $(OUTPUT)
run: $(OUTPUT)
	./$(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	$(RM) $(OBJS) $(DEPS)

-include $(DEPS)
%.o: %.c
	$(CC) -c -o $@ -MD -MP $(CPPFLAGS) $(CCFLAGS) $(CFLAGS) $<
%.o: %.cpp
	$(CXX) -c -o $@ -MD -MP $(CPPFLAGS) $(CCFLAGS) $(CXXFLAGS) $<
