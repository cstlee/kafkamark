SRCDIR = src
BUILDDIR = build
BINDIR = bin

src = $(wildcard $(SRCDIR)/*.cpp)
obj = $(src:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
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

$(BUILDDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: test
test:
	echo $(src)
	echo $(obj)
	echo $(dep)

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
