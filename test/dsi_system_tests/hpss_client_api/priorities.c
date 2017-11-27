// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/priorities.c $ $Id: priorities.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <structmember.h>
#include <stdio.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

/* Allocator, initializer, and deallocator */
static PyObject* hpssclapipy_cos_priorities_new    (PyTypeObject   *type, PyObject *args, PyObject *kwds);
static int       hpssclapipy_cos_priorities_init   (hpssclapipy_cos_priorities_t *self, PyObject *args, PyObject *kwds);
static void      hpssclapipy_cos_priorities_dealloc(hpssclapipy_cos_priorities_t *self);

/* Getters and Setters */
static int       hpssclapipy_cos_priorities_set(hpssclapipy_cos_priorities_t *self, PyObject *value, void *closure);
static PyObject* hpssclapipy_cos_priorities_get(hpssclapipy_cos_priorities_t *self, void     *closure);

/* Representation */
static PyObject* hpssclapipy_cos_priorities_repr(hpssclapipy_cos_priorities_t *self);

typedef enum {
  FIELD_COSIdPriority,
  FIELD_COSNamePriority,
  FIELD_OptimumAccessSizePriority,
  FIELD_MinFileSizePriority,
  FIELD_MaxFileSizePriority,
  FIELD_AccessFrequencyPriority,
  FIELD_TransferRatePriority,
  FIELD_AvgLatencyPriority,
  FIELD_WriteOpsPriority,
  FIELD_ReadOpsPriority,
  FIELD_StageCodePriority,
  FIELD_StripeWidthPriority,
  FIELD_StripeLengthPriority,
  FIELD_FamilyIdPriority,
} field_t;

static PyGetSetDef hpssclapipy_cos_priorities_getset[] = {
#define PRIORITIES(x, doc) \
  { #x, (getter)hpssclapipy_cos_priorities_get, (setter)hpssclapipy_cos_priorities_set, doc, (void*)FIELD_##x, }

  PRIORITIES(COSIdPriority,             "COS ID Priority"),
  PRIORITIES(COSNamePriority,           "COS Name Priority"),
  PRIORITIES(OptimumAccessSizePriority, "Optimum Access Size Priority"),
  PRIORITIES(MinFileSizePriority,       "Minimum File Size Priority"),
  PRIORITIES(MaxFileSizePriority,       "Maximum File Size Priority"),
  PRIORITIES(AccessFrequencyPriority,   "Access Frequency Priority"),
  PRIORITIES(TransferRatePriority,      "Transfer Rate Priority"),
  PRIORITIES(AvgLatencyPriority,        "Average Latency Priority"),
  PRIORITIES(WriteOpsPriority,          "Write Operations Priority"),
  PRIORITIES(ReadOpsPriority,           "Read Operations Priority"),
  PRIORITIES(StageCodePriority,         "Stage Code Priority"),
  PRIORITIES(StripeWidthPriority,       "Stripe Width Priority"),
  PRIORITIES(StripeLengthPriority,      "Stripe Length Priority"),
  PRIORITIES(FamilyIdPriority,          "File Family ID Priority"),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definition */
PyTypeObject hpssclapipy_cos_prioritiesType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.cos_priorities",
  .tp_basicsize = sizeof(hpssclapipy_cos_priorities_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS COS Priorities",
  .tp_new       = hpssclapipy_cos_priorities_new,
  .tp_init      = (initproc)hpssclapipy_cos_priorities_init,
  .tp_dealloc   = (destructor)hpssclapipy_cos_priorities_dealloc,
  .tp_getset    = hpssclapipy_cos_priorities_getset,
  .tp_repr      = (reprfunc)hpssclapipy_cos_priorities_repr,
};


/* Allocator, initializer, and deallocator */
static PyObject* hpssclapipy_cos_priorities_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  return type->tp_alloc(type, 0);
}

static int hpssclapipy_cos_priorities_init(hpssclapipy_cos_priorities_t *self, PyObject *args, PyObject *kwds) {
  memset(&self->data, 0, sizeof(self->data));
  return 0;
}

static void hpssclapipy_cos_priorities_dealloc(hpssclapipy_cos_priorities_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}


/* Getters and Setters */
static int hpssclapipy_cos_priorities_set(hpssclapipy_cos_priorities_t *self, PyObject *value, void *closure) {
  field_t field = (field_t)closure;

  if(value == NULL) {
    PyErr_SetString(PyExc_AttributeError, "Cannot delete this attribute");
    return -1;
  }

  switch(field) {
    SET_INT(COSIdPriority);
    SET_INT(COSNamePriority);
    SET_INT(OptimumAccessSizePriority);
    SET_INT(MinFileSizePriority);
    SET_INT(MaxFileSizePriority);
    SET_INT(AccessFrequencyPriority);
    SET_INT(TransferRatePriority);
    SET_INT(AvgLatencyPriority);
    SET_INT(WriteOpsPriority);
    SET_INT(ReadOpsPriority);
    SET_INT(StageCodePriority);
    SET_INT(StripeWidthPriority);
    SET_INT(StripeLengthPriority);
    SET_INT(FamilyIdPriority);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return -1;
  }
}

static PyObject* hpssclapipy_cos_priorities_get(hpssclapipy_cos_priorities_t *self, void *closure) {
  field_t field = (field_t)closure;

  switch(field) {
    GET_INT(COSIdPriority);
    GET_INT(COSNamePriority);
    GET_INT(OptimumAccessSizePriority);
    GET_INT(MinFileSizePriority);
    GET_INT(MaxFileSizePriority);
    GET_INT(AccessFrequencyPriority);
    GET_INT(TransferRatePriority);
    GET_INT(AvgLatencyPriority);
    GET_INT(WriteOpsPriority);
    GET_INT(ReadOpsPriority);
    GET_INT(StageCodePriority);
    GET_INT(StripeWidthPriority);
    GET_INT(StripeLengthPriority);
    GET_INT(FamilyIdPriority);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

/* Representation */
static PyObject* hpssclapipy_cos_priorities_repr(hpssclapipy_cos_priorities_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_cos_priorities_t {\n");
  indent += 2;

#define PRINT_PRI(x) \
  switch(self->data.x##Priority) { \
    PRINT_ENUM(x##Priority, NO_PRIORITY); \
    PRINT_ENUM(x##Priority, LOWEST_PRIORITY); \
    PRINT_ENUM(x##Priority, LOW_PRIORITY); \
    PRINT_ENUM(x##Priority, DESIRED_PRIORITY); \
    PRINT_ENUM(x##Priority, HIGHLY_DESIRED_PRIORITY); \
    PRINT_ENUM(x##Priority, REQUIRED_PRIORITY); \
    default: \
      PRINT_INT(x##Priority); \
  }

  PRINT_PRI(COSId);
  PRINT_PRI(COSName);
  PRINT_PRI(OptimumAccessSize);
  PRINT_PRI(MinFileSize);
  PRINT_PRI(MaxFileSize);
  PRINT_PRI(AccessFrequency);
  PRINT_PRI(TransferRate);
  PRINT_PRI(AvgLatency);
  PRINT_PRI(WriteOps);
  PRINT_PRI(ReadOps);
  PRINT_PRI(StageCode);
  PRINT_PRI(StripeWidth);
  PRINT_PRI(StripeLength);
  PRINT_PRI(FamilyId);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}
