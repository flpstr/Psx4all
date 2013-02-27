DESKTOP=1

#USE_HLE=1
#NEWSPU=1
GPUAPI=1
#DUMMYGPU=1
PETESPU=1
#NULLSPU=1


ifdef DESKTOP
  CC = gcc		# not 64-bit-safe
  CXX = g++
  INC    = /usr/include
  INCS   = ${INC} -I${INC}/SDL
  LD = g++
  LDFLAGS= `sdl-config --libs` -lz -lao -lSDL -lSDL_mixer -lSDL_ttf
  MACHINE_CFLAGS = -O2 -m32
else
  CC = /opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-cc
  CXX = /opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-c++
  AS = /opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-as
  STRIP = /opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-strip
  INC = /opt/gcw0-toolchain/usr/include
  INCS = ${INC} -I${INC}/SDL
  LD = /opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-c++
  LDFLAGS = -lSDL -lSDL_mixer -lao -lz -pthread -Wl --gc-sections
  MACHINE_CFLAGS = -O2 -m32 -fomit-frame-pointer -ffunction-sections -ffast-math -fsingle-precision-constant -flto -fwhole-program
endif

ifdef DESKTOP
  STD_INTERP=1
  #STD_INTERP_NEW=1
  NAME  = psx4all_std_int.gpe
else
  STD_PSXREC=1
  NAME  = psx4all_bios_1000.ope
endif



PROG   = $(NAME)
TOOLS  = cbin


DEFS  += -DUSE_OLD_COUNTERS
#DEFS  += -DNOSOUND

#DEFS += -DNO_RENDER

#DEFS += -DEMU_LOG -DLOG_STDOUT
#DEFS += -DPROFILER_PSX4ALL
DEFS += -DPROFILER_SDL
#DEFS += -DAUTOEVENTS=4000
#DEFS += -DMAXFRAMES=4000
#DEFS += -DNOTHREADLIB 
#DEFS += -DGP2X_SDLWRAPPER_NODOUBLEBUFFER
#DEFS += -DGTE_DUMP
#DEFS += -DLOG_STDOUT

ifdef STD_INTERP
DEFS += -DINTERP
endif

ifdef STD_PSXREC
DEFS += -DDYNAREC
endif

ifdef USE_HLE
DEFS += -DWITH_HLE
endif

DEFAULT_CFLAGS =  -DDATA_PREFIX=\"./data/\" -DROM_PREFIX=\"./\" -Isrc -Isrc/sdl -Isrc/include -Isrc/include/libpcsx -I$(INCS)

MORE_CFLAGS = -g -Wall -Wno-sign-compare
MORE_CFLAGS += -fsigned-char -fno-exceptions
#MORE_CFLAGS += -DLSB_FIRST -DSIGNED_SAMPLES -DINLINE="inline"
MORE_CFLAGS += -Wno-unused-value $(MACHINE_CFLAGS)

MORE_CFLAGS += -DMITSHM $(DEFS) -DUNZIP_SUPPORT -DZLIB -DHAVE_MKSTEMP -DHAVE_STRINGS_H

CFLAGS  = $(DEFAULT_CFLAGS) $(MORE_CFLAGS)
CPPFLAGS  = $(DEFAULT_CFLAGS) $(MORE_CFLAGS)
ASFLAGS  = -c $(DEFAULT_CFLAGS) $(MORE_CFLAGS)

OBJS =	\
	src/libpcsx/main.o \
	src/libpcsx/cdrom.o \
	src/libpcsx/decode_xa.o \
	src/libpcsx/mdec.o \
	src/libpcsx/misc.o \
	src/libpcsx/bios.o \
	src/libpcsx/dma.o \
	src/libpcsx/hle.o \
	src/libpcsx/hw.o \
	src/libpcsx/components.o \
	src/libpcsx/pad.o \
	src/libpcsx/sio.o \
	src/libpcsx/spu.o \
	src/sdl/minimal.o \

OBJS += 	src/cdr/oldversion/cdriso.o 

#OBJS += 	src/cdr/CDDAData.o 
#OBJS += 	src/cdr/FileInterface.o 
#OBJS += 	src/cdr/Globals.o 
#OBJS += 	src/cdr/Open.o 
#OBJS += 	src/cdr/SubchannelData.o 
#OBJS += 	src/cdr/TrackParser.o 
#OBJS += 	src/cdr/unrar/unrarlib.o 
#OBJS += 	src/cdr/bzip/blocksort.o 
#OBJS += 	src/cdr/bzip/bzlib.o 
#OBJS += 	src/cdr/bzip/compress.o 
#OBJS += 	src/cdr/bzip/crctable.o 
#OBJS += 	src/cdr/bzip/decompress.o 
#OBJS += 	src/cdr/bzip/huffman.o 
#OBJS += 	src/cdr/bzip/randtable.o 

