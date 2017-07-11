CC = g++
CFLAGS = -static
LFLAGS = -lrdkafka++

SRCDIR = src
OBJDIR = obj
BINDIR = bin

src = $(wildcard $(SRCDIR)/*.cpp)
obj = $(src:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
dep = $(obj:.o=.d)

.SUFFIXES:

.PHONY: all
all: $(BINDIR)/producer

$(BINDIR)/producer: $(OBJDIR)/producer.o
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS)

-include $(dep)

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
