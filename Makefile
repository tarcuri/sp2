
#
# SCCS ID: %W%	%G%
#
# Makefile to control the compiling, assembling and linking of
# standalone programs in the DSL.  Used for both 4003-406 and
# 4003-506 (with appropriate tweaking).
#

#
# User supplied files
#
U_C_SRC = clock.c klibc.c processes.c queues.c scheduler.c sio.c \
	stacks.c syscalls.c system.c user.c \
	device.c pci.c ide.c disk.c ramdisk.c ata.c shared_mem.c
U_C_OBJ = clock.o klibc.o processes.o queues.o scheduler.o sio.o \
	stacks.o syscalls.o system.o user.o \
	device.o pci.o ide.o disk.o ramdisk.o ata.o shared_mem.o
U_S_SRC = klibs.S ulibs.S
U_S_OBJ = klibs.o ulibs.o
U_LIBS	=

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG

#
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
#
# Compilation/assembly control
#

#
# We only want to include from the current directory and ~wrc/include
#
INCLUDES = -I. -I/home/fac/wrc/include

#
# Compilation/assembly/linking commands and options
#
CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc -I- $(INCLUDES)

CC = gcc
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = gas
ASFLAGS =

LD = gld

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
# use of #include/#define/#ifdef statements. However, the line numbers of
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
#

.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) -o $*.o $*.s -a=$*.lst
	$(RM) $*.s

.s.b:
	$(AS) -o $*.o $*.s -a=$*.lst
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# Binary/source file for system bootstrap code

BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S

# Assembly language object/source files

S_OBJ = startup.o isr_stubs.o $(U_S_OBJ)
S_SRC =	startup.S isr_stubs.S $(U_S_SRC)

# C object/source files

C_OBJ =	c_io.o support.o $(U_C_OBJ)
C_SRC =	c_io.c support.c $(U_C_SRC)

# Collections of files

OBJECTS = $(S_OBJ) $(C_OBJ)

SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)

#
# Targets for remaking bootable image of the program
#
# This is the default target
#

disk.image: bootstrap.b prog.b prog.nl BuildImage #prog.dis 
	./BuildImage -o disk.image bootstrap.b prog.b 0x10000

prog.out: $(OBJECTS)
	$(LD) -o prog.out $(OBJECTS)

prog.o:	$(OBJECTS)
	$(LD) -o prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

prog.b:	prog.o
	$(LD) -o prog.b -s --oformat binary -Ttext 0x10000 prog.o

#
# Target for copying bootable image onto a floppy
#

floppy:	disk.image
	dd if=disk.image of=/dev/fd0

#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c

Offsets:	Offsets.c
	$(CC) $(INCLUDES) -o Offsets Offsets.c

#
# Clean out this directory
#

clean:
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets

#
# Create a printable namelist from the prog.o file
#

prog.nl: prog.o
	nm -Bng prog.o | pr -w80 -3 > prog.nl

#
# Generate a disassembly
#

prog.dis: prog.o
	dis prog.o > prog.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

bootstrap.o: bootstrap.h
startup.o: bootstrap.h
isr_stubs.o: bootstrap.h
ulibs.o: syscalls.h headers.h shared_mem.h /home/fac/wrc/include/x86arch.h
c_io.o: c_io.h startup.h support.h /home/fac/wrc/include/x86arch.h
support.o: startup.h support.h c_io.h /home/fac/wrc/include/x86arch.h
support.o: bootstrap.h
clock.o: headers.h clock.h processes.h disk.h queues.h pci.h stacks.h
clock.o: scheduler.h ata.h device.h ramdisk.h startup.h
clock.o: /home/fac/wrc/include/uart.h /home/fac/wrc/include/x86arch.h
klibc.o: headers.h
processes.o: headers.h processes.h disk.h queues.h pci.h stacks.h
queues.o: headers.h queues.h processes.h disk.h pci.h stacks.h shared_mem.h
scheduler.o: headers.h scheduler.h processes.h disk.h queues.h pci.h stacks.h
scheduler.o: system.h
sio.o: headers.h sio.h processes.h disk.h queues.h pci.h stacks.h scheduler.h
sio.o: startup.h /home/fac/wrc/include/uart.h /home/fac/wrc/include/x86arch.h
stacks.o: headers.h stacks.h queues.h
syscalls.o: headers.h syscalls.h shared_mem.h /home/fac/wrc/include/x86arch.h
syscalls.o: bootstrap.h clock.h processes.h disk.h queues.h pci.h stacks.h
syscalls.o: scheduler.h sio.h startup.h system.h ulib.h types.h
system.o: headers.h system.h processes.h disk.h queues.h pci.h stacks.h
system.o: bootstrap.h clock.h scheduler.h sio.h syscalls.h shared_mem.h
system.o: /home/fac/wrc/include/x86arch.h user.h device.h ata.h ulib.h
system.o: types.h
user.o: headers.h user.h processes.h disk.h queues.h pci.h stacks.h
user.o: shared_mem.h
device.o: headers.h device.h disk.h queues.h pci.h ramdisk.h ata.h
device.o: processes.h stacks.h ulib.h types.h shared_mem.h
pci.o: headers.h pci.h ide.h startup.h
ide.o: headers.h ide.h disk.h queues.h pci.h ata.h device.h processes.h
ide.o: stacks.h
disk.o: headers.h disk.h queues.h pci.h ata.h device.h processes.h stacks.h
disk.o: startup.h
ramdisk.o: headers.h ramdisk.h disk.h queues.h pci.h device.h scheduler.h
ramdisk.o: processes.h stacks.h
ata.o: headers.h ata.h device.h disk.h queues.h pci.h processes.h stacks.h
ata.o: ide.h ulib.h types.h shared_mem.h startup.h scheduler.h
ata.o: /home/fac/wrc/include/x86arch.h
shared_mem.o: headers.h shared_mem.h queues.h
