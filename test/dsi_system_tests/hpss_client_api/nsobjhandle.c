// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/nsobjhandle.c $ $Id: nsobjhandle.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <structmember.h>
#include <hpss_api.h>
#include <hpss_uuid.h>
#include <api_internal.h>
#include <hpss_ObjHandle.h>
#include "hpssclapipy.h"

/* Deallocator */
static void hpssclapipy_ns_ObjHandle_dealloc(hpssclapipy_ns_ObjHandle_t *self);

/* Getters */
static PyObject* hpssclapipy_ns_ObjHandle_get(hpssclapipy_ns_ObjHandle_t *self, void *closure);

/* Representation */
static PyObject* hpssclapipy_ns_ObjHandle_repr(hpssclapipy_ns_ObjHandle_t *self);

typedef enum {
  FIELD_ObjId,
  FIELD_FileId,
  FIELD_Type,
  FIELD_Flags,
  FIELD_Generation,
#ifdef HPSS751
  FIELD_CoreServerId,
#else
  FIELD_CoreServerUUID,
#endif
} field_t;

static PyGetSetDef hpssclapipy_ns_ObjHandle_getset[] = {
#define NS_OBJHANDLE(x, doc) \
  { #x, (getter)hpssclapipy_ns_ObjHandle_get, (setter)NULL, doc, (void*)FIELD_##x, }
  NS_OBJHANDLE(ObjId,          "Object ID"
  ),
  NS_OBJHANDLE(FileId,         "If the Type specifies a hardlink, this is the\n"
                               "ObjId of the original file. Otherwise, it is\n"
                               "equal to the ObjId"
  ),
  NS_OBJHANDLE(Type,           "Type of the object:\n"
                               "  NS_OBJECT_TYPE_DIRECTORY\n"
                               "  NS_OBJECT_TYPE_FILE\n"
                               "  NS_OBJECT_TYPE_JUNCTION\n"
                               "  NS_OBJECT_TYPE_SYM_LINK\n"
                               "  NS_OBJECT_TYPE_HARD_LINK\n"
  ),
  NS_OBJHANDLE(Flags,          "Bitwise-or of the following:\n"
                               "  NS_OH_FLAG_FILESET_ROOT - Root node of a fileset"
  ),
  NS_OBJHANDLE(Generation,     "Random number used to detect stale object handles"
  ),
#ifdef HPSS751
  NS_OBJHANDLE(CoreServerId,   "Core Server ID that issued this object handle"
#else  
  NS_OBJHANDLE(CoreServerUUID, "Core Server UUID that issued this object handle"
#endif  
  ),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definition */
PyTypeObject hpssclapipy_ns_ObjHandleType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.ns_ObjHandle",
  .tp_basicsize = sizeof(hpssclapipy_ns_ObjHandle_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS NS Object Handle",
  .tp_dealloc   = (destructor)hpssclapipy_ns_ObjHandle_dealloc,
  .tp_getset    = hpssclapipy_ns_ObjHandle_getset,
  .tp_repr      = (reprfunc)hpssclapipy_ns_ObjHandle_repr,
};


/* Deallocator
 * This class can only be created by hpss.dirent,
 * so it has no allocator or initializer
 */
static void hpssclapipy_ns_ObjHandle_dealloc(hpssclapipy_ns_ObjHandle_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}


/* Getters */
static PyObject* hpssclapipy_ns_ObjHandle_get(hpssclapipy_ns_ObjHandle_t *self, void *closure) {
  field_t field = (field_t)closure;

  switch(field) {
    GET_U64(ObjId);
    GET_U64(FileId);
    GET_INT(Type);
    GET_INT(Flags);
    GET_INT(Generation);
#ifndef HPSS751    
    // TODO: Look into adding CoreServerId
    GET_OBJ(CoreServerUUID, uuid);
#endif    
    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}


/* Representation */
static PyObject* hpssclapipy_ns_ObjHandle_repr(hpssclapipy_ns_ObjHandle_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "ns_ObjHandle_t {\n");
  indent += 2;

  PRINT_U64(ObjId);
  PRINT_U64(FileId);

  switch(self->data.Type) {
    PRINT_ENUM(Type, NS_OBJECT_TYPE_DIRECTORY);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_FILE);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_JUNCTION);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_SYM_LINK);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_HARD_LINK);
    default:
      PRINT_INT(Type);
  }

  INDENT; strcat(str, "Flags: ");
  if((self->data.Flags & (NS_OH_FLAG_FILESET_ROOT)) == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(Flags, NS_OH_FLAG_FILESET_ROOT);
    strcat(str, ",\n");
  }
  
