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

#ifdef USE_LIBXML2
#include <libxml/tree.h>
#endif
#include <DrmEngineBase.h>

#ifndef SYM_EXPORT
#define SYM_EXPORT	__attribute__ ((visibility ("default")))
#endif

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
    SYM_EXPORT
    DrmConstraints* onGetConstraints(int uniqueId, 
				     const String8* path, 
				     int action);

    /*
     * Provides DRM metadata associated to a content
     */
    SYM_EXPORT
    DrmMetadata* onGetMetadata(int uniqueId, 
			       const String8* path);

    /*
     * 
     */
    SYM_EXPORT
    status_t onInitialize(int uniqueId);

    /*
     * 
     */
    SYM_EXPORT
    status_t onSetOnInfoListener(int uniqueId, 
				 const IDrmEngine::OnInfoListener* infoListener);

    /*
     * 
     */
    SYM_EXPORT
    status_t onTerminate(int uniqueId);

    /*
     * Provides hint about ability to support a content. Based on file
     * extension, mime type, or, if file is a media presentation description,
     * mime types and extensions obtained from its parsing.
     */
    SYM_EXPORT
    bool onCanHandle(int uniqueId, 
		     const String8& path);

    /*
     * 
     */
    SYM_EXPORT
    DrmInfoStatus* onProcessDrmInfo(int uniqueId, 
				    const DrmInfo* drmInfo);

    /*
     * 
     */
    SYM_EXPORT
    status_t onSaveRights(int uniqueId, 
			  const DrmRights& drmRights,
			  const String8& rightsPath, 
			  const String8& contentPath);

    /*
     * 
     */
    SYM_EXPORT
    DrmInfo* onAcquireDrmInfo(int uniqueId, 
			      const DrmInfoRequest* drmInfoRequest);

    /*
     * 
     */
    SYM_EXPORT
    String8 onGetOriginalMimeType(int uniqueId, 
				  const String8& path, 
				  int fd);

    /*
     * 
     */
    SYM_EXPORT
    int onGetDrmObjectType(int uniqueId, 
			   const String8& path, 
			   const String8& mimeType);

    /*
     * 
     */
    SYM_EXPORT
    int onCheckRightsStatus(int uniqueId, 
			    const String8& path,
			    int action);

    /*
     * 
     */
    SYM_EXPORT
    status_t onConsumeRights(int uniqueId,
			     DecryptHandle* decryptHandle, 
			     int action, 
			     bool reserve);

    /*
     * 
     */
    SYM_EXPORT
    status_t onSetPlaybackStatus(int uniqueId, 
				 DecryptHandle* decryptHandle, 
				 int playbackStatus, 
				 int64_t position);

    /*
     * 
     */
    SYM_EXPORT
    bool onValidateAction(int uniqueId, 
			  const String8& path, 
			  int action, 
			  const ActionDescription& description);

    /*
     * 
     */
    SYM_EXPORT
    status_t onRemoveRights(int uniqueId, 
			    const String8& path);

    /*
     * 
     */
    SYM_EXPORT
    status_t onRemoveAllRights(int uniqueId);

    /*
     * 
     */
    SYM_EXPORT
    status_t onOpenConvertSession(int uniqueId, 
				  int convertId);

    /*
     * 
     */
    SYM_EXPORT
    DrmConvertedStatus* onConvertData(int uniqueId, 
				      int convertId, 
				      const DrmBuffer* inputData);

    /*
     * 
     */
    SYM_EXPORT
    DrmConvertedStatus* onCloseConvertSession(int uniqueId, 
					      int convertId);

    /*
     * 
     */
    SYM_EXPORT
    DrmSupportInfo* onGetSupportInfo(int uniqueId);

    /*
     * 
     */
    SYM_EXPORT
    status_t onOpenDecryptSession(int uniqueId, 
				  DecryptHandle* decryptHandle, 
				  int fd, 
				  off64_t offset, 
				  off64_t length);

    /*
     * 
     */
    SYM_EXPORT
    status_t onOpenDecryptSession(int uniqueId, 
				  DecryptHandle* decryptHandle, 
				  const char* uri);

    /*
     * 
     */
    SYM_EXPORT
    status_t onCloseDecryptSession(int uniqueId, 
				   DecryptHandle* decryptHandle);

    /*
     * 
     */
    SYM_EXPORT
    status_t onInitializeDecryptUnit(int uniqueId, 
				     DecryptHandle* decryptHandle,
				     int decryptUnitId, 
				     const DrmBuffer* headerInfo);

    /*
     * 
     */
    SYM_EXPORT
    status_t onDecrypt(int uniqueId, 
		       DecryptHandle* decryptHandle, 
		       int decryptUnitId,
		       const DrmBuffer* encBuffer, 
		       DrmBuffer** decBuffer, 
		       DrmBuffer* IV);

    /*
     * 
     */
    SYM_EXPORT
    status_t onFinalizeDecryptUnit(int uniqueId, 
				   DecryptHandle* decryptHandle, 
				   int decryptUnitId);

    /*
     * 
     */
    SYM_EXPORT
    ssize_t onPread(int uniqueId, 
		    DecryptHandle* decryptHandle,
		    void* buffer, 
		    ssize_t numBytes, 
		    off64_t offset);

  private:
#ifdef USE_LIBXML2
    /*
     * Media Presentation Description (MPD) parsing
     */
    bool parseMpd(const String8 &path);
#endif

    /*
     * Open a decrypt session
     */
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
