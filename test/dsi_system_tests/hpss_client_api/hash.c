// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/hash.c $ $Id: hash.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <structmember.h>
#include "hpssclapipy.h"

#if defined(HPSS743P2A) || defined(HPSS743P2B) || defined(HPSS743P3) || \
defined(HPSS751)

#if defined(HPSS743P2B) || defined(HPSS743P3) || defined(HPSS751)
#define HASH_VARIANT_API
#endif

#ifdef HASH_VARIANT_API
#define HASH_INT_TYPE hpss_hash_type_t
#define HASH_ENUM_TYPE hpss_hash_type_t
#else
#define HASH_INT_TYPE int
#define HASH_ENUM_TYPE HashType
#endif

/* Allocators */
static PyObject* hpssclapipy_hash_new(PyTypeObject *type, PyObject *args, PyObject *keywds);

/* Initializers */
static int hpssclapipy_hash_init(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds);

/* Deallocators */
static void hpssclapipy_hash_dealloc(hpssclapipy_hash_t *self);

/* Representation */
static PyObject* hpssclapipy_hash_repr(hpssclapipy_hash_t *self);

/* Methods */
static PyObject* hpssclapipy_hash_reset    (hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds);
static PyObject* hpssclapipy_hash_append   (hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds);
static PyObject* hpssclapipy_hash_duplicate(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds);
static PyObject* hpssclapipy_hash_extract  (hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds);

