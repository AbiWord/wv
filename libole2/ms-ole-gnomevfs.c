/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/**
 * ms-ole.c: MS Office OLE support for Gnome
 *
 * Authors:
 *    Dom Lachowicz <doml@appligent.com>
 *
 * Copyright 2001 Dom Lachowicz
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <fcntl.h>

#ifdef HAVE_IO_H
#include <io.h>			/* use "normal" IO */
#endif

#include <libgnomevfs/gnome-vfs.h>

#include "ms-ole.h"

/*
 * These are small little helper functions
 * To convert fnctl.h flags to GNOME_VFS type flags
 */
static GnomeVFSOpenMode
flags_to_vfs (int flags)
{
	GnomeVFSOpenMode m = GNOME_VFS_OPEN_NONE;
	
	if (flags & O_RDONLY)
		m |= GNOME_VFS_OPEN_READ;
	if (flags & O_WRONLY)
		m |= GNOME_VFS_OPEN_WRITE;

	return m;
}

static GnomeVFSSeekPosition
whence_to_vfs (int whence)
{
	switch (whence)
		{
		case SEEK_SET: return GNOME_VFS_SEEK_START;
		case SEEK_CUR: return GNOME_VFS_SEEK_CURRENT;
		case SEEK_END: return GNOME_VFS_SEEK_END;
		default: return -1;
		}
}

static MsOleHandleType
open2_wrap (const char *pathname, int flags)
{
	GnomeVFSHandle * handle = NULL;
	GnomeVFSOpenMode mode   = flags_to_vfs (flags);
	
	if (gnome_vfs_open (&handle, pathname, mode) == GNOME_VFS_OK)
		{
			if (flags & O_TRUNC)
				gnome_vfs_truncate_handle (handle, 0);
			return handle;
		}
	return BAD_MSOLE_HANDLE;
}

static MsOleHandleType
open3_wrap (const char *pathname, int flags, mode_t mode)
{
	GnomeVFSHandle * handle = NULL;      
	GnomeVFSOpenMode m = flags_to_vfs (flags);
	
	if (flags & O_CREAT)
		{
			if (gnome_vfs_create (&handle, pathname, m, FALSE, mode) == GNOME_VFS_OK)
				{
					if (flags & O_TRUNC)
						gnome_vfs_truncate_handle (handle, 0);
					return handle;
				}
		}
	else
		{
			if (gnome_vfs_open (&handle, pathname, m) == GNOME_VFS_OK)
				{
					if (flags & O_TRUNC)
						gnome_vfs_truncate_handle (handle, 0);
					return handle;
				}
		}
	
	return BAD_MSOLE_HANDLE;
}

static ssize_t
read_wrap (MsOleHandleType fd, void *buf, size_t count)
{
	GnomeVFSFileSize bytes_read = 0;
	
	if (gnome_vfs_read ((GnomeVFSHandle *)fd, buf, (GnomeVFSFileSize)count, &bytes_read) == GNOME_VFS_OK)
		return (ssize_t)bytes_read;
	return -1;
}

static int
close_wrap (MsOleHandleType fd)
{
	return (gnome_vfs_close ((GnomeVFSHandle *)fd) != GNOME_VFS_OK);
}

static ssize_t
write_wrap (MsOleHandleType fd, const void *buf, size_t count)
{
	GnomeVFSFileSize bytes_written = 0;
	
	if (gnome_vfs_write ((GnomeVFSHandle *)fd, buf, (GnomeVFSFileSize)count, &bytes_written) == GNOME_VFS_OK)
		return (ssize_t)bytes_written;
	return -1;
}

static off_t
lseek_wrap (MsOleHandleType fd, off_t offset, int whence)
{
	GnomeVFSSeekPosition w = whence_to_vfs (whence);
	GnomeVFSFileSize o;
	
	if (gnome_vfs_seek ((GnomeVFSHandle *)fd, (GnomeVFSSeekPosition) offset, w) == GNOME_VFS_OK)
		{
			if (gnome_vfs_tell ((GnomeVFSHandle *)fd, &o) == GNOME_VFS_OK)
				return (off_t)o;
		}
	return -1;
}

static int
isregfile_wrap (MsOleHandleType fd)
{
	int rtn = 0;
	GnomeVFSFileInfo * fi = gnome_vfs_file_info_new ();
	
	if (gnome_vfs_get_file_info_from_handle ((GnomeVFSHandle *)fd, fi,
						 GNOME_VFS_FILE_INFO_FIELDS_TYPE) == GNOME_VFS_OK)
		rtn = (fi->type == GNOME_VFS_FILE_TYPE_REGULAR);
	else
		rtn = 0 ;
	gnome_vfs_file_info_unref (fi);
	return rtn;
}

static int
getfilesize_wrap (MsOleHandleType fd, guint32 *size)
{
	int rtn = 0;
	GnomeVFSFileInfo * fi = gnome_vfs_file_info_new ();
	
	if (gnome_vfs_get_file_info_from_handle ((GnomeVFSHandle *)fd, fi,
						 GNOME_VFS_FILE_INFO_FIELDS_SIZE) == GNOME_VFS_OK)
		*size = (int)fi->size;
	else
		rtn = -1;

	gnome_vfs_file_info_unref (fi);
	return rtn;
}

static MsOleSysWrappers gnomevfs_default_wrappers = {
	open2_wrap,
	open3_wrap,
	read_wrap,
	close_wrap,
	write_wrap,
	lseek_wrap,
	isregfile_wrap,	
	getfilesize_wrap,
	NULL,
	NULL
};

MsOleSysWrappers *ms_ole_get_gnomevfs_fs (void)
{
	return &gnomevfs_default_wrappers;
}
