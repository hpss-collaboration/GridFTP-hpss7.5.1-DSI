// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/link.c $ $Id: link.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

PyObject* hpssclapipy_Link(PyObject *self, PyObject *args, PyObject *keywds) {
  char *oldpath, *newpath;
  int rc;

  static char *kwlist[] = { "oldpath", "newpath", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss", kwlist, &oldpath, &newpath))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Link(oldpath, newpath);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_LinkHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL, *dirhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL, *DirHandle = NULL;
  char                  *path      = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  int rc;

  static char *kwlist[] = { "objhandle", "dirhandle", "path", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!O!s|O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &hpssclapipy_ns_ObjHandleType, &dirhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  ObjHandle = &objhandle->data;
  DirHandle = &dirhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_LinkHandle(ObjHandle, DirHandle, path, Ucred);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Readlink(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int rc;
  PyObject *link;

  static char *kwlist[] = { "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &path))
    return NULL;

  link = PyString_FromStringAndSize(NULL, HPSS_MAX_PATH_NAME);
  if(link == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Readlink(path, PyString_AS_STRING(link), HPSS_MAX_PATH_NAME);
  Py_END_ALLOW_THREADS

  if(rc < 0) {
    Py_DECREF(link);
    return PyErr_SetErrno(-rc);
  }

  if(_PyString_Resize(&link, rc)) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(link);
    return NULL;
  }

  return link;
}

PyObject* hpssclapipy_ReadlinkHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *handle;
  hpssclapipy_sec_cred_t     *creds = NULL;
  sec_cred_t            *ucred = NULL;
  char                  *path  = NULL;
  PyObject *link;
  int rc;

  static char *kwlist[] = { "objhandle", "path", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!|sO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &handle,
                                  &path,
                                  &hpssclapipy_sec_credType, &creds))
    return NULL;

  link = PyString_FromStringAndSize(NULL, HPSS_MAX_PATH_NAME);
  if(link == NULL)
    return PyErr_NoMemory();

  if(creds != NULL)
    ucred = &creds->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_ReadlinkHandle(&handle->data, path,
                           PyString_AS_STRING(link), HPSS_MAX_PATH_NAME,
                           ucred);
  Py_END_ALLOW_THREADS

  if(rc < 0) {
    Py_DECREF(link);
    return PyErr_SetErrno(-rc);
  }

  if(_PyString_Resize(&link, rc)) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(link);
    return NULL;
  }

  return link;
}

PyObject* hpssclapipy_Symlink(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path, *contents;
  int  rc;

  static char *kwlist[] = { "contents", "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss", kwlist, &contents, &path))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Symlink(contents, path);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_SymlinkHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *contents  = NULL, *path = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  hpssclapipy_vattr_t        *vattr;
  int rc;

  static char *kwlist[] = { "objhandle", "contents", "path", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!ssO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &contents, &path,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  vattr = TP_ALLOC(vattr);
  if(vattr == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SymlinkHandle(ObjHandle, contents, path, Ucred, &vattr->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(vattr);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)vattr;
}
