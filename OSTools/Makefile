#----------------------------------------------------------------------------
#       MAKEFILE
#
#	Controlling makefile for File Assistant
#
#	Created:	1st August 2005
#
#	Copyright (C) 1995-2005 T Swann
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Target to make
#	--------------

PATH 		:= /c/devkitPro/devkitPSP/bin:$(PATH)

STATICLIB = libost.a
TARGET :=					OSTools

#----------------------------------------------------------------------------
#	Project folders
#	---------------

SOURCE_DIR :=				
INCLUDE_DIR :=				../OSLib

#----------------------------------------------------------------------------
#	Source to make
#	--------------

LIBOBJS :=					 \
							$(SOURCE_DIR)ostools.o \

OBJS :=						$(SOURCE_DIR)test.o \
							../common/callbacks.o ../common/vram.o \
							$(LIBOBJS) \
							../OSLib/libosl.a ../OSLib/lib/libmikmod.a

#----------------------------------------------------------------------------
#	Additional includes
#	-------------------

INCDIR   :=					$(INCDIR) \
							$(INCLUDE_DIR)

#----------------------------------------------------------------------------
#	Addditional libraries
#	---------------------

SDK_LIBS :=					-lpspsdk \
							-lpspctrl \
							-lpspumd \
							-lpsprtc \
							-lpspgu -lpspgum \
							-lpspaudiolib \
							-lpspaudio \
							-lpspaudiocodec -lpspmpeg \
							-lpsppower \
							-lpspusb -lpspusbstor -lpspdebug

EXTERN_LIBS :=				../OSLib/libosl.a \
							../OSLib/lib/libpng.a \
							../OSLib/lib/libz.a \
							../OSLib/lib/libmikmod.a

LIBS :=						$(EXTERN_LIBS) \
							$(SDK_LIBS) \
							-lm

#----------------------------------------------------------------------------
#	Preprocesser defines
#	--------------------

DEFINES :=					-D_DEBUG \
							-DPSP

#----------------------------------------------------------------------------
#	Compiler settings
#	-----------------

CFLAGS :=					$(DEFINES) -O2 -G0 -g -Wall -DHAVE_AV_CONFIG_H -fno-strict-aliasing #-fverbose-asm -s -S
CXXFLAGS :=					$(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS :=					$(CFLAGS)

LIBDIR :=					../OSLib

LDFLAGS :=					-Wl,-Map -Wl,oslib.map

#----------------------------------------------------------------------------
#	PBP parameters
#	--------------

EXTRA_TARGETS :=			EBOOT.PBP
#PSP_EBOOT_ICON :=			../ICON0.PNG
#PSP_EBOOT_PIC1 :=			../PIC1.PNG
PSP_EBOOT_TITLE :=			Oldschool Library for PSP

#----------------------------------------------------------------------------
#	Default build settings
#	----------------------

PSPSDK :=					$(shell psp-config --pspsdk-path)

include						$(PSPSDK)/lib/build.mak

#----------------------------------------------------------------------------
#	Copy to PSP
#	-----------


lib: $(STATICLIB)

$(STATICLIB): $(LIBOBJS)
	$(AR) rcs $@ $(LIBOBJS)
	$(RANLIB) $@

ifneq ($VS_PATH),)
CC       = psp-gcc
endif

kx-install: kxploit
ifeq ($(PSP_MOUNT),)
		@echo '*** Error: $$(PSP_MOUNT) undefined. Please set it to for example /cygdrive/e'
		@echo if your PSP is mounted to E: in cygwin.
else
		cp -r $(TARGET) $(PSP_MOUNT)/PSP/GAME/
		cp -r $(TARGET)% $(PSP_MOUNT)/PSP/GAME/
		cp $(TARGET).elf $(PSP_MOUNT)/PSP/GAME/$(TARGET)
		cp -r -u "../Data" $(PSP_MOUNT)/PSP/GAME/$(TARGET)
endif
