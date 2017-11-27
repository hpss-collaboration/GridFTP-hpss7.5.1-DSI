// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/file.c $ $Id: file.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

PyObject* hpssclapipy_Creat(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int  mode  = 0777;
  int  status;
  hpssclapipy_cos_hints_t *hints = NULL, *hintsout = NULL;
  hpss_cos_hints_t   *hIn   = NULL, *hOut     = NULL;
  hpssclapipy_cos_priorities_t *priorities = NULL;
  hpss_cos_priorities_t   *hPri       = NULL;
  PyObject *rc;

  static char *kwlist[] = { "path", "mode", "hints", "priorities", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|iO!O!", kwlist, &path, &mode,
                                  &hpssclapipy_cos_hintsType, &hints,
                                  &hpssclapipy_cos_prioritiesType, &priorities))
    return NULL;

  if(hints != NULL)
    hIn = &hints->data;
  if(priorities != NULL)
    hPri = &priorities->data;

  hintsout = TP_ALLOC(cos_hints);
  if(hintsout == NULL)
    return PyErr_NoMemory();
  hOut = &hintsout->data;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_Creat(path, mode, hIn, hPri, hOut);
  Py_END_ALLOW_THREADS

  if(status < 0) {
    Py_DECREF(hintsout);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(iN)", status, hintsout);
  if(rc == NULL) {
    Py_DECREF(hintsout);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_Create(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int  mode = 0777;
  int  status;
  hpssclapipy_cos_hints_t *hints = NULL, *hintsout = NULL;
  hpss_cos_hints_t   *hIn   = NULL, *hOut     = NULL;
  hpssclapipy_cos_priorities_t *priorities = NULL;
  hpss_cos_priorities_t   *hPri       = NULL;

  static char *kwlist[] = { "path", "mode", "hints", "priorities", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|iO!O!", kwlist, &path, &mode,
                                  &hpssclapipy_cos_hintsType, &hints,
                                  &hpssclapipy_cos_prioritiesType, &priorities))
    return NULL;

  if(hints != NULL)
    hIn  = &hints->data;
  if(priorities != NULL)
    hPri = &priorities->data;

  hintsout = TP_ALLOC(cos_hints);
  if(hintsout == NULL)
    return PyErr_NoMemory();
  hOut = &hintsout->data;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_Create(path, mode, hIn, hPri, hOut);
  Py_END_ALLOW_THREADS

  if(status < 0) {
    Py_DECREF(hintsout);
    return PyErr_SetErrno(-status);
  }

  return (PyObject*)hintsout;
}

PyObject* hpssclapipy_CreateHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t   *objhandle = NULL;
  ns_ObjHandle_t          *ObjHandle = NULL;
  char                    *path  = NULL;
  int                     mode   = 0777;
  hpssclapipy_sec_cred_t       *ucred = NULL;
  sec_cred_t              *Ucred = NULL;
  hpssclapipy_cos_hints_t      *hints = NULL, *hintsout = NULL;
  hpss_cos_hints_t        *hIn   = NULL;
  hpssclapipy_cos_priorities_t *priorities = NULL;
  hpss_cos_priorities_t   *hPri       = NULL;
  hpssclapipy_vattr_t          *vattr;
  int status;
  PyObject *rc;

  static char *kwlist[] = { "objhandle", "path", "mode", "ucred", "hints", "priorities", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!siO!O!O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path, &mode,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &hpssclapipy_cos_hintsType, &hints,
                                  &hpssclapipy_cos_prioritiesType, &priorities))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;
  if(hints != NULL)
    hIn = &hints->data;
  if(priorities != NULL)
    hPri = &priorities->data;

  hintsout = TP_ALLOC(cos_hints);
  vattr    = TP_ALLOC(vattr);
  if(hintsout == NULL || vattr == NULL) {
    Py_XDECREF(hintsout);
    Py_XDECREF(vattr);
    return PyErr_NoMemory();
  }

  Py_BEGIN_ALLOW_THREADS
  status = hpss_CreateHandle(ObjHandle, path, mode, Ucred, hIn, hPri, &hintsout->data, &vattr->data);
  Py_END_ALLOW_THREADS

  if(status < 0) {
    Py_DECREF(hintsout);
    Py_DECREF(vattr);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(NN)", hintsout, vattr);
  if(rc == NULL) {
    Py_DECREF(hintsout);
    Py_DECREF(vattr);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_Open(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int  oflags;
  int  mode = 0777;
  int  status;
  hpssclapipy_cos_hints_t *hints = NULL, *hintsout = NULL;
  hpss_cos_hints_t   *hIn   = NULL, *hOut     = NULL;
  hpssclapipy_cos_priorities_t *priorities = NULL;
  hpss_cos_priorities_t   *hPri       = NULL;
  PyObject *rc;

  static char *kwlist[] = { "path", "oflags", "mode", "hints", "priorities", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "si|iO!O!", kwlist,
                                  &path, &oflags, &mode,
                                  &hpssclapipy_cos_hintsType, &hints,
                                  &hpssclapipy_cos_prioritiesType, &priorities))
    return NULL;

  if(hints != NULL)
    hIn  = &hints->data;
  if(priorities != NULL)
    hPri = &priorities->data;

  hintsout = TP_ALLOC(cos_hints);
  if(hintsout == NULL)
    return PyErr_NoMemory();
  hOut = &hintsout->data;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_Open(path, oflags, mode, hIn, hPri, hOut);
  Py_END_ALLOW_THREADS

  if(status < 0) {
    Py_DECREF(hintsout);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(iN)", status, hintsout);
  if(rc == NULL) {
    Py_DECREF(hintsout);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_OpenHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t   *objhandle = NULL;
  ns_ObjHandle_t          *ObjHandle = NULL;
  char                    *path  = NULL;
  int                     oflags = 0;
  int                     mode   = 0777;
  hpssclapipy_sec_cred_t       *ucred = NULL;
  sec_cred_t              *Ucred = NULL;
  hpssclapipy_cos_hints_t      *hints = NULL, *hintsout = NULL;
  hpss_cos_hints_t        *hIn   = NULL;
  hpssclapipy_cos_priorities_t *priorities = NULL;
  hpss_cos_priorities_t   *hPri       = NULL;
  hpssclapipy_vattr_t          *vattr;
  int status;
  PyObject *rc;

  static char *kwlist[] = { "objhandle", "path", "oflags", "mode", "ucred", "hints", "priorities", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!siiO!O!O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path, &oflags, &mode,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &hpssclapipy_cos_hintsType, &hints,
                                  &hpssclapipy_cos_prioritiesType, &priorities))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;
  if(hints != NULL)
    hIn = &hints->data;
  if(priorities != NULL)
    hPri = &priorities->data;

  hintsout = TP_ALLOC(cos_hints);
  vattr    = TP_ALLOC(vattr);
  if(hintsout == NULL || vattr == NULL) {
    Py_XDECREF(hintsout);
    Py_XDECREF(vattr);
    return PyErr_NoMemory();
  }

  Py_BEGIN_ALLOW_THREADS
  status = hpss_OpenHandle(ObjHandle, path, oflags, mode, Ucred, hIn, hPri, &hintsout->data, &vattr->data);
  Py_END_ALLOW_THREADS

  if(status < 0) {
    Py_DECREF(hintsout);
    Py_DECREF(vattr);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(iNN)", status, hintsout, vattr);
  if(rc == NULL) {
    Py_DECREF(hintsout);
    Py_DECREF(vattr);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_Close(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd;
  int rc;

  static char *kwlist[] = { "fd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &fd))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Close(fd);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Read(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd;
  ssize_t rc;
  unsigned long long size;
  PyObject *buf;

  static char *kwlist[] = { "fd", "size", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iK", kwlist, &fd, &size))
    return NULL;

  if(size == 0)
    return PyErr_SetErrno(EINVAL);

  // allocate a string object large enough for the buffer
  buf = PyString_FromStringAndSize(NULL, size);
  if(buf == NULL)
    return PyErr_NoMemory();

  /* You aren't supposed to modify strings except under very specific
   * circumstances; this is one of them. We just created the string
   * and we have the only reference to it. This is the only reliable
   * way to keep the current file offset consistent. We cannot afford
   * any errors to occur after hpss_Read succeeds until we return.
   */
  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Read(fd, PyString_AS_STRING(buf), (size_t)size);
  Py_END_ALLOW_THREADS

  if(rc < 0) {
    Py_DECREF(buf);
    return PyErr_SetErrno(-rc);
  }

  /* We read no bytes, so let's clean up this string buffer
   * which we allocated and return None
   */
  if(rc == 0) {
    Py_DECREF(buf);
    Py_RETURN_NONE;
  }

  /* We should *always* be resizing to smaller so this should NEVER fail.
   * If it does fail, we are in SERIOUS trouble
   */
  if(_PyString_Resize(&buf, rc)) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(buf);
    return NULL;
  }

  return buf;
}

PyObject* hpssclapipy_Write(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd;
  ssize_t rc;
  size_t bytes_to_write;

  const char *buf;

  PyObject *pyBufObj;
  Py_buffer pyBuf;

  static char *kwlist[] = { "fd", "buf", "size", NULL }; 

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iOk", kwlist, &fd, &pyBufObj, &bytes_to_write))
    return NULL;

  if (PyObject_GetBuffer(pyBufObj, &pyBuf, PyBUF_WRITABLE) < 0)
    return NULL;

  buf = pyBuf.buf;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Write(fd, buf, bytes_to_write);
  Py_END_ALLOW_THREADS

  PyBuffer_Release(&pyBuf);
  if(rc < 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("i", rc);
}

PyObject* hpssclapipy_Access(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int  mode;
  int  rc;

  static char *kwlist[] = { "path", "mode", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "si", kwlist, &path, &mode))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Access(path, mode);
  Py_END_ALLOW_THREADS

  if(rc == 0)
    Py_RETURN_TRUE;

  if(rc == HPSS_EACCES)
    Py_RETURN_FALSE;

  return PyErr_SetErrno(-rc);
}

PyObject* hpssclapipy_AccessHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path = NULL;
  int  mode  = 0, rc;
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;

  static char *kwlist[] = { "objhandle", "path", "mode", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!siO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path, &mode,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_AccessHandle(ObjHandle, path, mode, Ucred);
  Py_END_ALLOW_THREADS

  if(rc == 0)
    Py_RETURN_TRUE;

  if(rc == -EACCES)
    Py_RETURN_FALSE;

  return PyErr_SetErrno(-rc);
}

PyObject* hpssclapipy_Rename(PyObject *self, PyObject *args, PyObject *keywds) {
  char *old, *new;
  int rc;

  static char *kwlist[] = { "oldpath", "newpath", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss", kwlist, &old, &new))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Rename(old, new);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_RenameHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *oldhandle = NULL, *newhandle = NULL;
  ns_ObjHandle_t        *OldHandle = NULL, *NewHandle = NULL;
  char                  *oldpath   = NULL, *newpath   = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  int rc;

  static char *kwlist[] = { "oldhandle", "oldpath", "newhandle", "newpath", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!sO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &oldhandle,
                                  &oldpath,
                                  &hpssclapipy_ns_ObjHandleType, &newhandle,
                                  &newpath,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(oldhandle != NULL)
    OldHandle = &oldhandle->data;
  if(newhandle != NULL)
    NewHandle = &newhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_RenameHandle(OldHandle, oldpath, NewHandle, newpath, Ucred);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Undelete(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  char *flag;
  int flags = HPSS_UNDELETE_NONE;
  int rc;

  static char *kwlist[] = { "path", "flag", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|s", kwlist, &path, &flag))
     return NULL;

  if(flag != NULL)
  {
      if(strcmp(flag, "overwrite") == 0)
         flags = HPSS_UNDELETE_OVERWRITE;
      else if(strcmp(flag, "restore") == 0)
         flags = HPSS_UNDELETE_RESTORE_TIME;
      else if(strcmp(flag, "overwrite+restore") == 0)
         flags = HPSS_UNDELETE_OVERWRITE_AND_RESTORE;
  }

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Undelete(path, flags);
  Py_END_ALLOW_THREADS

  if(rc != 0)
     return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UndeleteHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t  *objhandle = NULL;
  ns_ObjHandle_t              *ObjHandle = NULL;
  char                        *path = NULL;
  char                        *flag = NULL;
  int                         flags = HPSS_UNDELETE_NONE;
  hpssclapipy_sec_cred_t      *ucred   = NULL;
  sec_cred_t                  *Ucred   = NULL;
  int                         rc;

  static char *kwlist[] = { "path", "objhandle", "flag", "ucred", NULL };
  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|O!sO!", kwlist, &path,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &flag, &hpssclapipy_sec_credType, &ucred))
     return NULL;

  if(objhandle != NULL)
     ObjHandle = &objhandle->data;
  if(ucred != NULL)
     Ucred = &ucred->data;

  if(flag != NULL)
  {
      if(strcmp(flag, "overwrite") == 0)
         flags = HPSS_UNDELETE_OVERWRITE;
      else if(strcmp(flag, "restore") == 0)
         flags = HPSS_UNDELETE_RESTORE_TIME;
      else if(strcmp(flag, "overwrite+restore") == 0)
         flags = HPSS_UNDELETE_OVERWRITE_AND_RESTORE;
  }

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UndeleteHandle(ObjHandle, path, Ucred, flags);
  Py_END_ALLOW_THREADS

  if(rc != 0)
     return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}


PyObject* hpssclapipy_Unlink(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int rc;

  static char *kwlist[] = { "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &path))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Unlink(path);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UnlinkHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *path      = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  int rc;

  static char *kwlist[] = { "objhandle", "path", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UnlinkHandle(ObjHandle, path, Ucred);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_SetFileOffset(PyObject *self, PyObject *args, PyObject *keywds) {
  int   Fildes, Whence, Direction, rc;
  unsigned long long TmpOffsetIn;
  u_signed64 OffsetIn, OffsetOut;
  static char *kwlist[] = {"fd", "offset", "whence", "direction", NULL};

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iKii", kwlist,
                                  &Fildes, &TmpOffsetIn, &Whence,
                                  &Direction))
    return NULL;

  OffsetIn = ULL_TO_U64(TmpOffsetIn);

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SetFileOffset(Fildes, OffsetIn, Whence, Direction, &OffsetOut);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("K", U64_TO_ULL(OffsetOut));
}

PyObject* hpssclapipy_CopyFile(PyObject *self, PyObject *args, PyObject *keywds) {
  int src, dst, rc;

  static char *kwlist[] = { "src", "dst", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ii", kwlist, &src, &dst))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_CopyFile(src, dst);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Fsync(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;

  static char *kwlist[] = { "fd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &fd))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fsync(fd);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Fclear(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;
  unsigned long long len;

  static char *kwlist[] = { "fd", "length", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iK", kwlist, &fd, &len))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fclear(fd, ULL_TO_U64(len));
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_FclearOffset(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;
  unsigned long long off, len;

  static char *kwlist[] = { "fd", "offset", "length", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iKK", kwlist, &fd, &off, &len))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_FclearOffset(fd, ULL_TO_U64(off), ULL_TO_U64(len));
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Fpreallocate(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;
  unsigned long long len;

  static char *kwlist[] = { "fd", "length", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iK", kwlist, &fd, &len))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fpreallocate(fd, ULL_TO_U64(len));
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Ftruncate(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;
  unsigned long long len;

  static char *kwlist[] = { "fd", "length", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iK", kwlist, &fd, &len))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Ftruncate(fd, ULL_TO_U64(len));
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Truncate(PyObject *self, PyObject *args, PyObject *keywds) {
  char               *path;
  unsigned long long len;
  int                rc;

  static char *kwlist[] = { "path", "length", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "sK", kwlist, &path, &len))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Truncate(path, ULL_TO_U64(len));
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_TruncateHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  char                  *path      = NULL;
  unsigned long long    len        = 0;
  int                   rc;

  static char *kwlist[] = { "objhandle", "path", "length", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sKO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path, &len,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_TruncateHandle(ObjHandle, path, ULL_TO_U64(len), Ucred);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}
