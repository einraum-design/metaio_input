#ifndef CAMERAIMAGERENDERER_H
#define CAMERAIMAGERENDERER_H

#include <gl/GL.h>

#include <metaioSDK/MobileStructs.h>


class CameraImageRenderer
{
	public:
		CameraImageRenderer();
		~CameraImageRenderer();

		void draw(float screenAspect);
		void updateFrame(metaio::ImageStruct* cameraFrame);

		float getScaleX() { return m_scaleX; }
		float getScaleY() { return m_scaleY; }

	protected:
		// The camera image is usually 640x480 or similar, so its dimensions are not a power of 2. As we
		// want to render the camera image, we just create a texture that has 2^n dimensions and copy
		// the camera image over a part of it.
		
		/// Aspect ratio of the camera image (width/height)
		float						m_cameraAspect;
		/// Width of image as it captured from the camera
		unsigned int				m_cameraImageWidth;
		/// Height of image as it captured from the camera
		unsigned int				m_cameraImageHeight;
		/// Whether the camera frames are upside down
		bool						m_cameraUpsideDown;

		GLuint						m_cameraTexture;
		BYTE*						m_pCameraTextureBuffer;
		bool						m_cameraTextureAllocated;
		/// Width of the texture we created to render the camera image (2^n)
		unsigned int				m_cameraTextureWidth;
		/// Height of the texture we created to render the camera image (2^n)
		unsigned int				m_cameraTextureHeight;

		/**
		 * Value by which the X axis must be scaled in the overall projection matrix in order to make
		 * up for a aspect-corrected (by cropping) camera image. Set on each draw() call.
		 */
		float						m_scaleX;
	
		float						m_scaleY;
};

#endif