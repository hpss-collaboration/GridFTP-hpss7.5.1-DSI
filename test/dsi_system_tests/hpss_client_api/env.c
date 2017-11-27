// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/env.c $ $Id: env.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <hpss_Getenv.h>
#include "hpssclapipy.h"

/* Deallocators */
static void hpssclapipy_errno_state_dealloc  (hpssclapipy_errno_state_t     *self);

/* Getters */
static PyObject* hpssclapipy_errno_state_get (hpssclapipy_errno_state_t *self, void *closure);

/* Representation */
static PyObject* hpssclapipy_errno_state_repr(hpssclapipy_errno_state_t *self);

typedef enum{
  FIELD_hpss_errno,
  FIELD_func,
  FIELD_requestId,
} errno_state_field_t;

static PyGetSetDef hpssclapipy_errno_state_getset[] = {
#define ERRNO_STATE(x, doc) \
  { #x, (getter)hpssclapipy_errno_state_get, (setter)NULL, doc, (void*)FIELD_##x, }
  ERRNO_STATE(hpss_errno,    "HPSS error number"),
  ERRNO_STATE(func,          "Name of function associated with error state"),
  ERRNO_STATE(requestId,     "Request Id associated with error state"),
  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definitions */
PyTypeObject hpssclapipy_errno_stateType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.errno_state",
  .tp_basicsize = sizeof(hpssclapipy_errno_state_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Error State",
  .tp_dealloc   = (destructor)hpssclapipy_errno_state_dealloc,
  .tp_getset    = hpssclapipy_errno_state_getset,
  .tp_repr      = (reprfunc)hpssclapipy_errno_state_repr,
};

/* Deallocator */
static void hpssclapipy_errno_state_dealloc(hpssclapipy_errno_state_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

/* Getters and Setters */
static PyObject* hpssclapipy_errno_state_get(hpssclapipy_errno_state_t *self, void *closure) {
  errno_state_field_t field = (errno_state_field_t)closure;

  switch(field) {
    GET_INT(hpss_errno);
    GET_STR(func);
    GET_U32(requestId);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

/* Representation */
static PyObject* hpssclapipy_errno_state_repr(hpssclapipy_errno_state_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_errno_state_t {\n");
  indent += 2;

  PRINT_U32(hpss_errno);
  PRINT_STR(func);
  PRINT_U32(requestId);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

PyObject* hpssclapipy_Getenv(PyObject *self, PyObject *args) {
  char *env, *rc;

  if(!PyArg_ParseTuple(args, "s", &env))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Getenv(env);
  Py_END_ALLOW_THREADS

  if(rc == NULL)
    Py_RETURN_NONE;

  return Py_BuildValue("s", rc);
}

PyObject* hpssclapipy_GetLastHPSSErrno(PyObject *self) {
  hpssclapipy_errno_state_t *rc;

  rc = TP_ALLOC(errno_state);
  if(rc == NULL) 
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc->data = hpss_GetLastHPSSErrno();
  Py_END_ALLOW_THREADS

  return (PyObject *) rc;
}
