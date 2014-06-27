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

#include <vector>
#include <string>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define LOG_NDEBUG 0
#define LOG_TAG "parseMpdHelper"
#include <utils/Log.h>

using namespace std;

/*
 * getAttrByName
 */
static xmlAttrPtr
getAttrByName(xmlAttrPtr cur, const char *name)
{
  while (cur)
    {
      if (!strcmp(name, (const char *)cur->name))
	return cur;
      
      cur = cur->next;
    }
  return (xmlAttrPtr)NULL;
}

/*
 * getNodeByNameAndType
 */
static xmlNodePtr
getNodeByNameAndType(xmlNodePtr cur, const char *name, xmlElementType type)
{
  while (cur)
    {
      if (cur->type == type && 
	  (name == (const char *)NULL || !strcmp(name, (const char *)cur->name)))
	return cur;
      
      xmlNodePtr ret = getNodeByNameAndType(cur->children, name, type);
      if (ret != NULL)
	return ret;
      cur = cur->next;
    }
  return (xmlNodePtr)NULL;
}

/*
 * findCencElement
 */
bool
findCencElement(xmlNodePtr cur)
{
  xmlNodePtr contentProtectionElement = getNodeByNameAndType(cur, 
							     "ContentProtection", 
							     XML_ELEMENT_NODE);
  while (contentProtectionElement)
    {
      xmlAttrPtr schemeIdUriAttr = getAttrByName(contentProtectionElement->properties, 
						 "schemeIdUri");
      if (schemeIdUriAttr)
	{
	  /* CENC: look at value attr */
	  const char* value = (const char*)xmlNodeListGetString(contentProtectionElement->doc, 
						   schemeIdUriAttr->children, 1);
	  ALOGV("findCencElement: schemeIdUriAttr value = %s\n", value);
	  if (!strcmp(value, "urn:mpeg:dash:mp4protection:2011"))
	    {
	      xmlAttrPtr valueAttr = getAttrByName(contentProtectionElement->properties, 
						   "value");
	      const char* cenc = (const char*)xmlNodeListGetString(contentProtectionElement->doc, 
						      valueAttr->children, 1);
	      ALOGV("findCencElement: value cenc = %s\n", cenc);
	      if (!strcmp(cenc, "cenc"))
		/* Cipher scheme is CENC, now check if DRM use case is supported */
		return true;
	    }
	}
      if (findCencElement(contentProtectionElement->children))
	return true;

      contentProtectionElement = contentProtectionElement->next;
    }
  return false;
}

/*
 * findDrmScheme
 */
bool
findDrmScheme(xmlNodePtr cur)
{
  xmlNodePtr contentProtectionElement = getNodeByNameAndType(cur, 
							     "ContentProtection", 
							     XML_ELEMENT_NODE);
  while (contentProtectionElement)
    {
      xmlAttrPtr schemeIdUriAttr = getAttrByName(contentProtectionElement->properties, 
						 "schemeIdUri");
      if (schemeIdUriAttr)
	{
	  /* CENC: look at value attr */
	  const char* value = (const char*)xmlNodeListGetString(contentProtectionElement->doc, 
								schemeIdUriAttr->children, 1);
	  ALOGV("findDrmScheme: schemeIdUriAttr content = %s\n", value);
	  if (!strcmp(value, "urn:uuid:9a04f079-9840-4286-ab92-e65be0885f95") ||
	      !strcmp(value, "urn:uuid:00000000-0000-0000-0000-000000000001"))
	    return true;
	}
      if (findDrmScheme(contentProtectionElement->children))
	return true;

      contentProtectionElement = contentProtectionElement->next;
    }
  return false;
}

char* 
find_extension(string mString)
{
    const char* lastSlash;
    const char* lastDot;
    int extLen;
    const char* const str = (const char* const)mString.c_str();

    // only look at the filename
    lastSlash = strrchr(str, '/');
    if (lastSlash == NULL)
        lastSlash = str;
    else
        lastSlash++;

    // find the last dot
    lastDot = strrchr(lastSlash, '.');
    if (lastDot == NULL)
        return NULL;

    // looks good, ship it
    return const_cast<char*>(lastDot);
}

