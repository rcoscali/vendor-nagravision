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

#ifndef __NV_DRM_PLUGIN_H__
#define __NV_DRM_PLUGIN_H__

#include <libxml/tree.h>
#include <DrmEngineBase.h>

namespace android 
{
  /*
   * Class providing implementation of Nagra DRM use cases. It integrates
   * in Android DRM framework for:
   *  - provisioning (personalization/individualization)
   *  - licensing (rights acquisition/storage)
   *  - keys (provided to Media framework for content descrambling) 
   */
  class NvDrmPlugin : public DrmEngineBase {

  public:
    /*
     * Public constructor
     */
    NvDrmPlugin();

    /*
     * Virtual destructor
     */
    virtual ~NvDrmPlugin();

  protected:
    /*
     * Provides constraints associated to a content for the action
     */
    DrmConstraints* onGetConstraints(int uniqueId, 
				     const String8* path, 
				     int action);

    /*
     * Provides DRM metadata associated to a content
     */
    DrmMetadata* onGetMetadata(int uniqueId, 
			       const String8* path);

    /*
     * 
     */
    status_t onInitialize(int uniqueId);

    /*
     * 
     */
    status_t onSetOnInfoListener(int uniqueId, 
				 const IDrmEngine::OnInfoListener* infoListener);

    /*
     * 
     */
    status_t onTerminate(int uniqueId);

    /*
     * Provides hint about ability to support a content. Based on file
     * extension, mime type, or, if file is a media presentation description,
     * mime types and extensions obtained from its parsing.
     */
    bool onCanHandle(int uniqueId, 
		     const String8& path);

    /*
     * 
     */
    DrmInfoStatus* onProcessDrmInfo(int uniqueId, 
				    const DrmInfo* drmInfo);

    /*
     * 
     */
    status_t onSaveRights(int uniqueId, 
			  const DrmRights& drmRights,
			  const String8& rightsPath, 
			  const String8& contentPath);

    /*
     * 
     */
    DrmInfo* onAcquireDrmInfo(int uniqueId, 
			      const DrmInfoRequest* drmInfoRequest);

    /*
     * 
     */
    String8 onGetOriginalMimeType(int uniqueId, 
				  const String8& path, 
				  int fd);

    /*
     * 
     */
    int onGetDrmObjectType(int uniqueId, 
			   const String8& path, 
			   const String8& mimeType);

    /*
     * 
     */
    int onCheckRightsStatus(int uniqueId, 
			    const String8& path,
			    int action);

    /*
     * 
     */
    status_t onConsumeRights(int uniqueId,
			     DecryptHandle* decryptHandle, 
			     int action, 
			     bool reserve);

    /*
     * 
     */
    status_t onSetPlaybackStatus(int uniqueId, 
				 DecryptHandle* decryptHandle, 
				 int playbackStatus, 
				 int64_t position);

    /*
     * 
     */
    bool onValidateAction(int uniqueId, 
			  const String8& path, 
			  int action, 
			  const ActionDescription& description);

    /*
     * 
     */
    status_t onRemoveRights(int uniqueId, 
			    const String8& path);

    /*
     * 
     */
    status_t onRemoveAllRights(int uniqueId);

    /*
     * 
     */
    status_t onOpenConvertSession(int uniqueId, 
				  int convertId);

    /*
     * 
     */
    DrmConvertedStatus* onConvertData(int uniqueId, 
				      int convertId, 
				      const DrmBuffer* inputData);

    /*
     * 
     */
    DrmConvertedStatus* onCloseConvertSession(int uniqueId, 
					      int convertId);

    /*
     * 
     */
    DrmSupportInfo* onGetSupportInfo(int uniqueId);

    /*
     * 
     */
    status_t onOpenDecryptSession(int uniqueId, 
				  DecryptHandle* decryptHandle, 
				  int fd, 
				  off64_t offset, 
				  off64_t length);

    /*
     * 
     */
    status_t onOpenDecryptSession(int uniqueId, 
				  DecryptHandle* decryptHandle, 
				  const char* uri);

    /*
     * 
     */
    status_t onCloseDecryptSession(int uniqueId, 
				   DecryptHandle* decryptHandle);

    /*
     * 
     */
    status_t onInitializeDecryptUnit(int uniqueId, 
				     DecryptHandle* decryptHandle,
				     int decryptUnitId, 
				     const DrmBuffer* headerInfo);

    /*
     * 
     */
    status_t onDecrypt(int uniqueId, 
		       DecryptHandle* decryptHandle, 
		       int decryptUnitId,
		       const DrmBuffer* encBuffer, 
		       DrmBuffer** decBuffer, 
		       DrmBuffer* IV);

    /*
     * 
     */
    status_t onFinalizeDecryptUnit(int uniqueId, 
				   DecryptHandle* decryptHandle, 
				   int decryptUnitId);

    /*
     * 
     */
    ssize_t onPread(int uniqueId, 
		    DecryptHandle* decryptHandle,
		    void* buffer, 
		    ssize_t numBytes, 
		    off64_t offset);

  private:
<<<<<<< HEAD

    /*
     * Media Presentation Description (MPD) parsing
     */
    bool parseMpd(const String8 &path);

    /*
     * Open a decrypt session
     */
=======
    bool parseMpd(const String8 &path);

>>>>>>> c16cfe270e0a762bbe2920bc4372ebb27b42d67b
    DecryptHandle* openDecryptSessionImpl();

    /*
     * Plugin metadata instance
     */
    DrmMetadata *mNvDrmMetadata;

  public:
    static String8 mimeTypesFind(const char *key);
    static String8 drmSchemesFind(const char *key);
    static String8 fileExtsFind(const char *key);
    
    // and constants for values initialization
    static const int N_METADATA = 2;
    static const char* const sNvMetadata[];
    static String8 cencUuid;
  };

};

#endif /* __NV_DRM_PLUGIN_H__ */
