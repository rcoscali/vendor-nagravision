#
# Copyright (C) 2014 NagraVision
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Targets to build for this makefile
STATIC_LIB= libnvcryptokernel.a
TEST_EXE=  CryptoKernelTest_decrypt
HOST_TEST_EXE= HostCryptoKernelTest_decrypt

# Sources for static lib target 
STATIC_LIB_SRCS= CryptoKernel.c

# Sources for test exe
TEST_EXE_SRCS = CryptoKernel.c tests/decrypt.c
TEST_EXE_OBJS = target_test/CryptoKernel.o target_test/decrypt.o
HOST_TEST_EXE_OBJS = \
	host_test/CryptoKernel.o host_test/decrypt.o \
	host_test/DrmKernel.o \
	host_test/nvDatabase.o \
	host_test/nvDatabaseSecureTable.o

# Includes for static lib on target
INCLUDES= \
	-I $(TOP)/frameworks/native/include/utils \
	-I $(TOP)/frameworks/av/include \
	-I $(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
	-I $(TOP)/external/openssl/include \
	-I $(TOP)/bionic \
	-I $(TOP)/bionic/libstdc++/include \
	-I $(TOP)/vendor/nagravision/libs/cores/cryptokernel/src/../include \
	-I $(TOP)/vendor/nagravision/libs/cores/cryptokernel/src \
	-I $(TOP)/libnativehelper/include/nativehelper  \
	-I $(TOP)/build/core/combo/include/arch/linux-arm
# Includes for test exe on target
TEST_INCLUDES= \
	-I $(TOP)/frameworks/native/include \
	-I $(TOP)/system/core/include \
	-I $(TOP)/frameworks/av/include \
	-I $(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
	-I $(TOP)/external/openssl/include \
	-I $(TOP)/external/sqlite/dist \
	-I $(TOP)/bionic \
	-I $(TOP)/bionic/libstdc++/include \
	-I ../../drmkernel/include \
	-I ../include \
	-I . \
	-I $(TOP)/libnativehelper/include/nativehelper
# System includes for target builds
SYS_INCLUDES= \
	-isystem $(TOP)/system/core/include \
	-isystem $(TOP)/hardware/libhardware/include \
	-isystem $(TOP)/hardware/libhardware_legacy/include \
	-isystem $(TOP)/hardware/ril/include \
	-isystem $(TOP)/libnativehelper/include \
	-isystem $(TOP)/frameworks/native/include \
	-isystem $(TOP)/frameworks/native/opengl/include \
	-isystem $(TOP)/frameworks/av/include \
	-isystem $(TOP)/frameworks/base/include \
	-isystem $(TOP)/external/skia/include \
	-isystem $(TOP)/bionic/libc/arch-arm/include \
	-isystem $(TOP)/bionic/libc/include \
	-isystem $(TOP)/bionic/libstdc++/include \
	-isystem $(TOP)/bionic/libc/kernel/common \
	-isystem $(TOP)/bionic/libc/kernel/arch-arm \
	-isystem $(TOP)/bionic/libm/include \
	-isystem $(TOP)/bionic/libm/include/arm \
	-isystem $(TOP)/bionic/libthread_db/include \
	-include $(TOP)/build/core/combo/include/arch/linux-arm/AndroidConfig.h

# Cflags for target
GCC_FLAGS= -fno-exceptions -Wno-multichar -msoft-float -fpic -fPIE -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=2 -fno-short-enums -march=armv7-a -mfloat-abi=softfp -mfpu=neon -Wno-unused-but-set-variable -fno-builtin-sin -fno-strict-volatile-bitfields -Wno-psabi -mthumb-interwork -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -DNDEBUG -g -Wstrict-aliasing=2 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -DNDEBUG -UDEBUG -Os -fomit-frame-pointer -fno-strict-aliasing   -std=c99 -fvisibility=hidden 
# Cflags for host
HOST_GCC_CFLAGS= -fno-exceptions -Wno-multichar -mstackrealign -msse3 -mfpmath=sse -fPIC -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -O2 -g -fno-strict-aliasing -DNDEBUG -UDEBUG   -DDUMP_SUBSAMPLES -std=c99 -fvisibility=hidden
# LD flags & libs for target
LDLIBS= -lsqlite -lcrypto -llog -lc -lstdc++ -lm
LDFLAGS= -nostdlib -Bdynamic -fPIE -pie -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -L$(TOP)/objs/lib -Wl,-rpath-link=$(TOP)/objs/lib 
# LD flags & libs for host 
HOST_LDLIBS= -Wl,--whole-archive -Wl,--no-whole-archive -L/usr/lib/x86_64-linux-gnu -lsqlite -lcrypto
HOST_LDFLAGS=  -Wl,--no-undefined     

# Target dynamic exe prologue & epilogue
DYNAMIC_EXE_PROLOGUE=$(TOP)/objs/lib/crtbegin_dynamic.o
DYNAMIC_EXE_EPILOGUE=$(TOP)/objs/lib/crtend_android.o

include $(TOP)/settings.mk

.PHONY: all clean libnvdrmkernel.a

# Generic rules
%.o: %.c
	$(GCC) $(INCLUDES) $(SYS_INCLUDES) $(GCC_FLAGS) -c $< -o $@

target_test/%.o: %.c
	$(GCC) $(TEST_INCLUDES) $(SYS_INCLUDES) $(GCC_FLAGS) -c $< -o $@

target_test/%.o: tests/%.c
	$(GCC) $(TEST_INCLUDES) $(SYS_INCLUDES) $(GCC_FLAGS) -c $< -o $@

host_test/%.o: %.c
	$(HOST_GCC) -m64 $(TEST_INCLUDES) -DDRM_HOST_KERNEL_TEST=1 -DCRYPTO_HOST_KERNEL_TEST=1 $(HOST_GCC_FLAGS) -c $< -o $@

host_test/%.o: tests/%.c
	$(HOST_GCC) -m64 $(TEST_INCLUDES) -DCRYPTO_HOST_KERNEL_TEST=1 $(HOST_GCC_FLAGS) -c $< -o $@

host_test/%.o: ../../drmkernel/src/%.c
	$(HOST_GCC) -m64 $(TEST_INCLUDES) -DDRM_HOST_KERNEL_TEST=1 $(HOST_GCC_FLAGS) -c $< -o $@

# Entry rule
all: $(STATIC_LIB) target_test $(TEST_EXE) host_test $(HOST_TEST_EXE)

# Objs dirs
target_test host_test:
	-@mkdir $@

# Cleanup
clean:
	rm -rf $(STATIC_LIB)
	rm -rf *.o *~ \#*\# .\#* *.P *.d
	rm -rf target_test host_test

# Target static lib
$(STATIC_LIB): $(STATIC_LIB_SRCS:%.c=%.o)
	$(AR) crsP $@ $(STATIC_LIB_SRCS:%.c=%.o)

# Tests exe depends on drmkernel. Build it through make
libnvdrmkernel.a: 
	make -C ../../drmkernel/src $@

# Test exe for target
$(TEST_EXE): $(TEST_EXE_OBJS) libnvdrmkernel.a
	$(GCC) $(LDFLAGS) $(DYNAMIC_EXE_PROLOGUE) $(TEST_EXE_OBJS) -Wl,--whole-archive -Wl,--no-whole-archive $(STATIC_LIB) ../../drmkernel/src/libnvdrmkernel.a $(LDLIBS) -o CryptoKernelTest_decrypt -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings -Wl,--icf=safe -Wl,--fix-cortex-a8 -Wl,--no-undefined $(TOP)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-$(GCC_VERSION)/lib/gcc/arm-linux-androideabi/$(GCC_VERSION)/armv7-a/libgcc.a $(DYNAMIC_EXE_EPILOGUE)

# Test exe for Host
$(HOST_TEST_EXE): $(HOST_TEST_EXE_OBJS)
	$(HOST_GCC) -m64 $(HOST_TEST_EXE_OBJS) $(HOST_LDLIBS) $(HOST_LDFLAGS)-o $@
