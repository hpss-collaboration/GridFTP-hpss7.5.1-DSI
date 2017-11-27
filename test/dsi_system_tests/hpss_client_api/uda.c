// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/uda.c $ $Id: uda.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <hpss_api.h>
#include <hpss_xml.h>
#include "hpssclapipy.h"

static inline PyObject* udalist_to_dict(hpss_userattr_list_t *attrs) {
  int i;
  PyObject *dict = PyDict_New();

  if(dict == NULL)
    return PyErr_NoMemory();

  for(i = 0; i < attrs->len; i++) {
    PyObject *val;

    // turn our value into a PyObject
    if(attrs->Pair[i].Value == NULL) {
      // use Py_None for NULL
      Py_INCREF(Py_None);
      val = Py_None;
    }
    else
      // otherwise make a string
      val = PyString_FromString(attrs->Pair[i].Value);

    if(val == NULL) {
      // clean up dictionary and its entries
      Py_DECREF(dict);
      return PyErr_NoMemory();
    }

    // apply this keypair to the dictionary
    if(PyDict_SetItemString(dict, attrs->Pair[i].Key, val) != 0) {
      // clean up the val since we won't use it and the dictionary didn't get
      // it
      Py_DECREF(val);
      // clean up dictionary and its entries
      Py_DECREF(dict);
      return PyErr_NoMemory();
    }

    // we won't reference val anymore
    Py_DECREF(val);
  }

  return dict;
}

/* WARNING: this does not make copies of the strings for Key and Value,
 * so don't clean them up
 */
static inline hpss_userattr_list_t* dict_to_udalist(PyObject *dict) {
  int i = 0;
  Py_ssize_t pos = 0;
  PyObject *key, *val;
  hpss_userattr_list_t *attrs;

  attrs = malloc(sizeof(hpss_userattr_list_t));
  if(attrs == NULL) {
    PyErr_NoMemory();
    return NULL;
  }

  attrs->len  = PyDict_Size(dict);
  attrs->Pair = (hpss_userattr_t*)malloc(sizeof(hpss_userattr_t)*attrs->len);
  if(attrs->Pair == NULL) {
    free(attrs);
    PyErr_NoMemory();
    return NULL;
  }

  while(PyDict_Next(dict, &pos, &key, &val)) {
    // get the key, it MUST be a string
    attrs->Pair[i].Key = PyString_AsString(key);
    if(attrs->Pair[i].Key == NULL) {
      free(attrs->Pair);
      free(attrs);
      // PyString_AsString set an exception
      return NULL;
    }

    // get the value, it MUST be a string or None
    if(val != Py_None) {
      attrs->Pair[i].Value = PyString_AsString(val);
      if(attrs->Pair[i].Value == NULL) {
        free(attrs->Pair);
        free(attrs);
        // PyString_AsString set an exception
        return NULL;
      }
    }
    else
      attrs->Pair[i].Value = NULL;
    i++;
  }

  return attrs;
}

/* WARNING: this does not make copies of the strings for Key,
 * but it will malloc Values of XMLSize from api_config
 */
static inline hpss_userattr_list_t* list_to_udalist(PyObject *list, int alloc)
{
  int i = 0, rc;
  hpss_userattr_list_t *attrs;
  api_config_t config;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_GetConfiguration(&config);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return (hpss_userattr_list_t*)PyErr_SetErrno(-rc);

  attrs = (hpss_userattr_list_t*)malloc(sizeof(hpss_userattr_list_t));
  if(attrs == NULL) {
    PyErr_NoMemory();
    return NULL;
  }

  attrs->len  = PyList_GET_SIZE(list);
  attrs->Pair = (hpss_userattr_t*)malloc(sizeof(hpss_userattr_t)*attrs->len);
  if(attrs->Pair == NULL) {
    free(attrs);
    PyErr_NoMemory();
    return NULL;
  }

  for(i = 0; i < attrs->len; i++) {
    // get the key, it MUST be a string
    attrs->Pair[i].Key = PyString_AsString(PyList_GET_ITEM(list, i));
    if(attrs->Pair[i].Key == NULL) {
      free(attrs->Pair);
      free(attrs);
      // PyString_AsString set an exception
      return NULL;
    }

    if(alloc) {
      attrs->Pair[i].Value = (char*)malloc(config.XMLSize);
      if(attrs->Pair[i].Value == NULL) {
        for(i = i-1; i >= 0; i--)
          free(attrs->Pair[i].Value);
        free(attrs->Pair);
        free(attrs);
        PyErr_NoMemory();
        return NULL;
      }
    }
    else
      attrs->Pair[i].Value = NULL;
  }

  return attrs;
}

