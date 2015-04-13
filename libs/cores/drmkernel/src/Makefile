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


STATIC_LIB= libnvdrmkernel.a

TESTS = \
	DrmKernelTest_checkRecordCrypto \
	DrmKernelTest_onSaveRights \
	DrmKernelTest_onCheckRightsStatus \
	DrmKernelTest_db

HOST_TESTS = \
	HostDrmKernelTest_checkRecordCrypto \
	HostDrmKernelTest_onSaveRights \
	HostDrmKernelTest_onCheckRightsStatus \
	HostDrmKernelTest_db

SRCS= \
	DrmKernel.c \
	nvDatabase.c \
	nvDatabaseSecureTable.c

CHECK_RECORD_CRYPTO_SRCS= tests/checkRecordCrypto.c
CHECK_RECORD_CRYPTO_OBJS= target_tests/checkRecordCrypto.o
HOST_CHECK_RECORD_CRYPTO_OBJS= host_tests/checkRecordCrypto.o

ON_SAVE_RIGHTS_SRCS= tests/onSaveRights.c
ON_SAVE_RIGHTS_OBJS= target_tests/onSaveRights.o
HOST_ON_SAVE_RIGHTS_OBJS= host_tests/onSaveRights.o

ON_CHECK_RIGHTS_STATUS_SRCS= tests/onCheckRightsStatus.c
ON_CHECK_RIGHTS_STATUS_OBJS= target_tests/onCheckRightsStatus.o
HOST_ON_CHECK_RIGHTS_STATUS_OBJS= host_tests/onCheckRightsStatus.o

DB_TEST_SRCS= tests/dbTest.c DrmKernel.c nvDatabase.c nvDatabaseSecureTable.c
DB_TEST_OBJS= target_tests/dbTest.o target_tests/DrmKernel.o target_tests/nvDatabase.o target_tests/nvDatabaseSecureTable.o
HOST_DB_TEST_OBJS= host_tests/dbTest.o host_tests/DrmKernel.o host_tests/nvDatabase.o host_tests/nvDatabaseSecureTable.o

INCLUDES= \
	-I $(TOP)/external/sqlite/dist \
	-I $(TOP)/vendor/nagravision/libs/cores/drmkernel/src/../include \
	-I $(TOP)/vendor/nagravision/libs/cores/drmkernel/src \
	-I $(TOP)/libnativehelper/include/nativehelper \
	-I $(TOP)/build/core/combo/include/arch/linux-arm
TEST_INCLUDES= $(INCLUDES) \
	-I $(TOP)/external/openssl/include \
	-I $(TOP)/system/core/include \
	-I $(TOP)/frameworks/native/include
HOST_TEST_INCLUDES= \
	-I $(TOP)/external/sqlite/dist \
	-I $(TOP)/vendor/nagravision/libs/cores/drmkernel/src/../include \
	-I $(TOP)/vendor/nagravision/libs/cores/drmkernel/src \
	-I $(TOP)/libnativehelper/include/nativehelper \
	-I $(TOP)/external/openssl/include \
	-I $(TOP)/system/core/include \
	-I $(TOP)/frameworks/native/include \
	-include $(TOP)/build/core/combo/include/arch/linux-x86/AndroidConfig.h 
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
	-isystem $(TOP)/bionic/libthread_db/include

GCC_FLAGS= -fno-exceptions -Wno-multichar -msoft-float -fpic -fPIE -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=2 -fno-short-enums -march=armv7-a -mfloat-abi=softfp -mfpu=neon -Wno-unused-but-set-variable -fno-builtin-sin -fno-strict-volatile-bitfields -Wno-psabi -mthumb-interwork -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -DNDEBUG -g -Wstrict-aliasing=2 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -DNDEBUG -UDEBUG -Os -fomit-frame-pointer -fno-strict-aliasing   -fvisibility=hidden
GCC_TEST_FLAGS= -fno-exceptions -Wno-multichar -msoft-float -fpic -fPIE -ffunction-sections -fdata-sections -funwind-tables -fstack-protector -Wa,--noexecstack -Werror=format-security -D_FORTIFY_SOURCE=1 -fno-short-enums -march=armv7-a -mfloat-abi=softfp -mfpu=neon -Wno-psabi -mthumb-interwork -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point  -Wno-unused-parameter -DNDEBUG -g -Wstrict-aliasing=2 -fgcse-after-reload -frerun-cse-after-loop -frename-registers -DNDEBUG -UDEBUG -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing   -fvisibility=hidden     
HOST_GCC_TEST_FLAGS=-fno-exceptions -Wno-multichar -mstackrealign -msse3 -mfpmath=sse -m64 -fPIC -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith -O2 -g -fno-strict-aliasing -DNDEBUG -UDEBUG   -DDRM_HOST_KERNEL_TEST=1 -fvisibility=hidden
TEST_LDFLAGS=-nostdlib -Bdynamic -fPIE -pie -Wl,-dynamic-linker,/system/bin/linker -Wl,--gc-sections -Wl,-z,nocopyreloc -L$(TOP)/objs/lib -Wl,-rpath-link=$(OBJS)/objs/lib 
TEST_LDLIBS=-Wl,--whole-archive   -Wl,--no-whole-archive  -lcrypto -lc -lm  -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings -Wl,--icf=safe -Wl,--fix-cortex-a8   -Wl,--no-undefined  $(TOP)/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-$(GCC_VERSION)/lib/gcc/arm-linux-androideabi/$(GCC_VERSION)/armv7-a/libgcc.a 
DB_TEST_LDLIBS=-Wl,--whole-archive -Wl,--no-whole-archive -lsqlite -lcrypto -llog -lc -lstdc++ -lm 
DYNAMIC_EXE_PROLOGUE=$(TOP)/objs/lib/crtbegin_dynamic.o         
DYNAMIC_EXE_EPILOGUE=$(TOP)/objs/lib/crtend_android.o

