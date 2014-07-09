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
LOCAL_MODULE:= libnvdrmplugin
LOCAL_MODULE_OWNER := nagravision
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= parseMpdHelpers.cpp nvDrmPlugin.cpp
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib/drm

LOCAL_C_INCLUDES += \
	$(TOP)/frameworks/native/include/utils \
	$(TOP)/frameworks/av/include \
	$(TOP)/frameworks/av/drm/libdrmframework/plugins/common/include \
	$(TOP)/external/libxml2/include \
	$(TOP)/external/icu4c/common \
	$(TOP)/external/stlport/stlport \
	$(TOP)/bionic \
	$(TOP)/bionic/libstdc++/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../../../cores/drmkernel/include

LOCAL_SHARED_LIBRARIES := \
	libdrmframework \
	libutils \
	libbinder \
	libicuuc \
	liblog \
	libcutils \
	libstlport \
	libstdc++ \
	libdl

LOCAL_STATIC_LIBRARIES := \
	libnvdrmkernel \
	libdrmframeworkcommon \
	libxml2

include $(BUILD_SHARED_LIBRARY)
