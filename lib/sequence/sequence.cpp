/*
	This file is part of Warzone 2100.
	Copyright (C) 2008-2011  Warzone 2100 Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "3rdparty/theoraplay/theoraplay.h"

#include "lib/framework/frame.h"
#include "lib/framework/frameint.h"
#include "lib/framework/opengl.h"
#include "lib/framework/wzapp.h"
#include "lib/ivis_opengl/piestate.h"
#include "lib/sound/mixer.h"
#include "lib/sound/openal_error.h"
#include "sequence.h"
#include "timer.h"

#include "3rdparty/theoraplay/theoraplay.h"
#include "3rdparty/theoraplay/theoraplay_physfs.h"


static THEORAPLAY_Decoder *decoder = NULL;
static const THEORAPLAY_VideoFrame *video = NULL;
static const THEORAPLAY_AudioPacket *audio = NULL;

static bool videoplaying = false;

static const int texture_width = 1024;
static const int texture_height = 1024;
static GLuint video_texture;

// Screen dimensions
static int videoX1 = 0;
static int videoX2 = 0;
static int videoY1 = 0;
static int videoY2 = 0;
static int ScrnvidXpos = 0;
static int ScrnvidYpos = 0;

static SCANLINE_MODE use_scanlines;

double seq_GetFrameTime()
{
	return 0;
}

static void Allocate_videoFrame(void)
{
	glGenTextures(1, &video_texture);
	glBindTexture(GL_TEXTURE_2D, video_texture);
	char *blackframe = (char *)calloc(1, texture_width * texture_height * 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, blackframe);
	free(blackframe);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

static void deallocateVideoFrame(void)
{
	glDeleteTextures(1, &video_texture);
}

// main routine to display video on screen.
static void video_write(bool update)
{
	const int video_width = video->width;
	const int video_height = video->height;

	glErrors();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, video_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (update)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video->width, video->height, GL_RGB, GL_UNSIGNED_BYTE, video->pixels);
		glErrors();
	}

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glPushMatrix();

	glTranslatef(ScrnvidXpos, ScrnvidYpos, 0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(videoX1, videoY1);
	glTexCoord2f((float) video_width / texture_width, 0);
	glVertex2f(videoX2, videoY1);				//screenWidth
	glTexCoord2f((float) video_width / texture_width, (float) video_height / texture_height);
	glVertex2f(videoX2, videoY2);		//screenWidth,screenHeight
	glTexCoord2f(0, (float) video_height / texture_height);
	glVertex2f(videoX1, videoY2);				//screenHeight
	glEnd();

	glPopMatrix();

	// Make sure the current texture page is reloaded after we are finished
	// Otherwise WZ will think it is still loaded and not load it again
	pie_SetTexturePage(TEXPAGE_NONE);
}

bool seq_Play(const char* filename)
{
	debug(LOG_VIDEO, "starting playback of: %s", filename);

	if (videoplaying)
	{
		debug(LOG_VIDEO, "previous movie is not yet finished");
		seq_Shutdown();
	}

	decoder = THEORAPLAY_startDecodePhysfs(filename, 30, THEORAPLAY_VIDFMT_RGB);
	if (decoder <= 0)
	{
		debug(LOG_VIDEO, "Couldn't create theoraplay decoder");
		seq_Shutdown();
	}

	do
	{
		if (!video)
			video = THEORAPLAY_getVideo(decoder);
		if (!audio)
			audio = THEORAPLAY_getAudio(decoder);
		wzYieldCurrentThread();
	}
	while (!video || !audio);

	if (video->width > texture_width || video->height > texture_height)
	{
		debug(LOG_ERROR, "Video size too large, must be below %dx%d!",
				texture_width, texture_height);
		return false;
	}

	Allocate_videoFrame();


	videoplaying = true;
	return true;
}

bool seq_Playing()
{
	return videoplaying;
}

/**
 * Display the next frame and play the sound.
 * \return false if the end of the video is reached.
 */
bool seq_Update()
{
	debug(LOG_VIDEO, "%s:%d", __FILE__, __LINE__);
	if (!videoplaying)
	{
		debug(LOG_VIDEO, "no movie playing");
		return false;
	}

	if (!audio)
		audio = THEORAPLAY_getAudio(decoder);
	while (audio)
	{
		const THEORAPLAY_AudioPacket *old = audio;
		audio = audio->next;
		THEORAPLAY_freeAudio(old);
	}

	if (!video)
		video = THEORAPLAY_getVideo(decoder);
	if (video)
	{
		video_write(true);
		wzYieldCurrentThread();
		const THEORAPLAY_VideoFrame *old = video;
		video = video->next;
		THEORAPLAY_freeVideo(old);
		return true;
	}
	return false;
}

void seq_Shutdown()
{
	debug(LOG_VIDEO, "%s:%d", __FILE__, __LINE__);

	if (!videoplaying)
	{
		debug(LOG_VIDEO, "movie is not playing");
		return;
	}

	videoplaying = false;
	Timer_stop();

	THEORAPLAY_freeVideo(video);
	video = NULL;
	THEORAPLAY_freeAudio(audio);
	audio = NULL;
	THEORAPLAY_stopDecode(decoder);
	pie_SetTexturePage(-1);
	deallocateVideoFrame();
}

// this controls the size of the video to display on screen
void seq_SetDisplaySize(int sizeX, int sizeY, int posX, int posY)
{
	videoX1 = 0;
	videoY1 = 0;
	videoX2 = sizeX;
	videoY2 = sizeY;

	if (sizeX > 640 || sizeY > 480)
	{
		const float aspect = screenWidth / (float)screenHeight, videoAspect = 4 / (float)3;

		if (aspect > videoAspect)
		{
			int offset = (screenWidth - screenHeight * videoAspect) / 2;
			videoX1 += offset;
			videoX2 -= offset;
		}
		else
		{
			int offset = (screenHeight - screenWidth / videoAspect) / 2;
			videoY1 += offset;
			videoY2 -= offset;
		}
	}

	ScrnvidXpos = posX;
	ScrnvidYpos = posY;
}

void seq_setScanlineMode(SCANLINE_MODE mode)
{
    use_scanlines = mode;
}

SCANLINE_MODE seq_getScanlineMode(void)
{
    return use_scanlines;
}
