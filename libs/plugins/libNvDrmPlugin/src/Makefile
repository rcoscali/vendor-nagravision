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
	nvDrmPlugin.cpp \
	drmNvToDroid.cpp \
	drmDroidToNv.cpp
SHARED_LIBRARY= libnvdrmplugin.so
GCC_FLAGS= -fno-exceptions -Wno-multichar -msoft-float -fpic -fPIE -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=2 -fno-short-enums -march=armv7-a -mfloat-abi=softfp -mfpu=neon -include $(TOP)/build/core/combo/include/arch/linux-arm/AndroidConfig.h -Wno-unused-but-set-variable -fno-builtin-sin -fno-strict-volatile-bitfields -Wno-psabi -mthumb-interwork -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -DNDEBUG -g -Wstrict-aliasing=2 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -DNDEBUG -UDEBUG -fvisibility-inlines-hidden -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Wsign-promo -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -DNDEBUG -UDEBUG -Os -fomit-frame-pointer -fno-strict-aliasing  -fno-rtti -fvisibility=hidden 
INCLUDES= \
	-I $(TOP)/frameworks/native/include/utils \
	-I $(TOP)/frameworks/av/include \
	-I $(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
	-I $(TOP)/external/icu4c/common \
	-I $(TOP)/external/stlport/stlport \
	-I $(TOP)/external/sqlite/dist \
	-I $(TOP)/bionic \
	-I $(TOP)/bionic/libstdc++/include \
	-I $(TOP)/vendor/nagravision/libs/plugins/libNvDrmPlugin/src/../include \
	-I $(TOP)/vendor/nagravision/libs/plugins/libNvDrmPlugin/src/../../../cores/drmkernel/include \
	-I $(TOP)/vendor/nagravision/libs/plugins/libNvDrmPlugin/src \
	-I $(TOP)/libnativehelper/include/nativehelper  \
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
	-isystem $(TOP)/bionic/libc/arch-arm/include \
	-isystem $(TOP)/bionic/libc/include \
	-isystem $(TOP)/bionic/libstdc++/include \
	-isystem $(TOP)/bionic/libc/kernel/common \
	-isystem $(TOP)/bionic/libc/kernel/arch-arm \
	-isystem $(TOP)/bionic/libm/include \
	-isystem $(TOP)/bionic/libm/include/arm \
	-isystem $(TOP)/bionic/libthread_db/include

LD_FLAGS= -nostdlib -Wl,-soname,$(SHARED_LIBRARY) -Wl,--gc-sections -Wl,-shared,-Bsymbolic -L$(TOP)/objs/lib 
PROLOGUE= \
	$(TOP)/objs/lib/crtbegin_so.o
EPILOGUE= \
	$(TOP)/objs/lib/crtend_so.o

LIDLIBS_FLAGS= -Wl,--whole-archive   -Wl,--no-whole-archive   
LDLIBS= \
	-ldrmframeworkcommon \
	-ldrmframework \
	-lutils \
	-lbinder \
	-lsqlite \
	-lcrypto \
	-licuuc \
	-llog \
	-lcutils \
	-lstlport \
	-lstdc++ \
	-ldl \
	-lc \
	-lstdc++ \
	-lm
LDFLAGS= -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings -Wl,--icf=safe -Wl,--fix-cortex-a8   -Wl,--no-undefined
LIBGCC= $(TOP)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/../lib/gcc/arm-linux-androideabi/4.7/armv7-a/libgcc.a
DRMCORELIB=$(TOP)/vendor/nagravision/libs/cores/drmkernel/src/libnvdrmkernel.a

include $(TOP)/settings.mk

%.o: %.cpp
	$(CCACHE) $(GPLUSPLUS) $(INCLUDES) $(SYS_INCLUDES) $(GCC_FLAGS) -c $< -o $@

.PHONY: all clean

all: $(SHARED_LIBRARY)

clean:
	rm -rf $(SHARED_LIBRARY)
	rm -rf *.o *~ \#*\# .\#* *.P *.d

$(SHARED_LIBRARY): $(SRCS:%.cpp=%.o)
	($(CCACHE) $(GPLUSPLUS) $(LD_FLAGS) $(PROLOGUE) $(SRCS:%.cpp=%.o) $(LDLIBS_FLAGS) $(DRMCORELIB) $(LDLIBS) -o $@ $(LDFLAGS) $(LIBGCC) $(EPILOGUE); \
	 mkdir -p $(TOP)/objs/system/vendor/lib/mediadrm; \
	 cp -vf $(SHARED_LIBRARY) $(TOP)/objs/system/vendor/lib/mediadrm; \
	)
