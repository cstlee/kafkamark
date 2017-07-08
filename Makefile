SRCDIR = src
BUILDDIR = build
BINDIR = bin

src = $(wildcard $(SRCDIR)/*.cpp)
obj = $(src:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
dep = $(obj:.o=.d)

CFLAGS = -static
LFLAGS = -lrdkafka++

.PHONY: all producer
all: producer
producer: $(BINDIR)/producer

$(BINDIR)/producer: $(SRCDIR)/producer.cpp
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS)

-include $(dep)

$(BUILDDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
