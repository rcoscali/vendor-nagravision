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

#ifndef __PARSE_MPD_HELPERS_H_
#define __PARSE_MPD_HELPERS_H_

bool   findDrmScheme(xmlNodePtr cur);
char  *find_extension(string mString);
string getPathExtension(string str);
bool   findSupportedMediaUri(xmlNodePtr cur);

#endif
