#include <Python.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

static inline char* getModeString(const char *mode) {
  if     (strcmp(mode, "r")  == 0) return "r";
  else if(strcmp(mode, "r+") == 0) return "r+";
  else if(strcmp(mode, "w")  == 0) return "w";
  else if(strcmp(mode, "w+") == 0) return "w+";
  else if(strcmp(mode, "a")  == 0) return "a";
  else if(strcmp(mode, "a+") == 0) return "a+";
  else                             return NULL;
}

/* Deallocator */
static void hpssclapipy_FILE_dealloc(hpssclapipy_FILE_t *self) {
  if(self->data != NULL)
    hpss_Fclose(self->data);
  self->ob_type->tp_free((PyObject*)self);
}

/* Type definition */
PyTypeObject hpssclapipy_FILEType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.FILE",
  .tp_basicsize = sizeof(hpssclapipy_FILE_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS File Stream",
  .tp_dealloc   = (destructor)hpssclapipy_FILE_dealloc,
};

PyObject* hpssclapipy_Fclose(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  int rc;

  static char *kwlist[] = { "stream", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fclose(fp->data);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  fp->data = NULL;

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Fflush(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  int           rc;

  static char *kwlist[] = { "stream", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fflush(fp->data);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Fgetc(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  int           rc;
  char          ch;

  static char *kwlist[] = { "stream", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fgetc(fp->data);
  Py_END_ALLOW_THREADS

  if(rc == EOF)
    Py_RETURN_NONE;

  ch = rc;
  return PyString_FromStringAndSize(&ch, 1);
}

PyObject* hpssclapipy_Fgets(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  int           size;
  PyObject      *buf;
  char          *rc;

  static char *kwlist[] = { "size", "stream", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "KO!", kwlist, &size,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  buf = PyString_FromStringAndSize(NULL, size);
  if(buf == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fgets(PyString_AS_STRING(buf), size, fp->data);
  Py_END_ALLOW_THREADS

  if(rc == NULL) {
    Py_DECREF(buf);
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  if(_PyString_Resize(&buf, strlen(PyString_AS_STRING(buf)))) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(buf);
    return NULL;
  }

  return buf;
}

PyObject* hpssclapipy_Fopen(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  char *path, *mode;

  static char *kwlist[] = { "path", "mode", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss", kwlist,
                                  &path, &mode))
    return NULL;

  if((mode = getModeString(mode)) == NULL)
    return PyErr_SetErrno(EINVAL);

  fp = TP_ALLOC(FILE);
  if(fp == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  fp->data = hpss_Fopen(path, mode);
  Py_END_ALLOW_THREADS

  if(fp->data == NULL) {
    Py_DECREF(fp);
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  return (PyObject*)fp;
}

PyObject* hpssclapipy_Fread(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  size_t rc;
  unsigned long long size;
  PyObject *buf;

  static char *kwlist[] = { "size", "stream", NULL, };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "KO!", kwlist,
                                  &size,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  if(size == 0)
    return PyErr_SetErrno(EINVAL);

  buf = PyString_FromStringAndSize(NULL, size);
  if(buf == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  errno = 0;
  rc = hpss_Fread(PyString_AS_STRING(buf), 1, (size_t)size, fp->data);
  Py_END_ALLOW_THREADS

  if(errno != 0) {
    Py_DECREF(buf);
    return PyErr_SetFromErrno(PyExc_IOError);
  }

  if(rc == 0) {
    Py_DECREF(buf);
    Py_RETURN_NONE;
  }

  if(_PyString_Resize(&buf, rc)) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(buf);
    return NULL;
  }

  return buf;
}

PyObject* hpssclapipy_Fseek(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  ssize_t offset;
  int whence, rc;

  static char *kwlist[] = { "stream", "offset", "whence", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!ni", kwlist,
                                  &hpssclapipy_FILEType, &fp,
                                  &offset, &whence))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fseek(fp->data, offset, whence);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Ftell(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  long rc;

  static char *kwlist[] = { "stream", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Ftell(fp->data);
  Py_END_ALLOW_THREADS

  if(rc < 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("l", rc);
}

PyObject* hpssclapipy_Fwrite(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_FILE_t *fp;
  char *buf;
  size_t size, rc;

  static char *kwlist[] = { "buf", "stream", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s#O!", kwlist,
                                  &buf, &size,
                                  &hpssclapipy_FILEType, &fp))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  errno = 0;
  rc = hpss_Fwrite(buf, 1, size, fp->data);
  Py_END_ALLOW_THREADS

  if(errno != 0)
    return PyErr_SetFromErrno(PyExc_IOError);

  return Py_BuildValue("n", rc);
}
