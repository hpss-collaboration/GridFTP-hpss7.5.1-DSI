// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/uuid.c $ $Id: uuid.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <hpss_api.h>
#include <hpss_uuid.h>
#include "hpssclapipy.h"

/* Deallocator */
static void      hpssclapipy_uuid_dealloc(hpssclapipy_uuid_t *self);

/* Representation */
static PyObject* hpssclapipy_uuid_repr(hpssclapipy_uuid_t *self);

PyTypeObject hpssclapipy_uuidType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.uuid",
  .tp_basicsize = sizeof(hpssclapipy_uuid_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Universally Unique Identifier",
  .tp_dealloc   = (destructor)hpssclapipy_uuid_dealloc,
  .tp_repr      = (reprfunc)hpssclapipy_uuid_repr,
};

/* Deallocator */
static void hpssclapipy_uuid_dealloc(hpssclapipy_uuid_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

/* Representation */
static PyObject* hpssclapipy_uuid_repr(hpssclapipy_uuid_t *self) {
#ifndef HPSS751
  char *uuid;
  int status;
  PyObject *rc;

  uuid_to_string(&self->data, &uuid, &status);
  if(status != 0)
    return NULL;

  rc = PyString_FromString(uuid);
  free(uuid);
#else
  PyObject *rc = PyString_FromString("N/A");
#endif  

  return rc;
}
