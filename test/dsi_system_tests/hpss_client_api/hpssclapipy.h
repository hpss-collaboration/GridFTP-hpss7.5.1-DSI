#pragma once
#include <Python.h>
#include <hpss_api.h>
#include <hpss_hash.h>

#define PYTYPEHPSSDEF(x) \
typedef struct { \
  PyObject_HEAD \
  hpss_##x##_t data; \
} hpssclapipy_##x##_t; \
extern PyTypeObject hpssclapipy_##x##Type
#define PYTYPEDEF(x) \
typedef struct { \
  PyObject_HEAD \
  x##_t data; \
} hpssclapipy_##x##_t; \
extern PyTypeObject hpssclapipy_##x##Type


/* for repr indentation */
extern int indent;


/* Python Type Definitions */
typedef struct {
  PyObject_HEAD
  HPSS_FILE *data;
} hpssclapipy_FILE_t;
extern PyTypeObject hpssclapipy_FILEType;

PYTYPEDEF(api_config);
PYTYPEDEF(ns_ObjHandle);
PYTYPEDEF(sec_cred);
PYTYPEDEF(bf_sc_attrib);
PYTYPEHPSSDEF(Attrs);
PYTYPEHPSSDEF(cos_hints);
PYTYPEHPSSDEF(cos_priorities);
PYTYPEHPSSDEF(dirent);
PYTYPEHPSSDEF(errno_state);
PYTYPEHPSSDEF(fileattr);
PYTYPEHPSSDEF(xfileattr);
PYTYPEHPSSDEF(hash);
PYTYPEHPSSDEF(pio_gapinfo);
PYTYPEHPSSDEF(pio_grp);
PYTYPEHPSSDEF(pio_params);
PYTYPEHPSSDEF(sockaddr);
PYTYPEHPSSDEF(stat);
PYTYPEHPSSDEF(statfs);
PYTYPEHPSSDEF(statvfs);
PYTYPEHPSSDEF(uuid);
PYTYPEHPSSDEF(vattr);

/* Function prototypes */
#define PYFUNCNOARGS(x)   PyObject* hpssclapipy_##x(PyObject *self)
#define PYFUNCVARARGS(x)  PyObject* hpssclapipy_##x(PyObject *self, PyObject *args)
#define PYFUNCKEYWORDS(x) PyObject* hpssclapipy_##x(PyObject *self, PyObject *args, PyObject *keywds)
#define METHOD(x, m)      PYFUNC##m(x);
#include "methods.h"
#undef METHOD

/* Macros */
#define trace(fmt, ...) fprintf(stderr, "%s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__ , ##__VA_ARGS__)
#define TP_ALLOC(x)   (hpssclapipy_##x##_t*)hpssclapipy_##x##Type.tp_alloc(&hpssclapipy_##x##Type,0)
#define U64_TO_ULL(v) (((unsigned long long)high32m(v)<<32) | low32m(v))
#define ULL_TO_U64(v) (bld64m(((v)>>32) & 0xFFFFFFFF, (v) & 0xFFFFFFFF))
#define GET_INT(x)      case FIELD_##x: return Py_BuildValue("i", self->data.x)
#define GET_U32(x)      case FIELD_##x: return Py_BuildValue("I", self->data.x)
#define GET_U64(x)      case FIELD_##x: return Py_BuildValue("K", U64_TO_ULL(self->data.x))
#define GET_STR(x)      case FIELD_##x: return Py_BuildValue("s", self->data.x)
#define GET_OBJ(x,t)    case FIELD_##x: { \
                                          hpssclapipy_##t##_t *obj = TP_ALLOC(t); \
                                          if(obj == NULL) \
                                            return PyErr_NoMemory(); \
                                          memcpy(&obj->data, &self->data.x, sizeof(obj->data)); \
                                          return (PyObject*)obj; \
                                        }

