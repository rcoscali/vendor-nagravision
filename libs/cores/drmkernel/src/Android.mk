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
LOCAL_PATH:= $(call my-dir)

## Library drmkernel
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	DrmKernel.c 		\
	nvDatabase.c 		\
	nvDatabaseSecureTable.c

LOCAL_C_INCLUDES += \
	$(TOP)/external/openssl/include \
	$(TOP)/external/sqlite/dist \
	$(LOCAL_PATH)/../include

LOCAL_MODULE:= libnvdrmkernel

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -fvisibility=hidden

LOCAL_SHARED_LIBRARIES := \
    libsqlite \
    liblog

include $(BUILD_STATIC_LIBRARY)

## Target Test checkRecordCrypto 
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/checkRecordCrypto.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= DrmKernelTest_checkRecordCrypto
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libcrypto

include $(BUILD_EXECUTABLE)

## Host Test checkRecordCrypto 
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/checkRecordCrypto.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= HostDrmKernelTest_checkRecordCrypto
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DDRM_HOST_KERNEL_TEST=1 -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libcrypto

include $(BUILD_HOST_EXECUTABLE)

## Target Test onSaveRights
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/onSaveRights.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= DrmKernelTest_onSaveRights
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libcrypto

include $(BUILD_EXECUTABLE)

## Host Test onSaveRights
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/onSaveRights.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= HostDrmKernelTest_onSaveRights
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DDRM_HOST_KERNEL_TEST=1 -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libcrypto

include $(BUILD_HOST_EXECUTABLE)

## Target Test onCheckRightsStatus
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/onCheckRightsStatus.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= DrmKernelTest_onCheckRightsStatus
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libcrypto

include $(BUILD_EXECUTABLE)

## Host Test onCheckRightsStatus
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/onCheckRightsStatus.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= HostDrmKernelTest_onCheckRightsStatus
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DDRM_HOST_KERNEL_TEST=1 -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libcrypto

include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/dbTest.c \
	DrmKernel.c \
	nvDatabase.c \
	nvDatabaseSecureTable.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= DrmKernelTest_db
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libsqlite \
    libcrypto \
    liblog

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	tests/dbTest.c \
	DrmKernel.c \
	nvDatabase.c \
	nvDatabaseSecureTable.c
LOCAL_C_INCLUDES += \
	$(TOP)/external/sqlite/dist \
	$(TOP)/external/openssl/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../include
LOCAL_MODULE:= HostDrmKernelTest_db
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DDRM_HOST_KERNEL_TEST=1 -fvisibility=hidden
LOCAL_SHARED_LIBRARIES := \
    libsqlite \
    libcrypto

include $(BUILD_HOST_EXECUTABLE)
