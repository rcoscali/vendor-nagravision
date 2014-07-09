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

#include "DrmKernel.h"

struct NV_DrmConstraints_st* 
DrmKernel_NvDrmPlugin_onGetConstraints(int uniqueId, const char *path, int action)
{
  struct NV_DrmConstraints_st *tmp = (struct NV_DrmConstraints_st *)malloc(sizeof(struct NV_DrmConstraints_st));
  if (tmp)
    {
      tmp->next = (struct NV_DrmConstraints_st *)NULL;
      tmp->key = strdup("license_available_time");
      tmp->value = strdup("dummy_available_time");
    }
  return tmp;
}
