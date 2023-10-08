# Makefile for AbstractMachine Kernels and Libraries

### *Get a more readable version of this Makefile* by `make html` (requires python-markdown)
### 查找makefile文件内容并
html:
	cat Makefile | sed 's/^\([^#]\)/    \1/g' | markdown_py > Makefile.html
.PHONY: html

## 1. Basic Setup and Checks

### Default to create a bare-metal kernel image
### makecmdgoals 如果为空，则将其设置为image，及那个default_goal同时设置为image
ifeq ($(MAKECMDGOALS),)
  MAKECMDGOALS  = image
  .DEFAULT_GOAL = image
endif

### Override checks when `make clean/clean-all/html`
### 发现string image, clean/clean-all/html
### 在clean|clean-all|html中查找字符串image，存在则返回，否则返回空
ifeq ($(findstring $(MAKECMDGOALS),clean|clean-all|html),)

### 
### Print build info message
## 打印信息，信息是build的名称以及image以及arch
$(info # Building $(NAME)-$(MAKECMDGOALS) [$(ARCH)])

### Check: environment variable `$AM_HOME` looks sane
### 检查环境变量
### 如果是*.c，在表达式中就是*.c，但在wildcardd中就是所有字符
### 还可以检查一个东西是否存在，这里就是这个变量的通配展示
ifeq ($(wildcard $(AM_HOME)/am/include/am.h),)
  $(error $$AM_HOME must be an AbstractMachine repo)
endif

### Check: environment variable `$ARCH` must be in the supported list
### 将所有的makefile名字中的前缀拿出来
### 将所有的mk文件的名称拿出来之后又将.mk删了
ARCHS = $(basename $(notdir $(shell ls $(AM_HOME)/scripts/*.mk)))
ifeq ($(filter $(ARCHS), $(ARCH)), )
  $(error Expected $$ARCH in {$(ARCHS)}, Got "$(ARCH)")
endif

### 将ARCH中所有的-替换为空格
### 获得所有ARCH_SPLIT中的第一个和第二个字符
### Extract instruction set architecture (`ISA`) and platform from `$ARCH`. Example: `ARCH=x86_64-qemu -> ISA=x86_64; PLATFORM=qemu`
ARCH_SPLIT = $(subst -, ,$(ARCH))
ISA        = $(word 1,$(ARCH_SPLIT))
PLATFORM   = $(word 2,$(ARCH_SPLIT))

### 使用flavor不需要给变量加上$符号
### 1.当输入变量在makefile及其include中都没有时，函数输出为’undefined’字符串
### 2.当输入变量在makefile及其include中有，且是用于了循环的变量时，输出’recursive’
### 3.当输入变量在makefile及其include中有且不是循环变量时，函数输出为’simple’

### Check if there is something to build
ifeq ($(flavor SRCS), undefined)
  $(error Nothing to build)
endif

### Checks end here
endif

## 2. General Compilation Targets

### 获得pwd
### 获得现在的架构
### 创建一个这个架构的文件夹
### Create the destination directory (`build/$ARCH`)
WORK_DIR  = $(shell pwd)
DST_DIR   = $(WORK_DIR)/build/$(ARCH)
$(shell mkdir -p $(DST_DIR))


### Compilation targets (a binary image or archive)
IMAGE_REL = build/$(NAME)-$(ARCH)

### 获得一个东西的绝对路径
### 获得IMAGE_REL的绝对路径
IMAGE     = $(abspath $(IMAGE_REL))
ARCHIVE   = $(WORK_DIR)/build/$(NAME)-$(ARCH).a
### pwd/
### Collect the files to be linked: object files (`.o`) and libraries (`.a`)
### 添加一个架构的前缀，添加一个.o的后缀
### 其中添加者是SRCS的没有.后缀的部分
OBJS      = $(addprefix $(DST_DIR)/, $(addsuffix .o, $(basename $(SRCS))))
### 为$LIBS中的所有的单词以首字母为排序标准排序，同时将重复的去掉
LIBS     := $(sort $(LIBS) am klib) # lazy evaluation ("=") causes infinite recursions
LINKAGE   = $(OBJS) \
  $(addsuffix -$(ARCH).a, $(join \
    $(addsuffix /build/, $(addprefix $(AM_HOME)/, $(LIBS))), \
    $(LIBS) ))

## 3. General Compilation Flags

### (Cross) compilers, e.g., mips-linux-gnu-g++
AS        = $(CROSS_COMPILE)gcc
CC        = $(CROSS_COMPILE)gcc
CXX       = $(CROSS_COMPILE)g++
LD        = $(CROSS_COMPILE)ld
OBJDUMP   = $(CROSS_COMPILE)objdump
OBJCOPY   = $(CROSS_COMPILE)objcopy
READELF   = $(CROSS_COMPILE)readelf

### Compilation flags
### 添加了前缀-I
INC_PATH += $(WORK_DIR)/include $(addsuffix /include/, $(addprefix $(AM_HOME)/, $(LIBS)))
INCFLAGS += $(addprefix -I, $(INC_PATH))

CFLAGS   += -O2 -MMD -Wall -Werror $(INCFLAGS) \
            -D__ISA__=\"$(ISA)\" -D__ISA_$(shell echo $(ISA) | tr a-z A-Z)__ \
            -D__ARCH__=$(ARCH) -D__ARCH_$(shell echo $(ARCH) | tr a-z A-Z | tr - _) \
            -D__PLATFORM__=$(PLATFORM) -D__PLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z | tr - _) \
            -DARCH_H=\"arch/$(ARCH).h\" \
            -fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector \
            -Wno-main -U_FORTIFY_SOURCE
CXXFLAGS +=  $(CFLAGS) -ffreestanding -fno-rtti -fno-exceptions
ASFLAGS  += -MMD $(INCFLAGS)
LDFLAGS  += -z noexecstack

## 4. Arch-Specific Configurations

### Paste in arch-specific configurations (e.g., from `scripts/x86_64-qemu.mk`)
### 引入了一个makefile文件
### 又引入了两个文件

# CROSS_COMPILE := riscv64-linux-gnu-
# COMMON_FLAGS  := -fno-pic -march=rv64g -mcmodel=medany -mstrict-align
# CFLAGS        += $(COMMON_FLAGS) -static
# ASFLAGS       += $(COMMON_FLAGS) -O0
# LDFLAGS       += -melf64lriscv

# AM_SRCS := platform/nemu/trm.c \
#            platform/nemu/ioe/ioe.c \
#            platform/nemu/ioe/timer.c \
#            platform/nemu/ioe/input.c \
#            platform/nemu/ioe/gpu.c \
#            platform/nemu/ioe/audio.c \
#            platform/nemu/ioe/disk.c \
#            platform/nemu/mpe.c

# CFLAGS    += -fdata-sections -ffunction-sections
# LDFLAGS   += -T $(AM_HOME)/scripts/linker.ld \
#              --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
# LDFLAGS   += --gc-sections -e _start
# NEMUFLAGS += -l $(shell dirname $(IMAGE).elf)/nemu-log.txt

# CFLAGS += -DMAINARGS=\"$(mainargs)\"
# CFLAGS += -I$(AM_HOME)/am/src/platform/nemu/include
# .PHONY: $(AM_HOME)/am/src/platform/nemu/trm.c

# image: $(IMAGE).elf
# 	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
# 	@echo + OBJCOPY "->" $(IMAGE_REL).bin
# 	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

# run: image
# 	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin

# gdb: image
# 	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) gdb ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin

### 同时还定义了h文件以及AM_SRCS文件
# CFLAGS  += -DISA_H=\"riscv/riscv.h\"

# AM_SRCS += riscv/nemu/start.S \
#            riscv/nemu/cte.c \
#            riscv/nemu/trap.S \
#            riscv/nemu/vme.c

-include $(AM_HOME)/scripts/$(ARCH).mk

### 在PATH变量指定的路径中搜索某个系统命令的位置并且返回第一个搜索结果
### 查找CC的位置
### Fall back to native gcc/binutils if there is no cross compiler
ifeq ($(wildcard $(shell which $(CC))),)
  $(info #  $(CC) not found; fall back to default gcc and binutils)
  CROSS_COMPILE :=
endif

## 5. Compilation Rules

### Rule (compile): a single `.c` -> `.o` (gcc)
$(DST_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo + CC $<
	@$(CC) -std=gnu11 $(CFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.cc` -> `.o` (g++)
$(DST_DIR)/%.o: %.cc
	@mkdir -p $(dir $@) && echo + CXX $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.cpp` -> `.o` (g++)
$(DST_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) && echo + CXX $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.S` -> `.o` (gcc, which preprocesses and calls as)
$(DST_DIR)/%.o: %.S
	@mkdir -p $(dir $@) && echo + AS $<
	@$(AS) $(ASFLAGS) -c -o $@ $(realpath $<)

### Rule (recursive make): build a dependent library (am, klib, ...)
$(LIBS): %:
	@$(MAKE) -s -C $(AM_HOME)/$* archive

### Rule (link): objects (`*.o`) and libraries (`*.a`) -> `IMAGE.elf`, the final ELF binary to be packed into image (ld)
$(IMAGE).elf: $(OBJS) am $(LIBS)
	@echo + LD "->" $(IMAGE_REL).elf
	@$(LD) $(LDFLAGS) -o $(IMAGE).elf --start-group $(LINKAGE) --end-group

### Rule (archive): objects (`*.o`) -> `ARCHIVE.a` (ar)
$(ARCHIVE): $(OBJS)
	@echo + AR "->" $(shell realpath $@ --relative-to .)
	@ar rcs $(ARCHIVE) $(OBJS)

### Rule (`#include` dependencies): paste in `.d` files generated by gcc on `-MMD`
-include $(addprefix $(DST_DIR)/, $(addsuffix .d, $(basename $(SRCS))))

## 6. Miscellaneous

### Build order control
image: image-dep
archive: $(ARCHIVE)
image-dep: $(OBJS) am $(LIBS)
	@echo \# Creating image [$(ARCH)]
# 表明一个东西是伪命令，也就是说，他可以不用生成文件，只需要执行命令
# 但是伪目标是可以成为依赖的
.PHONY: image image-dep archive run $(LIBS)

### Clean a single project (remove `build/`)
clean:
	rm -rf Makefile.html $(WORK_DIR)/build/
.PHONY: clean

### Clean all sub-projects within depth 2 (and ignore errors)

CLEAN_ALL = $(dir $(shell find . -mindepth 2 -name Makefile))
clean-all: $(CLEAN_ALL) clean
$(CLEAN_ALL):
	-@$(MAKE) -s -C $@ clean
.PHONY: clean-all $(CLEAN_ALL)