static inline void dict_to_udalist_cleanup(hpss_userattr_list_t *attrs) {
  free(attrs->Pair);
  free(attrs);
}

static inline void list_to_udalist_cleanup(hpss_userattr_list_t *attrs) {
  int i;

  for(i = 0; i < attrs->len; i++)
    free(attrs->Pair[i].Value);
  free(attrs->Pair);
  free(attrs);
}

static inline void udalist_cleanup(hpss_userattr_list_t *attrs) {
  int i;

  for(i = 0; i < attrs->len; i++) {
    free(attrs->Pair[i].Key);
    free(attrs->Pair[i].Value);
  }
  free(attrs->Pair);
}

PyObject* hpssclapipy_UserAttrListAttrs(PyObject *self, PyObject *args,
      PyObject *keywds) {
  PyObject             *dict;
  hpss_userattr_list_t attrs;
  char                 *path;
  int                  rc, flags = XML_NO_ATTR, xmlsize = HPSS_XML_SIZE;

  static char *kwlist[] = { "path", "flags", "xmlsize", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s|ii", kwlist, &path, &flags,
        &xmlsize))
    return NULL;

  memset(&attrs, 0, sizeof(attrs));

  Py_BEGIN_ALLOW_THREADS
#ifdef HPSS751
  rc = hpss_UserAttrListAttrs(path, &attrs, flags, xmlsize);
#else
  rc = hpss_UserAttrLenListAttrs(path, &attrs, flags, xmlsize);
#endif
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  // create a dictionary to contain our attr pairs
  dict = udalist_to_dict(&attrs);

  udalist_cleanup(&attrs);

  // if dict is NULL, it will have an exception set already
  return dict;
}

#ifndef HPSS751
PyObject* hpssclapipy_UserAttrLenListAttrs(PyObject *self, PyObject *args,
      PyObject *keywds) {
  return hpssclapipy_UserAttrListAttrs(self, args, keywds);
}
#endif

PyObject* hpssclapipy_UserAttrListAttrHandle(PyObject *self, PyObject *args,
      PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *path      = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  PyObject              *dict;
  int                   rc, flags  = XML_NO_ATTR, xmlsize = HPSS_XML_SIZE;
  hpss_userattr_list_t  attrs;

  static char *kwlist[] = { "objhandle", "path", "ucred", "flags", "xmlsize",
        NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!ii", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path, &hpssclapipy_sec_credType, &ucred,
                                  &flags, &xmlsize))
    return NULL;

  memset(&attrs, 0, sizeof(attrs));

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
#ifdef HPSS751
  rc = hpss_UserAttrListAttrHandle(ObjHandle, path, Ucred, &attrs, flags,
        xmlsize);
#else
  rc = hpss_UserAttrLenListAttrHandle(ObjHandle, path, Ucred, &attrs, flags,
        xmlsize);
#endif
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  dict = udalist_to_dict(&attrs);

  udalist_cleanup(&attrs);

  // if dict is NULL, it will have an exception set already
  return dict;
}

#ifndef HPSS751
PyObject* hpssclapipy_UserAttrLenListAttrHandle(PyObject *self, PyObject *args,
      PyObject *keywds) {
  return hpssclapipy_UserAttrListAttrHandle(self, args, keywds);
}
#endif

PyObject* hpssclapipy_UserAttrGetAttrs(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char *path;
  PyObject *list, *dict;
  int rc, flags = UDA_API_VALUE, xmlsize = HPSS_XML_SIZE;
  hpss_userattr_list_t *attrs;

  static char *kwlist[] = { "path", "keys", "flags", "xmlsize", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "sO!|ii", kwlist,
                                  &path, &PyList_Type, &list,
                                  &flags, &xmlsize))
    return NULL;

  // get a udalist with allocated Values
  attrs = list_to_udalist(list, 1);
  if(attrs == NULL)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
