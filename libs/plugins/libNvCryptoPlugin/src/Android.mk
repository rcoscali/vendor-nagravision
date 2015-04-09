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

WITHOUT_LIBCOMPILER_RT = yes

LOCAL_MODULE:= libnvcryptoplugin
LOCAL_MODULE_OWNER := nagravision
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib/mediadrm
LOCAL_SRC_FILES:= nvCryptoPlugin.cpp nvCryptoFactory.cpp

LOCAL_C_INCLUDES:= 			 \
	$(TOP)/frameworks/native/include/utils \
	$(TOP)/frameworks/av/include 	 \
	$(TOP)/frameworks/native/include 	 \
	$(TOP)/frameworks/native/include/media \
	$(TOP)/system/core/include \
	$(LOCAL_PATH)/../../../utils/include \
	$(LOCAL_PATH)/../../../cores/cryptokernel/include \
	$(LOCAL_PATH)/../include

LOCAL_ADDITIONAL_DEPENDENCIES := \
	libnvcryptokernel \
	libnvutils

LOCAL_STATIC_LIBRARIES := \
	libnvcryptokernel \
	libnvutils

LOCAL_SHARED_LIBRARIES += \
	libnvdrmplugin \
        libstagefright_foundation \
	libcrypto \
	liblog \
	libutils \
        libdl

include $(BUILD_SHARED_LIBRARY)

