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

SRCS= \
	DrmKernel.c \
	nvDatabase.c \
	nvDatabaseSecureTable.c
STATIC_LIB= libnvdrmkernel.a
INCLUDES= \
	-I $(TOP)/external/sqlite/dist \
	-I $(TOP)/vendor/nagravision/libs/cores/drmkernel/src/../include \
	-I $(TOP)/vendor/nagravision/libs/cores/drmkernel/src \
	-I $(TOP)/libnativehelper/include/nativehelper \
	-I $(TOP)/build/core/combo/include/arch/linux-arm
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
	-isystem $(TOP)/external/openssl/include \
	-isystem $(TOP)/bionic/libc/arch-arm/include \
	-isystem $(TOP)/bionic/libc/include \
	-isystem $(TOP)/bionic/libstdc++/include \
	-isystem $(TOP)/bionic/libc/kernel/common \
	-isystem $(TOP)/bionic/libc/kernel/arch-arm \
	-isystem $(TOP)/bionic/libm/include \
	-isystem $(TOP)/bionic/libm/include/arm \
	-isystem $(TOP)/bionic/libthread_db/include \
	-include $(TOP)/build/core/combo/include/arch/linux-arm/AndroidConfig.h
GCC_FLAGS= -fno-exceptions -Wno-multichar -msoft-float -fpic -fPIE -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=2 -fno-short-enums -march=armv7-a -mfloat-abi=softfp -mfpu=neon -Wno-unused-but-set-variable -fno-builtin-sin -fno-strict-volatile-bitfields -Wno-psabi -mthumb-interwork -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -DNDEBUG -g -Wstrict-aliasing=2 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -DNDEBUG -UDEBUG -Os -fomit-frame-pointer -fno-strict-aliasing   -fvisibility=hidden

include $(TOP)/settings.mk

.PHONY: all clean

%.o: %.c
	$(GCC) $(INCLUDES) $(SYS_INCLUDES) $(GCC_FLAGS) -c $< -o $@

all: $(STATIC_LIB)

clean:
	rm -rf $(STATIC_LIB)
	rm -rf *.o *~ \#*\# .\#* *.P *.d

$(STATIC_LIB): $(SRCS:%.c=%.o)
	$(AR) crsP $@ $(SRCS:%.c=%.o)
