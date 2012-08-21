#(C)2004-2007 AlliedModders LLC
#
# This sample Makefile is designed for SourceMod extensions that 
# are built against Metamod:Source 1.4.2, Episode 1 of the HL2SDK.
#

SMSDK = ../..
SRCDS = ../../..
SOURCEMM = ../../../sourcemm-1.4.2
HL2SDK = ../../../hl2sdk

#####################################
### EDIT BELOW FOR OTHER PROJECTS ###
#####################################

PROJECT = zps_rpg

#Uncomment for SourceMM-enabled extensions
LINK_HL2 = $(HL2LIB)/mathlib_i486.a $(HL2LIB)/tier1_i486.a

OBJECTS = sdk/smsdk_ext.cpp extension.cpp rpgplayer.cpp database.cpp events.cpp hooks.cpp vfuncs.cpp globals.cpp takedamageinfo.cpp

##############################################
### CONFIGURE ANY OTHER FLAGS/OPTIONS HERE ###
##############################################

C_OPT_FLAGS = -O3 -funroll-loops -pipe -fno-strict-aliasing
C_DEBUG_FLAGS = -O0 -g -ggdb3
CPP_GCC4_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
CPP = /opt/crosstool/gcc-3.4.1-glibc-2.3.2/i686-unknown-linux-gnu/bin/i686-unknown-linux-gnu-gcc

HL2PUB = $(HL2SDK)/public
HL2LIB = $(HL2SDK)/linux_sdk

LINK = $(LINK_HL2) -static-libgcc vstdlib_i486.so tier0_i486.so

INCLUDE = -I. -I.. -Isdk -I$(HL2PUB) -I$(HL2PUB)/dlls -I$(HL2PUB)/engine -I$(HL2PUB)/tier0 -I$(HL2PUB)/tier1 \
	-I$(HL2PUB)/vstdlib -I$(HL2SDK)/tier1 -I$(SOURCEMM) -I$(SOURCEMM)/sourcehook -I$(SOURCEMM)/sourcemm \
	-I$(SMSDK)/public -I$(SMSDK)/public/sourcepawn -I$(SMSDK)/public/extensions -I$(HL2SDK)/dlls \
	-I$(HL2SDK)/game_shared \

CFLAGS = -fpermissive -w -D_LINUX -DNDEBUG -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp -Wall -fPIC -mfpmath=sse -msse -DSOURCEMOD_BUILD -DHAVE_STDINT_H -Wno-uninitialized
CPPFLAGS = -Wno-non-virtual-dtor -fno-exceptions -fno-rtti

################################################
### DO NOT EDIT BELOW HERE FOR MOST PROJECTS ###
################################################

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS += $(C_DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS += $(C_OPT_FLAGS)
endif


GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)
ifeq "$(GCC_VERSION)" "4"
	CPPFLAGS += $(CPP_GCC4_FLAGS)
endif

BINARY = $(PROJECT).ext.so

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)/sdk
	ln -sf $(SRCDS)/bin/vstdlib_i486.so vstdlib_i486.so
	ln -sf $(SRCDS)/bin/tier0_i486.so tier0_i486.so
	$(MAKE) -f Makefile extension

extension: $(OBJ_LINUX)
	$(CPP) $(INCLUDE) $(CFLAGS) $(CPPFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o$(BIN_DIR)/$(BINARY)

debug:	
	$(MAKE) -f Makefile all DEBUG=true

default: all

clean:
	rm -rf Release/*.o
	rm -rf Release/sdk/*.o
	rm -rf Release/$(BINARY)
	rm -rf Debug/*.o
	rm -rf Debug/sdk/*.o
	rm -rf Debug/$(BINARY)
