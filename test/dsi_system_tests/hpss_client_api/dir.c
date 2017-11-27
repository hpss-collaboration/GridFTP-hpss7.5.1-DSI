#include <Python.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

PyObject* hpssclapipy_Opendir(PyObject *self, PyObject *args, PyObject *keywds) {
  char *dir;
  int  rc;

  static char *kwlist[] = { "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &dir))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Opendir(dir);
  Py_END_ALLOW_THREADS

  if(rc < 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("i", rc);
}

PyObject* hpssclapipy_Readdir(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;
  hpssclapipy_dirent_t *dent;

  static char *kwlist[] = { "fd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &fd))
    return NULL;

  dent = TP_ALLOC(dirent);
  if(dent == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Readdir(fd, &dent->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(dent);
    return PyErr_SetErrno(-rc);
  }

  if(dent->data.d_namelen == 0) {
    Py_DECREF(dent);
    Py_RETURN_NONE;
  }

  return (PyObject*)dent;
}

PyObject* hpssclapipy_ReaddirHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  unsigned long long    offin;
  u_signed64            offout;
  hpssclapipy_ns_ObjHandle_t *objhandle;
  hpssclapipy_sec_cred_t     *ucred = NULL;
  hpssclapipy_dirent_t       *dirent;
  hpss_dirent_t         *dirents;
  sec_cred_t            *Ucred = NULL;
  unsigned32            end = 0;
  PyObject *rc, *list;
  Py_ssize_t maxentries = 1;
  int status, i;

  static char *kwlist[] = { "objhandle", "offset", "ucred", "maxentries", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!K|O!n", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &offin,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &maxentries))
    return NULL;

  if(ucred != NULL)
    Ucred = &ucred->data;

  if(maxentries < 1)
    return PyErr_SetErrno(-EINVAL);

  dirents = (hpss_dirent_t*)malloc(sizeof(*dirents)*maxentries);
  if(dirents == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  status = hpss_ReaddirHandle(&objhandle->data,
                              ULL_TO_U64(offin),
                              Ucred,
                              sizeof(*dirents)*maxentries,
                              &end,
                              &offout,
                              dirents);
  Py_END_ALLOW_THREADS

  if(status < 0) {
    free(dirents);
    return PyErr_SetErrno(-status);
  }

  list = PyList_New(status);
  if(list == NULL) {
    free(dirents);
    return PyErr_NoMemory();
  }

  for(i = 0; i < status; i++) {
    dirent = TP_ALLOC(dirent);
    if(dirent == NULL) {
      free(dirents);
      Py_DECREF(list);
      return PyErr_NoMemory();
    }
    memcpy(&dirent->data, &dirents[i], sizeof(*dirents));
    PyList_SET_ITEM(list, i, (PyObject*)dirent);
  }
  free(dirents);

  rc = Py_BuildValue("(NKi)", list, U64_TO_ULL(offout), end);
  if(rc == NULL) {
    Py_DECREF(list);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_Closedir(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd;
  int rc;

  static char *kwlist[] = { "fd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &fd))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Closedir(fd);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Getcwd(PyObject *self) {
  int rc;
  PyObject *dir = PyString_FromStringAndSize(NULL, HPSS_MAX_PATH_NAME+1);

  if(dir == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Getcwd(PyString_AS_STRING(dir), HPSS_MAX_PATH_NAME+1);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(dir);
    return PyErr_SetErrno(-rc);
  }

  if(_PyString_Resize(&dir, strlen(PyString_AS_STRING(dir)))) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_DECREF(dir);
    return NULL;
  }

  return dir;
}

PyObject* hpssclapipy_Chdir(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int rc;

  static char *kwlist[] = { "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &path))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Chdir(path);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Mkdir(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int  mode, rc;

  static char *kwlist[] = { "path", "mode", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "si", kwlist, &path, &mode))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Mkdir(path, mode);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_MkdirHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle, *outhandle;
  char *path;
  mode_t mode;
  hpssclapipy_sec_cred_t *ucred = NULL;
  sec_cred_t        *Ucred = NULL;
  hpssclapipy_vattr_t    *vattr;
  int status;
  PyObject *rc;

  static char *kwlist[] = { "objhandle", "path", "mode", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!si|O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path, &mode,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(ucred != NULL)
    Ucred = &ucred->data;

  outhandle = TP_ALLOC(ns_ObjHandle);
  vattr     = TP_ALLOC(vattr);

  if(outhandle == NULL || vattr == NULL) {
    Py_XDECREF(outhandle);
    Py_XDECREF(vattr);
    return PyErr_NoMemory();
  }

  Py_BEGIN_ALLOW_THREADS
  status = hpss_MkdirHandle(&objhandle->data, path, mode, Ucred, &outhandle->data, &vattr->data);
  Py_END_ALLOW_THREADS

  if(status != 0) {
    Py_DECREF(outhandle);
    Py_DECREF(vattr);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(NN)", outhandle, vattr);
  if(rc == NULL) {
    Py_DECREF(outhandle);
    Py_DECREF(vattr);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_Rmdir(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int rc;

  static char *kwlist[] = { "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &path))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Rmdir(path);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_RmdirHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle;
  char                  *path  = NULL;
  hpssclapipy_sec_cred_t     *ucred = NULL;
  sec_cred_t            *Ucred = NULL;
  int rc;

  static char *kwlist[] = { "objhandle", "path", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!|sO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_RmdirHandle(&objhandle->data, path, Ucred);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Chroot(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path;
  int  rc;

  static char *kwlist[] = { "path", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &path))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Chroot(path);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Rewinddir(PyObject *self, PyObject *args, PyObject *keywds) {
  int fd, rc;

  static char *kwlist[] = { "fd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &fd))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Rewinddir(fd);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}
