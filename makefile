APPNAME = schematic
VER_MAJOR = 0
VER_MINOR = 1
VER_CUTOFF_COMMIT = e618606c53367bed5c74dee86905dac4c02797eb

#Auto-calculate patch version based on current commit.
#If patch version can't be calculated, just use the OS name.
LATEST_COMMIT = $(shell git rev-parse HEAD)
ifeq ($(LATEST_COMMIT),)
ifeq ($(OS),Windows_NT)
VER_PATCH = Windows
else
VER_PATCH = Linux
endif
else
VER_PATCH = $(shell git rev-list --count $(VER_CUTOFF_COMMIT)..HEAD^)
endif

D0 = $(sort $(dir $(wildcard src/*/)))
D1 = $(sort $(dir $(wildcard $(D0)*/)))
DIRS := $(sort $(dir $(wildcard $(D1)*/)) $(D0) $(D1) )
SRCS := $(wildcard $(addsuffix *.cpp, $(DIRS)))
HEADERS := $(wildcard $(addsuffix *.hpp, $(DIRS))) $(wildcard src/*.hpp)
OBJS := obj/main.o obj/version.o $(patsubst src/%.cpp,obj/%.o,$(SRCS))

#default compiler
CXX = g++

#Little macro for converting text to lowercase
#It's gross but it works on all systems without installing extra libs!
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

#Linux is default target
TGT := $(call lc,$(TARGET))
ifneq ($(filter $(TGT),lin nix linux),)
TGT :=
endif

#If we're cross-compiling for Windows.
ifneq ($(filter $(TGT),win64 win windows),)
CXX = x86_64-w64-mingw32-g++
else
ifeq ($(TGT),win32)
CXX = i686-w64-mingw32-g++
else

#If not windows and not linux, error.
ifneq ($(TGT),)
$(error ERROR: Invalid `TARGET=...` flag. Should be one of `win64`, `win32`, or `linux`)
endif

endif
endif

LN = $(CXX)

#If we're cross-compiling for Windows, set the OS variable to Windows_NT.
ifneq (,$(findstring mingw,$(CXX)))
OS = Windows_NT
endif

ARCH = $(shell $(CXX) -dumpmachine)

BITS =
ifeq ($(findstring x86_64,$(ARCH)),x86_64)
BITS = 64
else
ifeq ($(findstring i686,$(ARCH)),i686)
BITS = 32
endif
endif
ifeq ($(BITS),)
CXXTARGET =
else
CXXTARGET = -m$(BITS)
endif


#Generate for specified std and arch,
#Show all warnings & handle exceptions,
#Let compiler know we're making a library,
#And separate the data & function sections so that unused symbols can be stripped.
CXXFLAGS = -std=$(STD) $(CXXTARGET) \
	-W -Wall -Wextra -Wno-psabi -Werror \
	-pedantic -fexceptions \
	$(EXTRA_CFLAGS)

LFLAGS = $(CXXTARGET) -lzed $(EXTRA_LFLAGS)

STD = c++17

OLEVEL = $(OPT)

# opt defaults to -O3
ifndef OPT
OLEVEL = 3
endif

ifneq (,$(findstring $(OPT),F f Fast FAST))
OLEVEL = fast
endif
ifneq (,$(findstring $(OPT),S s size Size SIZE))
OLEVEL = s
endif

# if debug flag is not set
ifndef DEBUG
CXXFLAGS += -O$(OLEVEL)
LFLAGS += -s
endif
ifdef DEBUG
CXXFLAGS += -g$(DEBUG) -O$(OLEVEL) -DDEBUG
endif

# if rm exists use that, otherwise try Windows' "del"
ifeq (, $(shell rm --version))
RM = del
RMDIR = rd /s /q
MKDIR = mkdir -p
else
RM = rm -f
RMDIR = rm -rf
MKDIR = mkdir -p
endif

ifeq ($(OS),Windows_NT)
RMOBJS = $(subst /,\,$(OBJS))
endif

bin/$(APPNAME): $(OBJS)
	@$(MKDIR) bin
	$(LN) -o $@ $^ $(LFLAGS)

obj/main.o: src/main.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/%.o: src/%.cpp src/%.hpp makefile
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

src/version.cpp:
	echo "const char* VERSION = \"$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)\";" > src/version.cpp

clean: cleanbin cleanobjs
	$(RM) src/version.cpp

cleanobjs:
	$(RMDIR) obj

cleanbin:
	$(RMDIR) bin

rebuild: clean default

lint: lint.log
	@cat $^

lint.log: $(HEADERS)
	@find z/ -type f \( -name '*.cpp' -or -name '*.hpp' \) | xargs -P8 -I{} clang-tidy {} -header-filter=.* -- -std=c++17 -m64 -W -Wall -Wextra -Wno-psabi -Werror -pedantic -fexceptions -fPIC -fdata-sections -ffunction-sections -O3 -Wno-unused-private-field > lint.log 2>/dev/null || { cat $@; [ "$$(cat $@)" = '' ] && echo 'ERROR: Is clang-tidy installed?' && rm $@ -f; exit 1; }

format:
	find . -type f \( -name '*.cpp' -or -name '*.hpp' \) -not -name 'catch_amalgamated.*' | xargs -P8 -I{} sh -c 'echo Formatting {}; clang-format -i {}'

try-format:
	@find . -type f \( -name '*.cpp' -or -name '*.hpp' \) -not -name 'catch_amalgamated.*' | xargs -P8 -I{} sh -c 'clang-format --dry-run -Werror -i {}'

count-loc:
	@find z -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec wc -l {} +

get-version:
	@echo $(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)

get-revision:
	git rev-parse HEAD

.PHONY: rebuild clean cleanobjs cleanbin cleancov cleandox default install uninstall examples static dynamic shared all lint test tests benchmark benchmarks docs dox format try-format count-loc get-version get-revision coverage coverage-html
