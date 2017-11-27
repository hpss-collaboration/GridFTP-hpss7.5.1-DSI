// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/hints.c $ $Id: hints.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <structmember.h>
#include <stdio.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

/* Allocator, initializer, and deallocator */
static PyObject* hpssclapipy_cos_hints_new    (PyTypeObject       *type, PyObject *args, PyObject *kwds);
static int       hpssclapipy_cos_hints_init   (hpssclapipy_cos_hints_t *self, PyObject *args, PyObject *kwds);
static void      hpssclapipy_cos_hints_dealloc(hpssclapipy_cos_hints_t *self);

/* Getters and Setters */
static int       hpssclapipy_cos_hints_set(hpssclapipy_cos_hints_t *self, PyObject *value, void *closure);
static PyObject* hpssclapipy_cos_hints_get(hpssclapipy_cos_hints_t *self, void     *closure);

/* Representation */
static PyObject* hpssclapipy_cos_hints_repr(hpssclapipy_cos_hints_t *self);

typedef enum {
  FIELD_COSId,
  FIELD_COSName,
  FIELD_Flags,
  FIELD_OptimumAccessSize,
  FIELD_MinFileSize,
  FIELD_MaxFileSize,
  FIELD_AccessFrequency,
  FIELD_TransferRate,
  FIELD_AvgLatency,
  FIELD_WriteOps,
  FIELD_ReadOps,
  FIELD_StageCode,
  FIELD_StripeWidth,
  FIELD_StripeLength,
  FIELD_FamilyId,
} field_t;

