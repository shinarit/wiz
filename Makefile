#tryout makefile by hand

OBJS = colors.o  fps.o  grabscreen.o   hsv.o  resources.o  screenhack.o  usleep.o  visual.o  wiz.o  xmu.o  yarandom.o implementation.o

CC = gcc -pedantic -Wall -Wstrict-prototypes -Wnested-externs -Wmissing-prototypes -Wno-overlength-strings -Wdeclaration-after-statement -std=c89 -U__STRICT_ANSI__
CPP = g++ -pedantic -Wall -U__STRICT_ANSI__

CFLAGS		= -O2
LDFLAGS		=  -L/usr/local/lib
DEFS		= -DSTANDALONE -DHAVE_CONFIG_H -DHAVE_GTK2
INCLUDES	= -I. -pthread -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  

HACK_OBJS	= screenhack.o fps.o resources.o visual.o usleep.o yarandom.o xmu.o
CPP_HACK  = $(CPP) $(LDFLAGS)

HACK_LIBS = -lSM -lICE -lXt -lX11 -lXext -lm


wiz: $(OBJS) drawinterface.h
	$(CPP_HACK) -o $@ $(OBJS) $(HACK_LIBS)

wiz.o: wiz.cpp drawinterface.h

implementation.o: implementation.cpp drawinterface.h

%o: %c ; $(CC) -c $(INCLUDES) $(DEFS) $(CFLAGS) $<

%o: %cpp ; 	$(CPP) -c $(INCLUDES) $(DEFS) $(CFLAGS) $<

default: all

all: wiz

clean:
	-rm -f *.o wiz core