/* Return a copy of compile-time defined array */
#define GET_OBJ_LIST(x,t,n) case FIELD_##x: { \
                                    PyObject* py_list = PyTuple_New(n) \
                                    if (py_list == NULL) \
                                        return PyErr_NoMemory(); \
                                     unsigned int i; \
                                     for (i = 0; i < n; i++) { \
                                        hpssclapipy_##t##_t *obj = TP_ALLOC(t) \
                                        memcpy(obj->data, &self->data.x[i], sizeof(obj->data)) \
                                        PyTuple_SET_ITEM(py_list, i, Py_BuildValue("O", (PyObject)*obj) \
                                     return py_list; \
                                    }
#define SET_INT(x)      case FIELD_##x: return setInt(value, (int*)&self->data.x)
#define SET_U64(x)      case FIELD_##x: return setU64(value, &self->data.x)
#define SET_STR(x)      case FIELD_##x: return setStr(value,  (char*)self->data.x)
#define SET_OBJ(x,t)    case FIELD_##x: return setObj(value, &self->data.x, &hpssclapipy_##t##Type)
#define INDENT          sprintf(str+strlen(str), "%*s", indent, "")
#define PRINT_ENUM(x,e) case e: INDENT; strcat(str, #x ": " #e ",\n"); break
#define PRINT_INT(x)    INDENT; sprintf(str+strlen(str), "%s: %d,\n",   #x, self->data.x);
#define PRINT_U32(x)    INDENT; sprintf(str+strlen(str), "%s: %u,\n",   #x, self->data.x);
#define PRINT_U64(x)    INDENT; sprintf(str+strlen(str), "%s: %llu,\n", #x, U64_TO_ULL(self->data.x));
#define PRINT_STR(x)    INDENT; sprintf(str+strlen(str), "%s: \"%s\",\n", #x, self->data.x);
#define PRINT_OBJ(x) \
{ \
  PyObject *objstr = printObj((PyObject*)self, (void*)FIELD_##x); \
  if(objstr == NULL) { \
    indent -= 2; \
    free(str); \
    return NULL; \
  } \
  INDENT; sprintf(str+strlen(str), "%s: %s,\n", #x, PyString_AS_STRING(objstr)); \
  Py_DECREF(objstr); \
}
#define PRINT_FLAG(f, x) \
    if(self->data.f & x) { \
      strcat(str, "|" #x + first); \
      first = 0; \
    }
#define PRINT_FLAGU64(f, x) \
    if(chkbit64m(self->data.f, x)) { \
      strcat(str, "|" #x + first); \
      first = 0; \
    }
#define PRINT_TIME(x) \
  INDENT; \
  { \
    time_t    t = self->data.x; \
    char      tbuf[128]; \
    struct tm ts; \
    if(localtime_r(&t, &ts) == NULL) \
      sprintf(tbuf, "%d", self->data.x); \
    else \
      strftime(tbuf, sizeof(tbuf), "%a %Y-%m-%d %H:%M:%S %Z", &ts); \
    sprintf(str+strlen(str), "%s: %s,\n", #x, tbuf); \
  }

/* Helper Functions */
static inline int setInt(PyObject *in, int *out) {
  if(!PyInt_Check(in)) {
    PyErr_SetString(PyExc_TypeError, "This attribute must be an int");
    return -1;
  }

  *out = PyInt_AS_LONG(in);
  return 0;
}

static inline int setU64(PyObject *in, u_signed64 *out) {
  if(!PyInt_Check(in)) {
    PyErr_SetString(PyExc_TypeError, "This attribute must be an int");
    return -1;
  }

  *out = ULL_TO_U64(PyInt_AsUnsignedLongLongMask(in));
  return 0;
}

static inline int setStr(PyObject *in, char *out) {
  if(!PyString_CheckExact(in)) {
    PyErr_SetString(PyExc_TypeError, "This attribute must be a string");
    return -1;
  }

  strcpy(out, PyString_AS_STRING(in));
  return 0;
}

static inline int setObj(PyObject *in, void *out, PyTypeObject *type) {
  if(in->ob_type != type) {
    PyErr_SetString(PyExc_TypeError, "This attribute is the wrong type");
    return -1;
  }

  /* man this is hackish */
  memcpy(out, &in[1], type->tp_basicsize);
  return 0;
}

static inline PyObject* printObj(PyObject *self, void *field) {
  PyObject *obj, *objstr;
  obj = self->ob_type->tp_getset->get(self, field);
  if(obj == NULL)
    return NULL;

  objstr = obj->ob_type->tp_repr(obj);
  Py_DECREF(obj);
  return objstr;
}

static inline PyObject* PyErr_SetErrno(int err) {
  errno = err;
  return PyErr_SetFromErrno(PyExc_IOError);
}
