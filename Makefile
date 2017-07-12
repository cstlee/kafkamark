CC = g++
CFLAGS = -static
LFLAGS = -lrdkafka++

SRCDIR = src
OBJDIR = obj
BINDIR = bin
SRCEXT = cc
OBJEXT = o
DEPEXT = d

src = $(wildcard $(SRCDIR)/*.$(SRCEXT))
obj = $(src:$(SRCDIR)/%.$(SRCEXT)=$(OBJDIR)/%.$(OBJEXT))
dep = $(obj:.$(OBJEXT)=.$(DEPEXT))

.SUFFIXES:

.PHONY: all
all: $(BINDIR)/producer

$(BINDIR)/producer: $(OBJDIR)/producer.$(OBJEXT)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS)

-include $(dep)

$(OBJDIR)/%.$(DEPEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.$(DEPEXT)=.$(OBJEXT)) >$@

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
