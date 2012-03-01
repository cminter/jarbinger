######################################################################
# Makefile
######################################################################

ifndef JAVA_HOME
  $(error must define JAVA_HOME)
endif
JAVA_INC_DIRS = $(JAVA_HOME)/include $(JAVA_HOME)/include/linux
JAVA_LIB_DIRS = $(JAVA_HOME)/jre/lib/amd64/server
JAVA_LIBS = jvm

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
replaceSpace = $(subst $(SPACE),$1,$(strip $2))

JB_TOOL = jarbinger
JB_ARCHIVE_DIR = jarbinger_data

ifndef TOOL
  TOOL = $(JB_TOOL)
  OUTPUT_DIR = $(shell pwd)
  ARCHIVE_DIR = $(JB_ARCHIVE_DIR)
endif

ifndef ARCHIVE_DIR
  $(error must define ARCHIVE_DIR)
endif

ifndef JARBINGER_CACHE_PATH_FORMAT
  JARBINGER_CACHE_PATH_FORMAT = "/tmp/jarbinger-%1%/%2%/%3%"
endif

ifndef BUILD_TAG
  BUILD_TAG = $(shell uuidgen)
endif

ifndef JAR_FILENAME 
  JAR_FILENAME = "."
endif

ifndef MAIN_CLASS_NAME
  MAIN_CLASS_NAME = "Main"
endif

ifneq ($(TOOL),$(JB_TOOL))
  SRCS = tool.cpp jvm.cpp
else
  SRCS = jarbinger.cpp
endif
SRCS += common.cpp
EXE = $(OUTPUT_DIR)/$(TOOL)
UNARCHIVE_CMD_FORMAT = "tar -jxmf %1%"

ifndef BOOST_HOME
  BOOST_C_FLAGS =
  BOOST_LD_FLAGS1 =
else
  BOOST_C_FLAGS = -I$(BOOST_HOME)/include
  BOOST_LD_FLAGS1 = -L$(BOOST_HOME)/lib64
endif

ARCHIVE = $(ARCHIVE_DIR).tar.bz2
D_IN = data_file
D_HDR = data.h
D_HDR_M4 = $(D_HDR).m4
D_OBJ = data.o
OBJS = $(SRCS:.cpp=.o)
LIBS = boost_system boost_filesystem

C_FLAGS = -fpermissive $(BOOST_C_FLAGS)

LD_FLAGS1 = $(BOOST_LD_FLAGS1)
ifneq ($(TOOL),$(JB_TOOL))
  LD_FLAGS1 += $(addprefix -L,$(JAVA_LIB_DIRS))
  LIBS += $(JAVA_LIBS)
endif

LD_FLAGS2 = $(addprefix -l,$(LIBS))
RPATH =
ifdef BOOST_HOME
  RPATH += $(BOOST_HOME)/lib64
endif
ifneq ($(TOOL),$(JB_TOOL))
  RPATH += $(JAVA_LIB_DIRS)
endif
ifneq ($(RPATH),)
  LD_FLAGS2 += -Wl,-rpath,$(call replaceSpace,:,$(RPATH))
endif

CLEANFILES += $(EXE) $(OBJS) $(D_OBJ) $(D_HDR) *.h.gch $(D_IN) $(ARCHIVE)

######################################################################

all: $(EXE)

######################################################################

$(EXE): $(OBJS) $(D_OBJ)
	g++ -o $@ $^ $(LD_FLAGS1) $(LD_FLAGS2)
ifeq ($(ARCHIVE_DIR),$(JB_ARCHIVE_DIR))
	rm -rf $(ARCHIVE_DIR) $(D_IN)
endif
	rm -rf $(ARCHIVE)

tool.o: $(D_HDR) common.h

jarbinger.o: $(D_HDR) common.h

jvm.o: C_FLAGS += $(addprefix -I,$(JAVA_INC_DIRS))

%.o: %.cpp
	g++ -c -g -Wall $(C_FLAGS) $^

$(D_OBJ): $(D_IN)
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386 $^ $@

$(D_HDR): $(D_HDR_M4)
	m4 -D_TOOL_=$(TOOL) -D_DATA_FILE_=$(D_IN) \
  -D_JARBINGER_CACHE_PATH_FORMAT_=$(JARBINGER_CACHE_PATH_FORMAT) \
  -D_BUILD_TAG_=$(BUILD_TAG) \
  -D_UNARCHIVE_CMD_FORMAT_=$(UNARCHIVE_CMD_FORMAT) \
  -D_JAR_FILENAME_=$(JAR_FILENAME) \
  -D_MAIN_CLASS_NAME_=$(MAIN_CLASS_NAME) \
  -D_JVM_OPTS_DEFAULT_="$(JVM_OPTS_DEFAULT)" \
  -D_JVM_OPTS_=$(JVM_OPTS) $^ > $@

$(D_IN): $(ARCHIVE)
	ln -sf $^ $@

$(ARCHIVE): $(ARCHIVE_DIR)
	tar -jcf $@ -C $^ .

$(JB_ARCHIVE_DIR):
	mkdir $@
	cp tool.cpp $@
	cp common.cpp common.h $@
	cp jvm.cpp jvm.h $@
	cp data.h.m4 $@
	cp Makefile $@

######################################################################

test_run:
	./$(EXE)

######################################################################

clean: 
	rm -f $(CLEANFILES)

