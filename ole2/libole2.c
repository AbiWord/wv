/* The basic I/O functions used (or to be used) by wv, 
 * but with a libole2 back-end.
 * 
 * Justin Bradford (justin@ukans.edu, justin@gene.com) 7/99
 */

#include <stdio.h>
#include "../wv.h"
#include "ms-ole.h"

typedef FILE OLE_STREAM;

S32 ole_tell(OLE_STREAM* stream) {

   MS_OLE_STREAM *ole_stream = (MS_OLE_STREAM*)stream;
   
   if (stream == NULL) {
      errno = EBADF;
      return -1;
   }
   
   return (S32)stream->tell(stream);
}

S16 ole_seek(OLE_STREAM* stream, S32 offset, S16 whence) {
 
   MS_OLE_STREAM *ole_stream = (MS_OLE_STREAM*)stream;
   ms_ole_seek_t ole_whence;

   if (ole_stream == NULL) {
      errno = EBADF;
      return -1;
   }
   
   if (whence == SEEK_SET) ms_ole_seek_t = MS_OLE_SEEK_SET;
   else if (whence == SEEK_CUR) ms_ole_seek_t = MS_OLE_SEEK_CUR;
   else if (whence == SEEK_END) {
      offset += (S32)stream->size;
      ms_ole_seek_t = MS_OLE_SEEK_SET;
   } else {
      errno = EINVAL;
      return -1;
   }
   
   ((MS_OLE_STREAM*)stream)->lseek((MS_OLE_STREAM*)stream, 
				   (gint32)offset, ole_whence);

   return 0;
}

U32 read_32ubit(OLE_STREAM *in) {

   U32 ret;

   U16 temp1 = read_16ubit(in);
   U16 temp2 = read_16ubit(in);

   ret = temp2 << 16;;
   ret += temp1;

   return ret;
}

U16 read_16ubit(OLE_STREAM *in) {

   U16 ret;

   U8 temp1 = (U8)read_8ubit(in);
   U8 temp2 = (U8)read_8ubit(in);

   ret = temp2 << 8;
   ret += temp1;

   return ret;
}

U16 read_8ubit(OLE_STREAM *in) {
   
   U8 ret;
   
   if (in == NULL) {
      return EOF;
   }
      
   if (!in->read_copy(in, (guint8*)(&ret), 1)) return EOF;
   
   return ret;
}