static PyGetSetDef hpssclapipy_cos_hints_getset[] = {
#define HINTS(x, doc) \
  { #x, (getter)hpssclapipy_cos_hints_get, (setter)hpssclapipy_cos_hints_set, doc, (void*)FIELD_##x, }

  HINTS(COSId,             "Class of Service for the bitfile"
  ),
  HINTS(COSName,           "Name of the Class of Service for the bitfile"
  ),
  HINTS(Flags,             "Configuration Flags\n"
                           "Bitwise-or of the following:\n"
                           "  HINTS_FORCE_MAX_SSEG\n"
                           "  HINTS_NOTRUNC_FINAL_SEG\n"
  ),
  HINTS(OptimumAccessSize, "Block size in bytes that yields maximum data transfer rate"
  ),
  HINTS(MinFileSize,       "Minimum file size"
  ),
  HINTS(MaxFileSize,       "Maximum file size"
  ),
  HINTS(AccessFrequency,   "Expected rate of access\n"
                           "Any of the following:\n"
                           "  FREQ_HOURLY\n"
                           "  FREQ_DAILY\n"
                           "  FREQ_WEEKLY\n"
                           "  FREQ_MONTHLY\n"
                           "  FREQ_ARCHIVE\n"
  ),
  HINTS(TransferRate,      "Approximate file transfer rate in KB/s"
  ),
  HINTS(AvgLatency,        "Time in seconds from when a request is received by"
                           " Core Server until data is actually transmitted"
  ),
  HINTS(WriteOps,          "Valid write operations for the bitfile\n"
                           "Bitwise-or of the following:\n"
                           "  HPSS_OP_WRITE\n"
                           "  HPSS_OP_APPEND\n"
  ),
  HINTS(ReadOps,           "Valid read operations for the bitfile\n"
                           "  HPSS_OP_READ\n"
  ),
  HINTS(StageCode,         "Staging behavior\n"
                           "Any of the following:\n"
                           "  COS_STAGE_NO_STAGE\n"
                           "  COS_STAGE_ON_OPEN\n"
                           "  COS_STAGE_ON_OPEN_ASYNCH\n"
                           "  COS_STAGE_ON_OPEN_BACKGROUND\n"
  ),
  HINTS(StripeWidth,       "Stripe width"
  ),
  HINTS(StripeLength,      "Stripe length"
  ),
  HINTS(FamilyId,          "File Family ID"
  ),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definition */
PyTypeObject hpssclapipy_cos_hintsType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.cos_hints",
  .tp_basicsize = sizeof(hpssclapipy_cos_hints_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS COS Hints",
  .tp_new       = hpssclapipy_cos_hints_new,
  .tp_init      = (initproc)hpssclapipy_cos_hints_init,
  .tp_dealloc   = (destructor)hpssclapipy_cos_hints_dealloc,
  .tp_getset    = hpssclapipy_cos_hints_getset,
  .tp_repr      = (reprfunc)hpssclapipy_cos_hints_repr,
};


/* Allocator, initializer, and deallocator */
static PyObject* hpssclapipy_cos_hints_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  return type->tp_alloc(type, 0);
}

static int hpssclapipy_cos_hints_init(hpssclapipy_cos_hints_t *self, PyObject *args, PyObject *kwds) {
  memset(&self->data, 0, sizeof(self->data));
  return 0;
}

static void hpssclapipy_cos_hints_dealloc(hpssclapipy_cos_hints_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}


/* Getters and Setters */
static int hpssclapipy_cos_hints_set(hpssclapipy_cos_hints_t *self, PyObject *value, void *closure) {
  field_t field = (field_t)closure;

  if(value == NULL) {
    PyErr_SetString(PyExc_AttributeError, "Cannot delete this attribute");
    return -1;
  }

  switch(field) {
    SET_INT(COSId);
    SET_STR(COSName);
    SET_INT(Flags);
    SET_U64(OptimumAccessSize);
    SET_U64(MinFileSize);
    SET_U64(MaxFileSize);
    SET_INT(AccessFrequency);
    SET_INT(TransferRate);
    SET_INT(AvgLatency);
    SET_INT(WriteOps);
    SET_INT(ReadOps);
    SET_INT(StageCode);
    SET_INT(StripeWidth);
    SET_U64(StripeLength);
    SET_INT(FamilyId);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return -1;
  }
}

static PyObject* hpssclapipy_cos_hints_get(hpssclapipy_cos_hints_t *self, void *closure) {
  field_t field = (field_t)closure;

  switch(field) {
    GET_INT(COSId);
    GET_STR(COSName);
    GET_INT(Flags);
    GET_U64(OptimumAccessSize);
    GET_U64(MinFileSize);
    GET_U64(MaxFileSize);
    GET_INT(AccessFrequency);
    GET_INT(TransferRate);
    GET_INT(AvgLatency);
    GET_INT(WriteOps);
    GET_INT(ReadOps);
    GET_INT(StageCode);
    GET_INT(StripeWidth);
    GET_U64(StripeLength);
    GET_INT(FamilyId);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

/* Representation */
static PyObject* hpssclapipy_cos_hints_repr(hpssclapipy_cos_hints_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_cos_hints_t {\n");
  indent += 2;

  INDENT; strcat(str, "Flags: ");
  if((self->data.Flags & (HINTS_FORCE_MAX_SSEG|HINTS_NOTRUNC_FINAL_SEG)) == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(Flags, HINTS_FORCE_MAX_SSEG);
    PRINT_FLAG(Flags, HINTS_NOTRUNC_FINAL_SEG);
    strcat(str, ",\n");
  }

  PRINT_INT(COSId);
  PRINT_STR(COSName);
  PRINT_U64(OptimumAccessSize);
  PRINT_U64(MinFileSize);
  PRINT_U64(MaxFileSize);

  switch(self->data.AccessFrequency) {
    PRINT_ENUM(AccessFrequency, FREQ_HOURLY);
    PRINT_ENUM(AccessFrequency, FREQ_DAILY);
    PRINT_ENUM(AccessFrequency, FREQ_WEEKLY);
    PRINT_ENUM(AccessFrequency, FREQ_MONTHLY);
    PRINT_ENUM(AccessFrequency, FREQ_ARCHIVE);
    default:
      PRINT_INT(AccessFrequency);
  }

  PRINT_INT(TransferRate);
  PRINT_INT(AvgLatency);

  INDENT; strcat(str, "WriteOps: ");
  if(self->data.WriteOps == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(WriteOps, HPSS_OP_WRITE);
    PRINT_FLAG(WriteOps, HPSS_OP_APPEND);
    strcat(str, ",\n");
  }

  INDENT; strcat(str, "ReadOps: ");
  if(self->data.ReadOps == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(ReadOps,  HPSS_OP_READ);
    strcat(str, ",\n");
  }

  switch(self->data.StageCode) {
    PRINT_ENUM(StageCode, COS_STAGE_NO_STAGE);
    PRINT_ENUM(StageCode, COS_STAGE_ON_OPEN);
    PRINT_ENUM(StageCode, COS_STAGE_ON_OPEN_ASYNCH);
    PRINT_ENUM(StageCode, COS_STAGE_ON_OPEN_BACKGROUND);
    default:
      PRINT_INT(StageCode);
  }

  PRINT_INT(StripeWidth);
  PRINT_U64(StripeLength);
  PRINT_INT(FamilyId);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}