string 
getPathExtension(string str) 
{
    char* ext;

    ext = find_extension(str);
    if (ext != NULL)
        return string(ext);
    else
        return string("");
}

/*
 * Try to find media mimeType either through explicit mimeType
 * provided as:
 *   a 'Representation' element attribute, 'mimeType', containing a mimeType
 *   a text node enclosed in a 'BaseURL' element, containing an URL to a media
 *   a 'SegmentTemplate' element attribute, 'media', containing a URL template for segment media
 *
 * TODO: try to exploit codec codes (FourCC and other profiles as in avc1.640028, mp4a.40.2)
 */
bool
findSupportedMediaUri(xmlNodePtr cur)
{
  vector<xmlNodePtr> reprElts;
  vector<xmlNodePtr> adaptElts;
  vector<xmlNodePtr> baseUrlElts;
  vector<xmlNodePtr> segTmplElts;

  vector<string> mimeTypes;
  vector<string> baseUrls;

  vector<xmlNodePtr>::iterator it;
  vector<string>::iterator it1;

  /*
   * Representation/mimeType
   */
  ALOGV("findMediaUri: Seeking Representation elements ...\n");
  xmlNodePtr pRootNode = cur;
  it = reprElts.begin();
  do 
    {
      if (cur = getNodeByNameAndType(cur, "Representation", XML_ELEMENT_NODE))
	{
	  reprElts.insert(it, cur);
	  it = reprElts.begin();
	  if (cur->next) cur = cur->next;
	}
    }
  while(cur);
  ALOGV("findMediaUri: found %ld Representation elements\n", reprElts.size());

  it1 = mimeTypes.begin();
  for (vector<xmlNodePtr>::iterator it = reprElts.begin();
       it != reprElts.end();
       ++it)
    {
      xmlAttrPtr curAttr;
      if (curAttr = getAttrByName(((xmlNodePtr)*it)->properties, "mimeType"))
	{
	  const char* mime = (const char*)xmlNodeListGetString(((xmlNodePtr)*it)->doc, 
							       curAttr->children, 1);
	  ALOGV("findMediaUri: found mime %s in attr %s\n", mime, "mimeType");
	  mimeTypes.insert(it1, string(mime));
	  it1 = mimeTypes.begin();
	}
      /* Only one mime type per attribute */
    }
  ALOGV("findMediaUri: Added %ld mime types\n", mimeTypes.size());

  /*
   * AdaptationSet/mimeType
   */
  ALOGV("findMediaUri: Seeking AdaptationSet elements ...\n");
  cur = pRootNode;
  it = adaptElts.begin();
  do 
    if (cur = getNodeByNameAndType(cur, "AdaptationSet", XML_ELEMENT_NODE))
      {
	adaptElts.insert(it, cur);
	it = adaptElts.begin();
	if (cur->next) cur = cur->next;
      }
  while(cur);
  ALOGV("findMediaUri: found %ld AdaptationSet elements\n", adaptElts.size());

  it1 = mimeTypes.begin();
  for (vector<xmlNodePtr>::iterator it = adaptElts.begin();
       it != adaptElts.end();
       ++it)
    {
      xmlAttrPtr curAttr;
      if (curAttr = getAttrByName(((xmlNodePtr)*it)->properties, "mimeType"))
	{
	  const char* mime = (const char*)xmlNodeListGetString(((xmlNodePtr)*it)->doc, 
							       curAttr->children, 1);
	  ALOGV("findMediaUri: found mime %s in attr %s\n", mime, "mimeType");
	  mimeTypes.insert(it1, string(mime));
	  it1 = mimeTypes.begin();
	}
      /* Only one mime type per attribute */
    }
  ALOGV("findMediaUri: Added %ld mime types\n", mimeTypes.size());

  /*
   * BaseURL/textnode
   */
  ALOGV("findMediaUri: Seeking BaseURL elements ...\n");
  cur = pRootNode;
  it = baseUrlElts.begin();
  do 
    if (cur = getNodeByNameAndType(cur, "BaseURL", XML_ELEMENT_NODE))
      {
	baseUrlElts.insert(it, cur);
	it = baseUrlElts.begin();
	if (cur->next) cur = cur->next;
      }
  while(cur);
  ALOGV("findMediaUri: found %ld BaseURL elements\n", baseUrlElts.size());

  it1 = baseUrls.begin();
  for (vector<xmlNodePtr>::iterator it = baseUrlElts.begin();
       it != baseUrlElts.end();
       ++it)
    {
      xmlNodePtr textElt;
      if (textElt = getNodeByNameAndType((xmlNodePtr)*it, (const char *)NULL, XML_TEXT_NODE))
	{
	  ALOGV("findMediaUri: found url %s in elt %s\n", 
		textElt->content, ((xmlNodePtr)*it)->name);
	  baseUrls.insert(it1, string((const char *)textElt->content));
	  it1 = baseUrls.begin();
	}
      /* Only one mime type per attribute */
    }
  ALOGV("findMediaUri: Added %ld urls\n", baseUrls.size());
      
  /*
   * SegmentTemplate/media
   */
  ALOGV("findMediaUri: Seeking SegmentTemplate elements ...\n");
  cur = pRootNode;
  it = segTmplElts.begin();
  do 
    if (cur = getNodeByNameAndType(cur, "SegmentTemplate", XML_ELEMENT_NODE))
      {
	segTmplElts.insert(it, cur);
	it = segTmplElts.begin();
	if (cur->next) cur = cur->next;
      }
  while(cur);
  ALOGV("findMediaUri: found %ld SegmentTemplate elements\n", segTmplElts.size());

  it1 = baseUrls.begin();
  for (vector<xmlNodePtr>::iterator it = segTmplElts.begin();
       it != segTmplElts.end();
       ++it)
    {
      xmlAttrPtr curAttr;
      if (curAttr = getAttrByName(((xmlNodePtr)*it)->properties, "media"))
	{
	  const char* url = (const char*)xmlNodeListGetString(((xmlNodePtr)*it)->doc, 
							       curAttr->children, 1);
	  ALOGV("findMediaUri: found url %s in attr %s\n", url, "media");
	  baseUrls.insert(it1, string(url));
	  it1 = baseUrls.begin();
	}
      /* Only one mime type per attribute */
    }
  ALOGV("findMediaUri: Added %ld urls\n", mimeTypes.size());

  ALOGV("findMediaUri: found %ld mimeTypes and %ld URLs\n", 
	mimeTypes.size(), baseUrls.size());
  bool ret = true;
  for (vector<string>::iterator it = mimeTypes.begin();
       it != mimeTypes.end();
       ++it)
    {
      ALOGV("findMediaUri: testing mime type %s\n", (*it).c_str());
      bool supported = ((string)*it == string("video/mp4") ||
			(string)*it == string("audio/mp4") ||
			(string)*it == string("audio/m4a") ||
			(string)*it == string("video/m4v") ||
			(string)*it == string("audio/mpa") ||
			(string)*it == string("video/mpv") ||
			(string)*it == string("video/uvv") ||
			(string)*it == string("audio/uva"));
      ALOGV("findMediaUri: *** mime type %s in this MPD are %ssupported\n", (*it).c_str(), supported ? "" : "NOT ");
      ret = ret && supported;
    }
  for (vector<string>::iterator it = baseUrls.begin();
       it != baseUrls.end();
       ++it)
    {
      ALOGV("findMediaUri: testing url %s\n", (*it).c_str());
      string ext = getPathExtension((string)*it);
      bool supported = true;
      if (ext.length())
	{
	  supported = (ext == ".mp4" ||
		       ext == ".m4a" ||
		       ext == ".m4v" ||
		       ext == ".mpa" ||
		       ext == ".mpv" ||
		       ext == ".uvv" ||
		       ext == ".uva");
	  ALOGV("findMediaUri: *** url %s %ssupported\n", (*it).c_str(), supported ? "" : "NOT ");
	}
      else
	ALOGV("findMediaUri: *** no extension in url %s (base url): discarded\n", (*it).c_str());
      ret = ret && supported;
    }

  ALOGV("findMediaUri: >>> Media found in this MPD are %ssupported\n", ret ? "" : "NOT ");

  return ret;
}

