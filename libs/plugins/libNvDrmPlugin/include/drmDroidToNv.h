/*
 * Copyright (C) 2014 NagraVision
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DRM_DROID_TO_NV_H__
#define DRM_DROID_TO_NV_H__

#include <drm/DrmRights.h>
#include <drm/DrmConstraints.h>
#include <drm/DrmMetadata.h>
#include <drm/DrmInfo.h>
#include <drm/DrmInfoEvent.h>
#include <drm/DrmInfoStatus.h>
#include <drm/DrmConvertedStatus.h>
#include <drm/DrmInfoRequest.h>
#include <drm/DrmSupportInfo.h>

#include "DrmKernel.h"

#ifdef __cplusplus
extern "C" 
{
  using namespace android;
#endif

  struct NV_DrmInfo_st *DrmInfo_droid2nv(const DrmInfo *);
  struct NV_DrmRights_st *DrmRights_droid2nv(const DrmRights *);
  struct NV_DrmInfoRequest_st *DrmInfoRequest_droid2nv(const DrmInfoRequest *);

#ifdef __cplusplus
}
#endif

#endif /* DRM_DROID_TO_NV_H__ */
