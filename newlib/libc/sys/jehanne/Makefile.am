AUTOMAKE_OPTIONS = cygnus
INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)
AM_CCASFLAGS = $(INCLUDES)
 
noinst_LIBRARIES = lib.a
 
if MAY_SUPPLY_SYSCALLS
extra_objs = syscalls.o libposix_conf.o chown.o getrusage.o ids.o \
	chmod.o sigsets.o sigaction.o sigsuspend.o sigwaitinfo.o \
	sigtimedwait.o sigwait.o alarm.o sigqueue.o siglongjmp.o \
	termios.o ioctl.o
else
extra_objs =
endif
 
lib_a_SOURCES = getenv_r.c getenv.c malloc.c mallocr.c free.c freer.c \
	calloc.c callocr.c realloc.c reallocr.c environ.c signal.c \
	sysfcntl.c exit.c setenv_r.c

lib_a_LIBADD = $(extra_objs)
EXTRA_lib_a_SOURCES = libposix_conf.c syscalls.c chown.c getrusage.c \
	ids.c chmod.c sigsets.c sigaction.c sigsuspend.c sigwaitinfo.c \
	sigtimedwait.c sigwait.c alarm.c sigqueue.c siglongjmp.c \
	termios.c ioctl.c
lib_a_DEPENDENCIES = $(extra_objs)
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)
 
# if MAY_SUPPLY_SYSCALLS
# all: crt0.o
# endif
 
ACLOCAL_AMFLAGS = -I ../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