#ifdef HPSS751
  PRINT_U64(Generation);
  // TODO: look into adding the CoreServerId
  //PRINT_OBJ(CoreServerId);

#else
  PRINT_INT(Generation);
  PRINT_OBJ(CoreServerUUID);
#endif

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

#ifndef HPSS751
PyObject* hpssclapipy_GetObjId(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle;
  unsigned32            rc;

  static char *kwlist[] = { "objhandle", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_GetObjId(&objhandle->data);
  Py_END_ALLOW_THREADS

  if(rc == HPSS_EFAULT)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("I", rc);
}
#endif

PyObject* hpssclapipy_GetObjType(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle;
  unsigned32            rc;

  static char *kwlist[] = { "objhandle", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_GetObjType(&objhandle->data);
  Py_END_ALLOW_THREADS

  if(rc == HPSS_EFAULT || rc == HPSS_EINVAL)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("I", rc);
}

#ifndef HPSS751
PyObject* hpssclapipy_GetPathHandleObjId(PyObject *self, PyObject *args, PyObject *keywds) {
  unsigned long long objid;
  unsigned32 subsys = 1;
  int status;
  hpssclapipy_ns_ObjHandle_t *FSRootHandle, *ObjHandle;
  PyObject *path, *rc;

  static char *kwlist[] = { "objid", "subsysid", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "K|I", kwlist, &objid, &subsys))
    return NULL;

  FSRootHandle = TP_ALLOC(ns_ObjHandle);
  ObjHandle    = TP_ALLOC(ns_ObjHandle);
  path         = PyString_FromStringAndSize(NULL, HPSS_MAX_PATH_NAME+1);

  if(FSRootHandle == NULL || ObjHandle == NULL || path == NULL) {
    Py_XDECREF(FSRootHandle);
    Py_XDECREF(ObjHandle);
    Py_XDECREF(path);
  }

  status = hpss_GetPathHandleObjId(ULL_TO_U64(objid), subsys, &FSRootHandle->data, &ObjHandle->data, PyString_AS_STRING(path));

  if(status != 0) {
    Py_DECREF(FSRootHandle);
    Py_DECREF(ObjHandle);
    Py_DECREF(path);
    return PyErr_SetErrno(-status);
  }

  if(_PyString_Resize(&path, strlen(PyString_AS_STRING(path))) != 0) {
    Py_DECREF(FSRootHandle);
    Py_DECREF(ObjHandle);
    Py_XDECREF(path);
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    return NULL;
  }

  rc = Py_BuildValue("(NNN)", FSRootHandle, ObjHandle, path);
  if(rc == NULL) {
    Py_DECREF(FSRootHandle);
    Py_DECREF(ObjHandle);
    Py_DECREF(path);
    return PyErr_NoMemory();
  }

  return rc;
}
#endif

#ifndef HPSS751
PyObject* hpssclapipy_HandleCompare(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *first, *second;
  unsigned32 rc;

  static char *kwlist[] = { "first", "second", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!O!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &first,
                                  &hpssclapipy_ns_ObjHandleType, &second))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_HandleCompare(&first->data, &second->data);
  Py_END_ALLOW_THREADS

  if(rc == HPSS_EINVAL)
    return PyErr_SetErrno(HPSS_EINVAL);
  else if(rc == 0)
    Py_RETURN_FALSE;

  Py_RETURN_TRUE;
}
#endif
