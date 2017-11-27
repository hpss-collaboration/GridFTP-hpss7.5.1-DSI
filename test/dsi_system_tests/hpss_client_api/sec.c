// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/sec.c $ $Id: sec.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <hpss_api.h>
#include <hpss_mech.h>
#include <hpss_sec.h>
#include <hpss_String.h>
#include "hpssclapipy.h"

/* Deallocator */
static void      hpssclapipy_sec_cred_dealloc(hpssclapipy_sec_cred_t *self);

/* Getters and Setters */
static PyObject* hpssclapipy_sec_cred_get(hpssclapipy_sec_cred_t *self, void *closure);

/* Representation */
static PyObject* hpssclapipy_sec_cred_repr(hpssclapipy_sec_cred_t *self);

typedef enum {
  FIELD_Name,
  FIELD_RealmName,
  FIELD_Directory,
  FIELD_UserShell,
  FIELD_RealmId,
  FIELD_Uid,
  FIELD_Gid,
  FIELD_Uuid,
  FIELD_DefAccount,
  FIELD_CurAccount,
  FIELD_NumGroups,
  FIELD_AltGroups,
} field_t;

static PyGetSetDef hpssclapipy_sec_cred_getset[] = {
#define SEC_CRED(x, doc) \
  { #x, (getter)hpssclapipy_sec_cred_get, (setter)NULL, doc, (void*)FIELD_##x, }
  SEC_CRED(Name,       "User name"),
  SEC_CRED(RealmName,  "Realm name"),
  SEC_CRED(Directory,  "Directory name"),
  SEC_CRED(UserShell,  "User's shell"),
  SEC_CRED(RealmId,    "Realm ID"),
  SEC_CRED(Uid,        "Unix user ID"),
  SEC_CRED(Gid,        "Unix group ID"),
  SEC_CRED(Uuid,       "Unique user ID"),
  SEC_CRED(DefAccount, "Accounting code when one has not been specified"),
  SEC_CRED(CurAccount, "Accounting code to apply to new files and directories"),
  SEC_CRED(NumGroups,  "Number of alternate groups"),
  SEC_CRED(AltGroups,  "List of alternate groups"),

  { NULL, NULL, NULL, NULL, },
};

PyTypeObject hpssclapipy_sec_credType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.sec_cred",
  .tp_basicsize = sizeof(hpssclapipy_sec_cred_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Security Credentials",
  .tp_dealloc   = (destructor)hpssclapipy_sec_cred_dealloc,
  .tp_getset    = hpssclapipy_sec_cred_getset,
  .tp_repr      = (reprfunc)hpssclapipy_sec_cred_repr,
};

/* Deallocator
 * This class can only be created by hpss.SECGetCredsByUid(),
 * so it has no allocator or initializer
 */
static void hpssclapipy_sec_cred_dealloc(hpssclapipy_sec_cred_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

/* Getters and Setters */
static PyObject* hpssclapipy_sec_cred_get(hpssclapipy_sec_cred_t *self, void *closure) {
  field_t field = (field_t)closure;
  int i;
  PyObject *rc, *item;

  switch(field) {
    GET_STR(Name);
    GET_STR(RealmName);
    GET_STR(Directory);
    GET_STR(UserShell);
    GET_INT(RealmId);
    GET_INT(Uid);
    GET_INT(Gid);
    GET_OBJ(Uuid, uuid);
    GET_INT(DefAccount);
    GET_INT(CurAccount);
    GET_INT(NumGroups);

    case FIELD_AltGroups:
      rc = PyList_New(0);
      if(rc == NULL)
        return PyErr_NoMemory();

      for(i = 0; i < self->data.NumGroups; i++) {
        item = Py_BuildValue("i", self->data.AltGroups[i]);
        if(item == NULL || PyList_Append(rc, item) != 0) {
          Py_DECREF(rc);
          return PyErr_NoMemory();
        }
        Py_DECREF(item);
      }
      return rc;

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

/* Representation */
static PyObject* hpssclapipy_sec_cred_repr(hpssclapipy_sec_cred_t *self) {
  char *str;
  PyObject *rc;
  int i;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "sec_cred_t {\n");
  indent += 2;

  PRINT_STR(Name);
  PRINT_STR(RealmName);
  PRINT_STR(Directory);
  PRINT_STR(UserShell);
  PRINT_INT(RealmId);
  PRINT_INT(Uid);
  PRINT_INT(Gid);
  PRINT_OBJ(Uuid);
  PRINT_INT(DefAccount);
  PRINT_INT(CurAccount);
  PRINT_INT(NumGroups);

  INDENT; strcat(str, "AltGroups = {\n");
  indent += 2;
  for(i = 0; i < self->data.NumGroups; i++) {
    INDENT; sprintf(str+strlen(str), "%d,\n", self->data.AltGroups[i]);
  }
  indent -= 2;
  INDENT; strcat(str, "},\n");

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

PyObject* hpssclapipy_GetThreadUcred(PyObject *self)
{
   hpssclapipy_sec_cred_t *creds;
   int rc;

   creds = TP_ALLOC(sec_cred);
   if (NULL == creds)
   {
      return PyErr_NoMemory();
   }

   Py_BEGIN_ALLOW_THREADS
   rc = hpss_GetThreadUcred(&creds->data);
   Py_END_ALLOW_THREADS

   if (rc !=0)
   {
      Py_DECREF(creds);
      return PyErr_SetErrno(-rc);
   }

   return (PyObject*) creds;
}

PyObject* hpssclapipy_SECGetCredsByUid(PyObject *self, PyObject *args) {
  char *realm = NULL;
  int  uid;
  int  rc;
  hpssclapipy_sec_cred_t *creds;

  if(!PyArg_ParseTuple(args, "i|s", &uid, &realm))
    return NULL;

  creds = TP_ALLOC(sec_cred);
  if(creds == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SECGetCredsByUid(uid, realm, &creds->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(creds);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)creds;
}

PyObject* hpssclapipy_SECGetCredsByName(PyObject *self, PyObject *args) {
  char *realm = NULL;
  char *user_name = NULL;
  int  rc;
  hpssclapipy_sec_cred_t *creds;

  if(!PyArg_ParseTuple(args, "s|s", &user_name, &realm))
    return NULL;

  creds = TP_ALLOC(sec_cred);
  if(creds == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SECGetCredsByName(user_name, realm, &creds->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(creds);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)creds;
}

PyObject* hpssclapipy_SECGetGroupNameById(PyObject *self, PyObject *args) {
  char *realm = NULL;
  int  gid;
  int  rc;
  PyObject *group;

  if(!PyArg_ParseTuple(args, "i|s", &gid, &realm))
    return NULL;

  group = PyString_FromStringAndSize(NULL, HPSS_MAX_GROUP_NAME);
  if(group == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SECGetGroupNameById(realm, gid, PyString_AS_STRING(group));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(group);
    return PyErr_SetErrno(-rc);
  }

  if(_PyString_Resize(&group, strlen(PyString_AS_STRING(group)))) {
    PyErr_SetString(PyExc_AssertionError, "This shouldn't happen");
    Py_XDECREF(group);
    return NULL;
  }

  return group;
}

PyObject* hpssclapipy_ParseAuthString(PyObject *self, PyObject *args, PyObject *keywds) {
  char                 *auth_str;
  hpss_authn_mech_t    auth_mech;
  hpss_rpc_auth_type_t auth_type;
  int      rc;
  PyObject *tuple;
  void     *authPtr;
  static char *kwlist[] = { "auth_str", "auth_mech", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "si", kwlist,
                                &auth_str, &auth_mech))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_ParseAuthString(auth_str,
                            &auth_mech,
                            &auth_type,
                            &authPtr);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  tuple = Py_BuildValue("(is)", auth_type, authPtr);
  free(authPtr);

  return tuple;
}

PyObject* hpssclapipy_SetLoginCred(PyObject *self, PyObject *args, PyObject *keywds){
  int auth_mech, auth_type, rc, cred_type;
  char *princ, *authenticator;

  static char *kwlist[] = { "princ", "auth_mech", "cred_type", "auth_type", "authenticator", NULL};

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "siiis", kwlist,
                                  &princ, &auth_mech, &cred_type, &auth_type, &authenticator))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SetLoginCred(princ,
                         auth_mech,
                         cred_type,
                         auth_type,
                         authenticator);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_PurgeLoginCred(PyObject *self) {
  Py_BEGIN_ALLOW_THREADS
  hpss_PurgeLoginCred();
  Py_END_ALLOW_THREADS

  Py_RETURN_NONE;
}
/*

PyObject* hpssclapi_ConvertNamesToIds(PyObject *self, PyObject *args)
{

   if(!PyArg_ParseTuple(args, "iO", &gid, &realm))
     return NULL;
Py_BEGIN_ALLOW_THREADS
rc = hpss_SECGetGroupNameById(realm, gid, PyString_AS_STRING(group));
Py_END_ALLOW_THREADS

if(rc != 0) {
  Py_DECREF(group);
  return PyErr_SetErrno(-rc);
}
}
*/
