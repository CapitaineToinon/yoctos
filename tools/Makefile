# Run "make STATIC=1" to generate static binaries

include ../common.mk

CC=gcc
LD=gcc

ifeq ($(STATIC),1)
	LD+= -static
	SANITIZERS=
else
	SANITIZERS+=-fsanitize=address -fsanitize=leak -fsanitize=undefined -g
endif

override CC_FLAGS+=-m32 -c -std=gnu11 -MMD -Wall -Wextra $(SANITIZERS)

override LD_FLAGS+=-m32 $(SANITIZERS)
#LD_FLAGS+=-m32 $(SANITIZERS) -fuse-ld=gold

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEPS=$(OBJS:%.o=%.d)

BIN=fs_create fs_info fs_add fs_del fs_list fs_cat fs_read fs_fileinfo
BIN_OBJS=$(BIN:%=%.o)
NONBIN_OBJS=$(filter-out $(BIN_OBJS),$(OBJS))

all: $(BIN)

%.o: %.c
	$(CC) $(CC_FLAGS) -I.. $< -o $@

fs_fileinfo: fs_fileinfo.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_create: fs_create.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_info: fs_info.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_add: fs_add.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_cat: fs_cat.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_read: fs_read.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_del: fs_del.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

fs_list: fs_list.o $(NONBIN_OBJS) $(COMMON_OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

clean:
	$(MAKE) -C $(COMMON_DIR) clean
	/bin/rm -f $(BIN) $(OBJS) $(DEPS)

check:
	cppcheck --force -I .. -I /usr/include --enable=all --language=c --platform=unix32 --std=c11 .

-include $(DEPS)
