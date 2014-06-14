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

#ifndef __UUID_H_
#define __UUID_H_

#define UUID_LEN     16
#define UUID_STR_LEN 37

#include <stdio.h>

int uuid_str2bin(const char *str, uint8_t *bin);
int uuid_bin2str(const uint8_t *bin, char *str, size_t max_len);
int uuid_is_nil(const uint8_t *uuid);

#endif /* __UUID_H_ */
