#include <Python.h>
#include <hpss_String.h>
#include "hpssclapipy.h"

PyObject* hpssclapipy_BuildLevelString(PyObject *self) {
  PyObject *rc;
  char *str;

  Py_BEGIN_ALLOW_THREADS
  str = hpss_BuildLevelString();
  Py_END_ALLOW_THREADS

  if(str == NULL)
    return PyErr_NoMemory();

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

PyObject* hpssclapipy_CopyrightStrings(PyObject *self) {
  PyObject *rc;
  char **str, **p;

  Py_BEGIN_ALLOW_THREADS
  p = str = hpss_CopyrightStrings();
  Py_END_ALLOW_THREADS

  if(str == NULL)
    return PyErr_NoMemory();

  rc = PyList_New(0);
  if(rc == NULL) {
    free(str);
    return PyErr_NoMemory();
  }

  while(*p != NULL) {
    PyObject *item = Py_BuildValue("s", *p);
    if(item == NULL || PyList_Append(rc, item) != 0) {
      free(str);
      Py_DECREF(rc);
      return NULL;
    }
    Py_DECREF(item);
    p++;
  }

  free(str);
  return rc;
}

PyObject* hpssclapipy_AuthnMechTypeFromString(PyObject *self, PyObject *args) {
  char *str;
  unsigned32 rc;
  hpss_authn_mech_t mech_type;

  if(!PyArg_ParseTuple(args, "s", &str))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_AuthnMechTypeFromString(str, &mech_type);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("i", mech_type);
}
