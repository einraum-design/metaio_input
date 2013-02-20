#include <Windows.h>
#include "CameraImageRenderer.h"


static unsigned int getNextPowerOf2(unsigned int n)
{
	for(int i = 0; i < sizeof(n) * 8; ++i)
	{
		if((1U << i) >= n)
			return 1U << i;
	}

	// Should not be reached
	assert(false);
	return 0;
}


CameraImageRenderer::CameraImageRenderer() :
	m_cameraAspect(-1),	
	m_cameraTexture(0),
	m_cameraTextureAllocated(false),
	m_pCameraTextureBuffer(0)
{
	// Prepare display of captured camera image
	glGenTextures(1, &m_cameraTexture);
}


CameraImageRenderer::~CameraImageRenderer()
{
	if(m_cameraTextureAllocated)
		glDeleteTextures(1, &m_cameraTexture);

	delete[] m_pCameraTextureBuffer;
	m_pCameraTextureBuffer = 0;
}


void CameraImageRenderer::draw(float screenAspect)
{
	// If we didn't receive a camera frame yet, do nothing
	if (!m_cameraTextureAllocated)
		return;

	glPushAttrib(GL_ENABLE_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	// Load identity as camera (view) matrix. This is identical to using a calculated "look at" matrix
	// in which the camera looks from the origin along the negative Z axis, as done for example with
	// gluLookAt(0,0,0,  0,0,-1,  0,1,0)
	glLoadIdentity();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_cameraTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(m_cameraUpsideDown)
	{
		glMatrixMode(GL_PROJECTION);
		glScalef(1, -1, 1);
	}

	const float xRatio = (float)m_cameraImageWidth / m_cameraTextureWidth;
	const float yRatio = (float)m_cameraImageHeight / m_cameraTextureHeight;

	// On Windows PCs, there is no idea of screen rotation
	const metaio::ESCREEN_ROTATION screenRotation = metaio::ESCREEN_ROTATION_0;

	// The below code is very similar to the Custom Renderer example on other platforms (e.g. iOS)
	const bool cameraIsRotated = screenRotation == metaio::ESCREEN_ROTATION_90 ||
		                         screenRotation == metaio::ESCREEN_ROTATION_270;
	const float cameraAspect = cameraIsRotated ? 1.0f/m_cameraAspect : m_cameraAspect;

	float offsetX, offsetY;

	if (cameraAspect > screenAspect)
	{
		// Camera image is wider (e.g. 480x640 camera image vs. a 480x800 device, example
		// in portrait mode), so crop the width of the camera image
		float aspectRatio = screenAspect / cameraAspect;
		offsetX = 0.5f * (1 - aspectRatio);
		offsetY = 0;

		m_scaleX = cameraAspect / screenAspect;
		m_scaleY = 1;
	}
	else
	{
		// Screen is wider, so crop the height of the camera image
		float aspectRatio = cameraAspect / screenAspect;
		offsetY = 0.5f * (1 - aspectRatio);
		offsetX = 0;

		m_scaleX = 1;
		m_scaleY = screenAspect / cameraAspect;
	}

	if (cameraIsRotated)
	{
		// Camera image will be rendered with +-90° rotation, so switch UV coordinates
		float tmp = offsetX;
		offsetX = offsetY;
		offsetY = tmp;
	}

	switch (screenRotation)
	{
		// Portrait
		case metaio::ESCREEN_ROTATION_270:
			// Rotate by 90° clockwise
			glRotatef(-90, 0, 0, 1);
			break;

		// Reverse portrait (upside down)
		case metaio::ESCREEN_ROTATION_90:
			// Rotate by 90° counter-clockwise
			glRotatef(90, 0, 0, 1);
			break;

		// Landscape (right side of tall device facing up)
		case metaio::ESCREEN_ROTATION_0:
			break;

		// Reverse landscape (left side of tall device facing up)
		case metaio::ESCREEN_ROTATION_180:
			// Rotate by 180°
			glRotatef(180, 0, 0, 1);
			break;

		default:
			fprintf(stderr, "Unknown screen rotation\n");
	}

	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
		// Calculate texture coordinates. offsetX/offsetY are for cropping if camera and screen
		// aspect ratios differ. xRatio/yRatio are here because the OpenGL texture has
		// dimensions of 2^n, but the camera image does not fill it completely (e.g. camera
		// image 640x480 vs. texture size 1024x512).
		glTexCoord2f((1-offsetX) * xRatio, offsetY * yRatio);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(offsetX * xRatio, offsetY * yRatio);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(offsetX * xRatio, (1-offsetY) * yRatio);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f((1-offsetX) * xRatio,
		             (1-offsetY) * yRatio);
		glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}


void CameraImageRenderer::updateFrame(metaio::ImageStruct* cameraFrame)
{
	glBindTexture(GL_TEXTURE_2D, m_cameraTexture);

	switch(cameraFrame->colorFormat)
	{
		case metaio::common::ECF_B8G8R8:
		{
			if(!m_cameraTextureAllocated)
			{
				m_cameraImageWidth = cameraFrame->width;
				m_cameraImageHeight = cameraFrame->height;
				m_cameraAspect = (float)m_cameraImageWidth / m_cameraImageHeight;

				m_cameraTextureWidth = getNextPowerOf2(m_cameraImageWidth);
				m_cameraTextureHeight = getNextPowerOf2(m_cameraImageHeight);

				m_pCameraTextureBuffer = new BYTE[m_cameraImageWidth * m_cameraImageHeight * 3];

				// Allocate camera image texture once with 2^n dimensions
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGB,
					m_cameraTextureWidth,
					m_cameraTextureHeight,
					0,
					GL_BGR_EXT,
					GL_UNSIGNED_BYTE,
					0);

				m_cameraTextureAllocated = true;
			}

			// ...but only overwrite the camera image-sized region
			m_cameraUpsideDown = !cameraFrame->originIsUpperLeft;
			memcpy(m_pCameraTextureBuffer,
				    cameraFrame->buffer,
				    m_cameraImageWidth * m_cameraImageHeight * 3);

			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				m_cameraImageWidth,
				m_cameraImageHeight,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE,
				m_pCameraTextureBuffer);

			break;
		}

		default:
			fprintf(stderr, "Camera image format not implemented: %d\n", cameraFrame->colorFormat);
			break;
	}
}