#ifdef HPSS751
  rc = hpss_UserAttrGetAttrs(path, attrs, flags, xmlsize);
#else
  rc = hpss_UserAttrLenGetAttrs(path, attrs, flags, xmlsize);
#endif
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    list_to_udalist_cleanup(attrs);
    return PyErr_SetErrno(-rc);
  }

  dict = udalist_to_dict(attrs);

  list_to_udalist_cleanup(attrs);

  // if dict is NULL, it will have an exception set already
  return dict;
}

#ifndef HPSS751
PyObject* hpssclapipy_UserAttrLenGetAttrs(PyObject *self, PyObject *args,
      PyObject *keywds) {
  return hpssclapipy_UserAttrGetAttrs(self, args, keywds);
}
#endif

PyObject* hpssclapipy_UserAttrGetAttrHandle(PyObject *self, PyObject *args,
      PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *path      = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  PyObject              *list      = NULL, *dict;
  int                   rc, flags  = UDA_API_VALUE, xmlsize = HPSS_XML_SIZE;
  hpss_userattr_list_t  *attrs;

  static char *kwlist[] = { "objhandle", "path", "ucred", "keys", "flags",
        "xmlsize", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!O!ii", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  path,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &PyList_Type, &list,
                                  &flags, &xmlsize))
    return NULL;

  if(list == NULL)
    return PyErr_SetErrno(EINVAL);

  // get a udalist with allocated Values
  attrs = list_to_udalist(list, 1);
  if(attrs == NULL)
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
#ifdef HPSS751
  rc = hpss_UserAttrGetAttrHandle(ObjHandle, path, Ucred, attrs, flags,
        xmlsize);
#else
  rc = hpss_UserAttrLenGetAttrHandle(ObjHandle, path, Ucred, attrs, flags,
        xmlsize);
#endif
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    list_to_udalist_cleanup(attrs);
    return PyErr_SetErrno(-rc);
  }

  dict = udalist_to_dict(attrs);

  list_to_udalist_cleanup(attrs);

  // if dict is NULL, it will have an exception set already
  return dict;
}

#ifndef HPSS751
PyObject* hpssclapipy_UserAttrLenGetAttrHandle(PyObject *self, PyObject *args,
      PyObject *keywds) {
  return hpssclapipy_UserAttrGetAttrHandle(self, args, keywds);
}
#endif

PyObject* hpssclapipy_UserAttrSetAttrs(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char *path, *schema = NULL;
  int rc;
  PyObject *dict;
  hpss_userattr_list_t *attrs;

  static char *kwlist[] = { "path", "attrs", "schema", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "sO!|s", kwlist, &path,
        &PyDict_Type, &dict, &schema))
    return NULL;

  attrs = dict_to_udalist(dict);
  if(attrs == NULL)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UserAttrSetAttrs(path, attrs, schema);
  Py_END_ALLOW_THREADS

  dict_to_udalist_cleanup(attrs);

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UserAttrSetAttrHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *path      = NULL, *schema = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  PyObject              *dict      = NULL;
  int                   rc;
  hpss_userattr_list_t  *attrs;

  static char *kwlist[] = { "objhandle", "path", "ucred", "attrs", "schema",
        NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!O!s", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  path,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &PyDict_Type, &dict,
                                  &schema))
    return NULL;

  if(dict == NULL)
    return PyErr_SetErrno(EINVAL);

  attrs = dict_to_udalist(dict);
  if(attrs == NULL)
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UserAttrSetAttrHandle(ObjHandle, path, Ucred, attrs, schema);
  Py_END_ALLOW_THREADS

  dict_to_udalist_cleanup(attrs);

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UserAttrDeleteAttrs(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char *path, *schema = NULL;
  PyObject *list;
  int rc;
  hpss_userattr_list_t *attrs;

  static char *kwlist[] = { "path", "keys", "schema", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "sO!|s", kwlist, &path,
        &PyList_Type, &list, &schema))
    return NULL;

  // get a udalist without allocated Values
  attrs = list_to_udalist(list, 0);
  if(attrs == NULL)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UserAttrDeleteAttrs(path, attrs, schema);
  Py_END_ALLOW_THREADS

  list_to_udalist_cleanup(attrs);

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UserAttrDeleteAttrHandle(PyObject *self, PyObject *args,
      PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *path      = NULL, *schema = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  PyObject              *list      = NULL;
  int                   rc;
  hpss_userattr_list_t  *attrs;

  static char *kwlist[] = { "objhandle", "path", "ucred", "keys", "schema",
        NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!O!s", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  path,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &PyList_Type, &list,
                                  &schema))
    return NULL;

  if(list == NULL)
    return PyErr_SetErrno(EINVAL);

  // get a udalist without allocated Values
  attrs = list_to_udalist(list, 0);
  if(attrs == NULL)
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UserAttrDeleteAttrHandle(ObjHandle, path, Ucred, attrs, schema);
  Py_END_ALLOW_THREADS

  list_to_udalist_cleanup(attrs);

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_ChompXMLHeader(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char *xml;
  PyObject *rc;

  static char *kwlist[] = { "xml", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &xml))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  xml = hpss_ChompXMLHeader(xml, NULL);
  Py_END_ALLOW_THREADS

  if(xml == NULL)
    return PyErr_NoMemory();

  rc = Py_BuildValue("s", xml);
  free(xml);
  return rc;
}