static PyMethodDef hpssclapipy_hash_methods[] = {
#define HASH_METHOD(x, m, doc) \
  { #x, (PyCFunction)hpssclapipy_hash_##x, METH_##m, doc, }
  HASH_METHOD(reset,     NOARGS,   "Reset the hash"),
  HASH_METHOD(append,    KEYWORDS, "Append data to the hash"),
  HASH_METHOD(duplicate, NOARGS,   "Create a deep copy"),
  HASH_METHOD(extract,   NOARGS,   "Extract hash to a buffer and type, which "
                                   "can later be loaded into the hpss.hash "
                                   "constructor"),

  { NULL, NULL, 0, NULL, },
};

/* Type definitions */
PyTypeObject hpssclapipy_hashType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.hash",
  .tp_basicsize = sizeof(hpssclapipy_hash_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Hash",
  .tp_new       = hpssclapipy_hash_new,
  .tp_init      = (initproc)hpssclapipy_hash_init,
  .tp_dealloc   = (destructor)hpssclapipy_hash_dealloc,
  .tp_repr      = (reprfunc)hpssclapipy_hash_repr,
  .tp_methods   = hpssclapipy_hash_methods,
};

/* Allocators */
static PyObject* hpssclapipy_hash_new(PyTypeObject *type, PyObject *args, PyObject *keywds) {
  return type->tp_alloc(type, 0);
}

static PyObject* hpssclapipy_hash_type_new(PyTypeObject *type, PyObject *args, PyObject *keywds) {
  return type->tp_alloc(type, 0);
}

/* Initializers */
static int hpssclapipy_hash_init(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds) {
  HASH_INT_TYPE   type;
  char       *buffer = NULL;
  Py_ssize_t size    = 0;
  unsigned int        len;

  static char *kwlist[] = { "type", "buffer", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i|s#", kwlist, &type, &buffer, &size))
    return -1;

  if(self == NULL) {
    PyErr_SetString(PyExc_AssertionError, "self is NULL");
    return -1;
  }

  if(self->data != NULL)
    hpss_HashDelete(self->data);

  len = size;

  Py_BEGIN_ALLOW_THREADS
  if(buffer != NULL)
#ifdef HASH_VARIANT_API
    self->data = hpss_HashLoad(buffer, len, (HASH_ENUM_TYPE)type);
#else
    self->data = hpss_HashLoad(buffer, (int *)&len, (HASH_ENUM_TYPE)type);
#endif
  else
    self->data = hpss_HashCreate(type);
  Py_END_ALLOW_THREADS

  if(self->data == NULL) {
    PyErr_SetString(PyExc_IOError, "Failed to create hash");
    return -1;
  }

  return 0;
}

/* Deallocators */
static void hpssclapipy_hash_dealloc(hpssclapipy_hash_t *self) {
  if(self->data != NULL) {
    Py_BEGIN_ALLOW_THREADS
    hpss_HashDelete(self->data);
    Py_END_ALLOW_THREADS
    self->data = NULL;
  }
  self->ob_type->tp_free((PyObject*)self);
}

/* Representation */
static PyObject* hpssclapipy_hash_repr(hpssclapipy_hash_t *self) {
  PyObject      *rc;
  hpss_hash_t    hash;
  char          *hex;

  hash = hpss_HashDuplicate(self->data);
  /* TODO: handle hash duplicate error */
  /*
  if(hash == NULL)
    return PyErr_NoMemory();
  */
  hex = hpss_HashFinishHex(hash);
  hpss_HashDelete(hash);
  /* TODO: Look into handling hpss_HashDelete error code */
  if(hex == NULL)
    return PyErr_NoMemory();

  rc = PyString_FromString(hex);
  free(hex);
  if(rc == NULL)
    return PyErr_NoMemory();

  return rc;
}

/* Methods */
static PyObject* hpssclapipy_hash_reset(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds) {
  Py_BEGIN_ALLOW_THREADS
  hpss_HashReset(self->data);
  Py_END_ALLOW_THREADS
  Py_RETURN_NONE;
}

static PyObject* hpssclapipy_hash_append(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds) {
  unsigned char *buf;
  Py_ssize_t size;
  int rc;

  static char *kwlist[] = { "buffer", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwlist, &buf, &size))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_HashAppendBuf(self->data, buf, (unsigned int)size);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

static PyObject* hpssclapipy_hash_duplicate(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *copy;

  copy = TP_ALLOC(hash);
  if(copy == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  copy->data = hpss_HashDuplicate(self->data);
  Py_END_ALLOW_THREADS
  if(copy->data == NULL) {
    Py_DECREF(copy);
    return PyErr_NoMemory();
  }

  return (PyObject*)copy;
}

static PyObject* hpssclapipy_hash_extract(hpssclapipy_hash_t *self, PyObject *args, PyObject *keywds) {
  unsigned int  len;
  int status;
  HASH_ENUM_TYPE type;
  PyObject *buf, *rc;

  buf = PyString_FromStringAndSize(NULL, 1024);
  if(buf == NULL)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
#ifdef HASH_VARIANT_API
  status = hpss_HashExtract(self->data, PyString_AS_STRING(buf), &len, &type);
#else
  status = hpss_HashExtract(self->data, PyString_AS_STRING(buf), (int*)&len,
                            &type);
#endif
  Py_END_ALLOW_THREADS

  if(status != 0) {
    Py_DECREF(buf);
    return PyErr_SetErrno(-status);
  }

  if(_PyString_Resize(&buf, len)) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(buf);
    return NULL;
  }

  rc = Py_BuildValue("(Ni)", buf, type);
  if(rc == NULL) {
    Py_DECREF(buf);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_HashAppend(PyObject *self, PyObject *args, PyObject *keywds) {
  return hpssclapipy_HashAppendBuf(self, args, keywds);
}

PyObject* hpssclapipy_HashAppendBuf(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;
  unsigned char *buf;
  Py_ssize_t    len;
  int           rc;

  static char *kwlist[] = { "hash", "buffer", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!s#", kwlist,
                                  &hpssclapipy_hashType, &h, &buf, &len))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_HashAppendBuf(h->data, buf, (unsigned int)len);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_HashAppendStr(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;
  char          *str;
  int           rc;

  static char *kwlist[] = { "hash", "str", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!s", kwlist,
                                  &hpssclapipy_hashType, &h, &str))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_HashAppendStr(h->data, str);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

static PyObject* hpssclapipy_hash_create(HASH_INT_TYPE type) {
  hpssclapipy_hash_t *h;

  h = TP_ALLOC(hash);
  if(h == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  h->data = hpss_HashCreate(type);
  Py_END_ALLOW_THREADS

  if(h->data == NULL) {
    Py_DECREF(h);
    PyErr_SetString(PyExc_IOError, "Failed to create hash");
    return NULL;
  }

  return (PyObject *)h;
}

PyObject* hpssclapipy_HashCreate(PyObject *self, PyObject *args, PyObject *keywds) {
  HASH_INT_TYPE type;

  static char *kwlist[] = { "type", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &type))
    return NULL;

  return hpssclapipy_hash_create(type);
}

PyObject* hpssclapipy_HashCreateAdler32(PyObject *self) {
  return hpssclapipy_hash_create(HASH_ADLER32);
}

PyObject* hpssclapipy_HashCreateCrc32(PyObject *self) {
  return hpssclapipy_hash_create(HASH_CRC32);
}

PyObject* hpssclapipy_HashCreateMD5(PyObject *self) {
  return hpssclapipy_hash_create(HASH_MD5);
}

PyObject* hpssclapipy_HashCreateSHA1(PyObject *self) {
  return hpssclapipy_hash_create(HASH_SHA1);
}

PyObject* hpssclapipy_HashCreateSHA224(PyObject *self) {
  return hpssclapipy_hash_create(HASH_SHA224);
}

PyObject* hpssclapipy_HashCreateSHA256(PyObject *self) {
  return hpssclapipy_hash_create(HASH_SHA256);
}

PyObject* hpssclapipy_HashCreateSHA384(PyObject *self) {
  return hpssclapipy_hash_create(HASH_SHA384);
}

PyObject* hpssclapipy_HashCreateSHA512(PyObject *self) {
  return hpssclapipy_hash_create(HASH_SHA512);
}

PyObject* hpssclapipy_HashDuplicate(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;

  static char *kwlist[] = { "hash", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_hashType, &h))
    return NULL;

  return hpssclapipy_hash_duplicate(h, NULL, NULL);
}

PyObject* hpssclapipy_HashExtract(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;

  static char *kwlist[] = { "hash", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_hashType, &h))
    return NULL;

  return hpssclapipy_hash_extract(h, NULL, NULL);
}

PyObject* hpssclapipy_HashFinish(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;
  unsigned char *hash;
  unsigned int  len;
  PyObject      *rc;

  static char *kwlist[] = { "hash", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_hashType, &h))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  hash = hpss_HashFinish(h->data, &len);
  Py_END_ALLOW_THREADS

  if(hash == NULL) {
    PyErr_SetString(PyExc_IOError, "Failed to finalize hash");
    return NULL;
  }

  rc = PyString_FromStringAndSize((char*)hash, len);
  free(hash);
  if(rc == NULL)
    return PyErr_NoMemory();

  return rc;
}

PyObject* hpssclapipy_HashFinishHex(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;
  char          *hash;
  PyObject      *rc;

  static char *kwlist[] = { "hash", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_hashType, &h))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  hash = hpss_HashFinishHex(h->data);
  Py_END_ALLOW_THREADS

  if(hash == NULL) {
    PyErr_SetString(PyExc_IOError, "Failed to finalize hash");
    return NULL;
  }

  rc = PyString_FromString(hash);
  free(hash);
  if(rc == NULL)
    return PyErr_NoMemory();

  return rc;
}

PyObject* hpssclapipy_HashLoad(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;
  HASH_ENUM_TYPE      type;
  char          *buffer = NULL;
  Py_ssize_t    size    = 0;
  unsigned int           len;

  static char *kwlist[] = { "buffer", "type", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s#i", kwlist, &buffer, &size, &type))
    return NULL;

  h = TP_ALLOC(hash);
  if(h == NULL)
    return PyErr_NoMemory();

  len = size;

  Py_BEGIN_ALLOW_THREADS
#ifdef HASH_VARIANT_API
  h->data = hpss_HashLoad(buffer, len, type);
#else
  h->data = hpss_HashLoad(buffer, (int *)&len, type);
#endif
  Py_END_ALLOW_THREADS

  if(h->data == NULL) {
    Py_DECREF(h);
    PyErr_SetString(PyExc_IOError, "Failed to load hash");
    return NULL;
  }

  return (PyObject*)h;
}

PyObject* hpssclapipy_HashReset(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_hash_t *h;
  int           rc;

  static char *kwlist[] = { "hash", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_hashType, &h))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_HashReset(h->data);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}
#endif
