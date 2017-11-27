#include <Python.h>
#include <structmember.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

/* Deallocator */
static void hpssclapipy_dirent_dealloc(hpssclapipy_dirent_t *self);

/* Getters */
static PyObject* hpssclapipy_dirent_get(hpssclapipy_dirent_t *self, void *closure);

/* Representation */
static PyObject* hpssclapipy_dirent_repr(hpssclapipy_dirent_t *self);

typedef enum {
  FIELD_d_offset,
  FIELD_d_handle,
  FIELD_d_reclen,
  FIELD_d_namelen,
  FIELD_d_name,
} field_t;

static PyGetSetDef hpssclapipy_dirent_getset[] = {
#define DIRENT(x, doc) \
  { #x, (getter)hpssclapipy_dirent_get, (setter)NULL, doc, (void*)FIELD_##x, }
  DIRENT(d_offset,  "Offset to next entry"
  ),
  DIRENT(d_handle,  "HPSS NS Object Handle"
  ),
  DIRENT(d_reclen,  "Record Length"
  ),
  DIRENT(d_namelen, "Length of entry name"
  ),
  DIRENT(d_name,    "Directory entry name"
  ),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definition */
PyTypeObject hpssclapipy_direntType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.dirent",
  .tp_basicsize = sizeof(hpssclapipy_dirent_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Directory Entry",
  .tp_dealloc   = (destructor)hpssclapipy_dirent_dealloc,
  .tp_getset    = hpssclapipy_dirent_getset,
  .tp_repr      = (reprfunc)hpssclapipy_dirent_repr,
};


/* Deallocator
 * This class can only be created by hpss.Readdir(),
 * so it has no allocator or initializer
 */
static void hpssclapipy_dirent_dealloc(hpssclapipy_dirent_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}


/* Getters */
static PyObject* hpssclapipy_dirent_get(hpssclapipy_dirent_t *self, void *closure) {
  field_t field = (field_t)closure;

  switch(field) {
    GET_U64(d_offset);
    GET_OBJ(d_handle, ns_ObjHandle);
    GET_INT(d_reclen);
    GET_INT(d_namelen);
    GET_STR(d_name);
    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}


/* Representation */
static PyObject* hpssclapipy_dirent_repr(hpssclapipy_dirent_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_dirent_t {\n");
  indent += 2;

  PRINT_U64(d_offset);
  PRINT_OBJ(d_handle);
  PRINT_INT(d_reclen);
  PRINT_INT(d_namelen);
  PRINT_STR(d_name);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}