PyObject* hpssclapipy_UserAttrXQueryUpdate(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char *path, *xquery, *schema = NULL;
  int  rc;

  static char *kwlist[] = { "path", "xquery", "schema", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss|s", kwlist, &path, &xquery,
        &schema))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UserAttrXQueryUpdate(path, xquery, schema);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UserAttrXQueryUpdateHandle(PyObject *self,
      PyObject *args, PyObject *keywds) {
  char                  *path = NULL, *xquery = NULL, *schema = NULL;
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  int                   rc;

  static char *kwlist[] = { "objhandle", "path", "ucred", "xquery", "schema",
        NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!ss", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred, &xquery,
                                  &schema))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_UserAttrXQueryUpdateHandle(ObjHandle, path, Ucred, xquery, schema);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_UserAttrXQueryGet(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char *path, *xquery;
  int status;
  api_config_t config;
  PyObject *rc;

  static char *kwlist[] = { "path", "xquery", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ss", kwlist, &path, &xquery))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_GetConfiguration(&config);
  Py_END_ALLOW_THREADS

  if(status != 0)
    return PyErr_SetErrno(-status);

  rc = PyString_FromStringAndSize(NULL, config.XMLSize);
  if(rc == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  status = hpss_UserAttrXQueryGet(path, xquery, PyString_AS_STRING(rc),
        config.XMLSize);
  Py_END_ALLOW_THREADS

  if(status != 0) {
    Py_DECREF(rc);
    return PyErr_SetErrno(-status);
  }

  if(_PyString_Resize(&rc, strlen(PyString_AS_STRING(rc))) != 0) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(rc);
    return NULL;
  }

  return rc;
}

PyObject* hpssclapipy_UserAttrXQueryGetHandle(PyObject *self, PyObject *args,
      PyObject *keywds) {
  char                  *path = NULL, *xquery = NULL;
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  api_config_t          config;
  PyObject              *rc;
  int                   status;

  static char *kwlist[] = { "objhandle", "path", "ucred", "xquery", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!s", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred, &xquery))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_GetConfiguration(&config);
  Py_END_ALLOW_THREADS

  if(status != 0)
    return PyErr_SetErrno(-status);

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_UserAttrXQueryGetHandle(ObjHandle, path, Ucred, xquery,
        PyString_AS_STRING(rc), config.XMLSize);
  Py_END_ALLOW_THREADS

  if(status != 0) {
    Py_DECREF(rc);
    return PyErr_SetErrno(-status);
  }

  if(_PyString_Resize(&rc, strlen(PyString_AS_STRING(rc))) != 0) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(rc);
    return NULL;
  }

  return rc;
}
