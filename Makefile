#tryout makefile by hand


CC = gcc -pedantic -Wall -Wstrict-prototypes -Wnested-externs -Wmissing-prototypes -Wno-overlength-strings -Wdeclaration-after-statement -std=c89 -U__STRICT_ANSI__
CPP = g++ -pedantic -Wall -U__STRICT_ANSI__

ifdef SystemRoot
  RM = del /Q
  EXE = wiz.exe
  LDFLAGS		=  
  DEFS		= 
  INCLUDES	= -I.
  HACK_LIBS = -lgdi32
  OBJS = wiz.o implementation.o
else
  ifeq ($(shell uname), Linux)
    RM = rm -f
    EXE = wiz
    LDFLAGS		=  -L/usr/local/lib
    DEFS		= -DSTANDALONE -DHAVE_CONFIG_H -DHAVE_GTK2
    INCLUDES	= -I. -pthread -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  
    HACK_LIBS = -lSM -lICE -lXt -lX11 -lXext -lm
    OBJS = colors.o  fps.o  grabscreen.o   hsv.o  resources.o  screenhack.o  usleep.o  visual.o  wiz.o  xmu.o  yarandom.o implementation.o
  endif
endif

CFLAGS		= -O2

HACK_OBJS	= screenhack.o fps.o resources.o visual.o usleep.o yarandom.o xmu.o
CPP_HACK  = $(CPP) $(LDFLAGS)


default: all

all: wiz

wiz: $(OBJS) drawinterface.h
	$(CPP_HACK) -o $@ $(OBJS) $(HACK_LIBS)

wiz.o: wiz.cpp drawinterface.h

implementation.o: implementation.cpp drawinterface.h

%o: %c ; $(CC) -c $(INCLUDES) $(DEFS) $(CFLAGS) $<

%o: %cpp ; 	$(CPP) -c $(INCLUDES) $(DEFS) $(CFLAGS) $<

clean:
	$(RM) *.o $(EXE) core


