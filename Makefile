CC = g++
CFLAGS = -Wall -Werror
LFLAGS = -lboost_program_options -lrdkafka++

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

producer-objs = \
		$(OBJDIR)/KafkaClient.$(OBJEXT)

$(BINDIR)/producer: $(OBJDIR)/producer.$(OBJEXT) $(producer-objs)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

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
