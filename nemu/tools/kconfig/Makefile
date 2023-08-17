NAME = conf
obj := build
SRCS += confdata.c expr.c preprocess.c symbol.c util.c
SRCS += $(obj)/lexer.lex.c $(obj)/parser.tab.c
CC = gcc
CFLAGS += -DYYDEBUG
INC_PATH += .
## 如果name就是conf，那么就把conf.c加入到SRCS中，否则如果等于mconf，则加入mconf.c同时还要将
ifeq ($(NAME),conf)
SRCS += conf.c
else ifeq ($(NAME),mconf)
SRCS += mconf.c $(shell find lxdialog/ -name "*.c")
LIBS += -lncurses
else
$(error bad target=$(NAME))
endif

include $(NEMU_HOME)/scripts/build.mk

$(obj)/lexer.lex.o: $(obj)/parser.tab.h
$(obj)/lexer.lex.c: lexer.l $(obj)/parser.tab.h
	@echo + LEX $@
	@flex -o $@ $<

$(obj)/parser.tab.c $(obj)/parser.tab.h: parser.y
	@echo + YACC $@
	@bison -v $< --defines=$(obj)/parser.tab.h -o $(obj)/parser.tab.c

conf:
	@$(MAKE) -s

mconf:
	@$(MAKE) -s NAME=mconf

.PHONY: conf mconf
