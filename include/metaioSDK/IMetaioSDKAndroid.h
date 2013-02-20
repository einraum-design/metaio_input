/*
 *  IMetaioSDKAndroid.h
 *	metaio SDK v4.0
 *
 *  Copyright 2012 metaio GmbH. All rights reserved.
 *
 */
 
#ifndef ___AS_IMETAIO_SDK_ANDROID_H__
#define ___AS_IMETAIO_SDK_ANDROID_H__

#include <metaioSDK/IMetaioSDK.h>
#include <metaioSDK/IAudioCallback.h>
#include <jni.h>

namespace metaio
{

	/** 
	 * Specialized interface for Android.
	 * 
	 */
	class IMetaioSDKAndroid : public virtual IMetaioSDK
	{
	public:

		/**
		* Initialize the renderer
		*		
		* \param width width of the renderer
		* \param height height of the renderer
		* \param renderSystem To use a specify OpenGL version or the NullRenderer
		*/
		virtual void initializeRenderer( int width, int height, 
			const ERENDER_SYSTEM renderSystem=ERENDER_SYSTEM_OPENGL_ES_2_0) = 0;

		/**
		* Reload all textures.
		* 
		* On Android we need to reload textures every time the OpenGL 
		* surface has been paused.
		*/
		virtual void reloadTextures() = 0;

		/**
		* Get the CPU type as string. 
		*
		* This function is mostly for logging purposes.
		*
		* \return The CPU type as std::string.
		*/
		virtual std::string getCPUType() = 0;
		
		/**
		* Set the audio callback.
		* \param audioCallback The callback implementation.
		*/
		virtual void registerAudioCallback( metaio::IAudioCallback* audioCallback ) = 0;
	};




	/**
	* \brief Create an IMetaioSDKAndroid instance.
	*
	* relates IMetaioSDKAndroid
	* 
	* \param activity Android activity in which this instance is created and used
	* \param appSignature The signature of the app.
	* \return Pointer to the IMetaioSDKAndroid instance on success
	*/
	IMetaioSDKAndroid* CreateMetaioSDKAndroid(jobject activity, std::string appSignature);


} //namespace metaio


#endif //___AS_IMETAIO_SDK_ANDROID_H__