ifdef NULLSPU
OBJS += src/nullspu/spu_core.o 
else
ifdef NEWSPU
OBJS +=	\
	src/franspu/spu.o \
	src/franspu/spu_dma.o \
	src/franspu/spu_registers.o \
	src/franspu/spu_output.o
endif
ifdef PETESPU
OBJS +=	\
	src/petespu/stdafx.o \
	src/petespu/adsr.o \
	src/petespu/spu_dma.o \
	src/petespu/freeze.o \
	src/petespu/alsa.o \
	src/petespu/registers.o \
	src/petespu/reverb.o \
	src/petespu/spu.o \
	src/petespu/xa.o 
endif
endif

ifdef STD_INTERP
OBJS += src/interpreter/interpreter_pcsx/interpreter.o 
OBJS +=	src/libpcsx/r3000a_new.o
OBJS +=	src/libpcsx/counters_new.o
OBJS +=	src/libpcsx/gte_pcsx.o
OBJS +=	src/libpcsx/gte_mame.o
OBJS +=	src/libpcsx/mem.o
endif

ifdef STD_INTERP_NEW
OBJS += src/interpreter/interpreter_new/psxinterpreter.o 
OBJS +=	src/libpcsx/r3000a_new.o
OBJS +=	src/libpcsx/counters_new.o
OBJS +=	src/libpcsx/gte_pcsx.o
OBJS +=	src/libpcsx/gte_mame.o
OBJS +=	src/libpcsx/mem.o
endif

ifdef STD_PSXREC
OBJS += src/interpreter/interpreter_pcsx/interpreter.o 
OBJS += src/recompiler3/mips/recompiler.o 
OBJS +=	src/libpcsx/r3000a_new.o
OBJS +=	src/libpcsx/counters_new.o
OBJS +=	src/libpcsx/gte_pcsx.o
OBJS +=	src/libpcsx/gte_mame.o
OBJS +=	src/libpcsx/mem.o
OBJS += src/libpcsx/recompiler3/mips/mips_disasm.o 
OBJS += src/libpcsx/recompiler3/mips/mips.o
endif


GPU_OBJS =  src/gpuAPI/gpuAPI.o               \
            src/gpuAPI/newGPU/newGPU.o 	      \
            src/gpuAPI/newGPU/fixed.o         \
            src/gpuAPI/newGPU/core_Command.o  \
            src/gpuAPI/newGPU/core_Dma.o      \
            src/gpuAPI/newGPU/core_Draw.o     \
            src/gpuAPI/newGPU/core_Misc.o     \
            src/gpuAPI/newGPU/raster_Sprite.o \
            src/gpuAPI/newGPU/raster_Poly.o   \
            src/gpuAPI/newGPU/raster_Line.o   \
            src/gpuAPI/newGPU/raster_Image.o  \
            src/gpuAPI/newGPU/inner.o


%.o: %.cpp
	${CXX} ${CFLAGS} -c -o $@ $<

%.o: %.s
	${CXX} ${CFLAGS} -c -o $@ $<

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

all: ${PROG} ${TOOLS}

${PROG}:	${OBJS} ${GPU_OBJS}
	${LD} ${CFLAGS} ${CFLAGS} ${GPU_OBJS} ${OBJS} -o ${PROG} ${LDFLAGS}  

strip:	${PROG} ${TOOLS}
	${STRIP} ${PROG} $(TOOLS}

cbin:	tools/cbin.c tools/cbin.h
	cc -Wall -O2 -o $@ $< -lz

clean:
	$(RM) $(PROG) $(OBJS) $(GPU_OBJS) $(TOOLS)

test2:
	$(CXX) $(CFLAGS) -S -o src/misc.S src/misc.cpp

src/recompiler3/mips/recompiler.o: src/recompiler3/mips/recompiler.cpp src/recompiler3/mips/mips_codegen.h src/recompiler3/mips/mips_std_rec_calls.cpp src/recompiler3/mips/mips_std_rec_globals.cpp src/recompiler3/mips/defines.h src/recompiler3/mips/disasm.h src/recompiler3/mips/rec_bcu.cpp.h src/recompiler3/mips/rec_cpo.h src/recompiler3/mips/rec_lsu.cpp.h \
 src/recompiler3/mips/mips_dpimacros.h src/recompiler3/mips/mips_std_rec_debug.cpp src/recompiler3/mips/mips_std_rec_regcache.cpp src/recompiler3/mips/externs.h src/recompiler3/mips/rec_alu.cpp.h src/recompiler3/mips/rec_cp0.cpp.h src/recompiler3/mips/rec_gte.cpp.h src/recompiler3/mips/rec_mdu.cpp.h src/recompiler3/mips/recompiler.h
src/recompiler3/mips/recompiler.o: src/generator/mips/generator.cpp.h src/generator/mips/gen_helper.cpp.h src/generator/mips/gen_alu.cpp.h
src/gpuAPI/newGPU/core_Draw.o: src/include/gpuAPI/newGPU/inner_Blit.h
