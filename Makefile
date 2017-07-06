SRCDIR = src
BINDIR = bin

src = $(wildcard $(SRCDIR)/*.cpp)
obj = $(src:.cpp=.o)
dep = $(obj:.o=.d)

CFLAGS = -static

# Flags for using librdkafka
KAFKA_FLAGS = -Ilib/librdkafka/src-cpp \
			-Llib/librdkafka/src-cpp \
			-l:librdkafka++.a

.PHONY: all
all: producer

producer: $(SRCDIR)/producer.cpp
	@mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/$@ $(CFLAGS) $< $(KAFKA_FLAGS)

-include $(dep)

%.d: %.cpp
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
