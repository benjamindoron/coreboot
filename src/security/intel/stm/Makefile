# SPDX-License-Identifier: BSD-2-Clause

project_name=STM
project_dir=../../../../3rdparty/stm/
build_dir=$(project_dir)/Stm/build
project_git_branch=$(CONFIG_STM_GIT_BRANCH)

ifeq ($(CONFIG_STM_CONSOLE_DEBUG),y)
STM_BUILD="debug"
endif

ifeq ($(CONFIG_STM_CONSOLE_RELEASE),y)
STM_BUILD="release"
endif


all: clean build

build:
	echo "STM - Build"
	echo "--  WARNING: This uses the system toolchain instead of"
	echo "    the coreboot toolchain, so is not reproducible."
	cd $(project_dir)/Stm; \
	mkdir -p build; \
	cd build; \
	cmake .. -DBIOS=coreboot \
		-DUART=$(CONFIG_STM_TTYS0_BASE) \
		-DHEAPSIZE=$(CONFIG_STM_HEAPSIZE) \
		-DCBMEM_ENABLE=$(CONFIG_STM_CBMEM_CONSOLE) \
		-DSTMPE_ENABLED=$(CONFIG_STM_STMPE_ENABLED) \
		-DBUILD=$(STM_BUILD); \
	$(MAKE);

clean:
	test -d $(project_dir) && (cd $(project_dir); rm -rf Stm/build) || exit 0

.PHONY: clean build
