#ifndef _INCL_THEORAPLAY_PHYSFS_H_
#define _INCL_THEORAPLAY_PHYSFS_H_

#include <physfs.h>

static long IoPhysfsRead(THEORAPLAY_Io *io, void *buffer, long bufferlength)
{
	PHYSFS_file *filehandle = (PHYSFS_file*)io->userdata;
	const size_t bytesread = PHYSFS_read(filehandle, buffer, 1, bufferlength);
	return bytesread;
}

static void IoPhysfsClose(THEORAPLAY_Io *io)
{
	PHYSFS_close((PHYSFS_file*)io->userdata);
}

static THEORAPLAY_Decoder *THEORAPLAY_startDecodePhysfs(const char *filename, const unsigned int maxframes, THEORAPLAY_VideoFormat videoformat)
{
	THEORAPLAY_Io *io = (THEORAPLAY_Io *)malloc(sizeof(THEORAPLAY_Io));
	if (io == NULL)
		return NULL;

	static PHYSFS_file *filehandle = PHYSFS_openRead(filename);
	if (filehandle == NULL)
	{
		free(io);
		return NULL;
	}

	io->read = IoPhysfsRead;
	io->close = IoPhysfsClose;
	io->userdata = filehandle;
	return THEORAPLAY_startDecode(io, maxframes, videoformat);
}

#endif
