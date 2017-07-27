CC = g++
CFLAGS = -std=c++11 -Wall -Werror \
		-Ilib/PerfUtils/include
LFLAGS = -static \
		-l:libboost_program_options.a \
		-l:librdkafka++.a -l:librdkafka.a -lpthread \
		-Llib/PerfUtils/lib -l:libPerfUtils.a

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
all: $(BINDIR)/producer $(BINDIR)/consumer

producer-objs = \
		$(OBJDIR)/KafkaClient.$(OBJEXT) \
		$(OBJDIR)/TraceLog.$(OBJEXT)

$(BINDIR)/producer: $(OBJDIR)/producer.$(OBJEXT) $(producer-objs)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

consumer-objs = \
		$(OBJDIR)/KafkaClient.$(OBJEXT) \
		$(OBJDIR)/TraceLog.$(OBJEXT)

$(BINDIR)/consumer: $(OBJDIR)/consumer.$(OBJEXT) $(consumer-objs)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

-include $(dep)

$(OBJDIR)/%.$(DEPEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.$(DEPEXT)=.$(OBJEXT)) >$@

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
