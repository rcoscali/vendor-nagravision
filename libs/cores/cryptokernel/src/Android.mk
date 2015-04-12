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

include $(CLEAR_VARS)
LOCAL_MODULE:= libnvcryptokernel
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= CryptoKernel.c
LOCAL_CFLAGS := -std=c99 -fvisibility=hidden
LOCAL_C_INCLUDES += \
    $(TOP)/frameworks/native/include/utils \
    $(TOP)/frameworks/av/include \
    $(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
    $(TOP)/external/openssl/include \
    $(TOP)/bionic \
    $(TOP)/bionic/libstdc++/include \
    $(LOCAL_PATH)/../include

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= CryptoKernelTest_decrypt
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= \
    CryptoKernel.c \
    tests/decrypt.c
LOCAL_CFLAGS := -std=c99 -fvisibility=hidden
LOCAL_C_INCLUDES += \
    $(TOP)/frameworks/native/include/utils \
    $(TOP)/frameworks/av/include \
    $(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
    $(TOP)/external/openssl/include \
    $(TOP)/external/sqlite/dist \
    $(TOP)/bionic \
    $(TOP)/bionic/libstdc++/include \
    $(LOCAL_PATH)/../../drmkernel/include \
    $(LOCAL_PATH)/../include

LOCAL_STATIC_LIBRARIES := \
    libnvdrmkernel

LOCAL_SHARED_LIBRARIES := \
    libsqlite \
    libcrypto \
    liblog

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE:= HostCryptoKernelTest_decrypt
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= \
	CryptoKernel.c \
	tests/decrypt.c \
	../../drmkernel/src/DrmKernel.c \
	../../drmkernel/src/nvDatabase.c \
	../../drmkernel/src/nvDatabaseSecureTable.c
LOCAL_CFLAGS := -DDUMP_SUBSAMPLES -DDRM_HOST_KERNEL_TEST=1 -DCRYPTO_HOST_KERNEL_TEST=1 -std=c99 -fvisibility=hidden
LOCAL_C_INCLUDES += \
    $(TOP)/frameworks/native/include/utils \
    $(TOP)/frameworks/av/include \
    $(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
    $(TOP)/external/openssl/include \
    $(TOP)/external/sqlite/dist \
    $(TOP)/bionic \
    $(TOP)/bionic/libstdc++/include \
    $(LOCAL_PATH)/../../drmkernel/include \
    $(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := \
    libsqlite \
    libcrypto

include $(BUILD_HOST_EXECUTABLE)