include $(TOP)/settings.mk

.PHONY: all clean

# Generic rules
%.o: %.c
	$(GCC) $(INCLUDES) $(SYS_INCLUDES) $(GCC_FLAGS) -c $< -o $@

target_tests/%.o: tests/%.c
	$(GCC) $(TEST_INCLUDES) $(SYS_INCLUDES) $(GCC_TEST_FLAGS) -c $< -o $@

# Used for dbTest
target_tests/%.o: %.c
	$(GCC) $(TEST_INCLUDES) $(SYS_INCLUDES) $(GCC_TEST_FLAGS) -c $< -o $@

host_tests/%.o: tests/%.c
	$(HOST_GCC) $(HOST_TEST_INCLUDES) $(HOST_GCC_TEST_FLAGS) -c $< -o $@

# Used for dbTest
host_tests/%.o: %.c
	$(HOST_GCC) $(HOST_TEST_INCLUDES) $(HOST_SYS_INCLUDES) $(HOST_GCC_TEST_FLAGS) -c $< -o $@

# Entry rule
all: $(STATIC_LIB) target_tests $(TESTS) host_tests $(HOST_TESTS)

# Target & Host object dirs
target_tests host_tests:
	-@mkdir $@

# Target & Host tests executable
DrmKernelTest_checkRecordCrypto: $(CHECK_RECORD_CRYPTO_OBJS)
	$(GCC) $(TEST_LDFLAGS) $(DYNAMIC_EXE_PROLOGUE) $(CHECK_RECORD_CRYPTO_OBJS) $(TEST_LDLIBS) -o $@ $(DYNAMIC_EXE_EPILOGUE)

HostDrmKernelTest_checkRecordCrypto: $(HOST_CHECK_RECORD_CRYPTO_OBJS)
	$(HOST_GCC) $(HOST_CHECK_RECORD_CRYPTO_OBJS) -lcrypto -Wl,--no-undefined -m64 -o $@

DrmKernelTest_onSaveRights: $(ON_SAVE_RIGHTS_OBJS)
	$(GCC) $(TEST_LDFLAGS) $(DYNAMIC_EXE_PROLOGUE) $(ON_SAVE_RIGHTS_OBJS) $(TEST_LDLIBS) -o $@ $(DYNAMIC_EXE_EPILOGUE)

HostDrmKernelTest_onSaveRights: $(HOST_ON_SAVE_RIGHTS_OBJS)
	$(HOST_GCC) $(HOST_ON_SAVE_RIGHTS_OBJS) -lcrypto -Wl,--no-undefined -m64 -o $@

DrmKernelTest_onCheckRightsStatus: $(ON_CHECK_RIGHTS_STATUS_OBJS)
	$(GCC) $(TEST_LDFLAGS) $(DYNAMIC_EXE_PROLOGUE) $(ON_CHECK_RIGHTS_STATUS_OBJS) $(TEST_LDLIBS) -o $@ $(DYNAMIC_EXE_EPILOGUE)

HostDrmKernelTest_onCheckRightsStatus: $(HOST_ON_CHECK_RIGHTS_STATUS_OBJS)
	$(HOST_GCC) $(HOST_ON_CHECK_RIGHTS_STATUS_OBJS) -lcrypto -Wl,--no-undefined -m64 -o $@

DrmKernelTest_db: $(DB_TEST_OBJS)
	$(GCC) $(TEST_LDFLAGS) $(DYNAMIC_EXE_PROLOGUE) $(DB_TEST_OBJS) $(DB_TEST_LDLIBS) -o $@ $(DYNAMIC_EXE_EPILOGUE)

HostDrmKernelTest_db: $(HOST_DB_TEST_OBJS)
	$(HOST_GCC) $(HOST_DB_TEST_OBJS) -lsqlite3 -lcrypto -Wl,--no-undefined -m64 -o $@

# cleanup target
clean:
	rm -rf $(STATIC_LIB)
	rm -rf *.o *~ \#*\# .\#* *.P *.d
	rm -rf target_tests host_tests

# Static lib build
$(STATIC_LIB): $(SRCS:%.c=%.o)
	$(AR) crsP $@ $(SRCS:%.c=%.o)

# Depends
target_tests/checkRecordCrypto.o: tests/checkRecordCrypto.c DrmKernel.c ../include/DrmKernel.h ../include/nvDatabase.h ../include/nvDatabaseSecureTable.h
host_tests/checkRecordCrypto.o: tests/checkRecordCrypto.c DrmKernel.c ../include/DrmKernel.h ../include/nvDatabase.h ../include/nvDatabaseSecureTable.h
target_tests/onSaveRights.o: tests/onSaveRights.c DrmKernel.c ../include/DrmKernel.h ../include/nvDatabase.h ../include/nvDatabaseSecureTable.h
host_tests/onSaveRights.o: tests/onSaveRights.c DrmKernel.c ../include/DrmKernel.h ../include/nvDatabase.h ../include/nvDatabaseSecureTable.h
target_tests/onCheckRightsStatus.o: tests/onCheckRightsStatus.c DrmKernel.c ../include/DrmKernel.h ../include/nvDatabase.h ../include/nvDatabaseSecureTable.h
host_tests/onCheckRightsStatus.o: tests/onCheckRightsStatus.c DrmKernel.c ../include/DrmKernel.h ../include/nvDatabase.h ../include/nvDatabaseSecureTable.h
