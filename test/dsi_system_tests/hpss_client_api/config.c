#include <Python.h>
#include <structmember.h>
#include <hpss_api.h>
#include <hpss_String.h>
#include "hpssclapipy.h"

/* Deallocator */
static void      hpssclapipy_api_config_dealloc(hpssclapipy_api_config_t *self);

/* Getters and Setters */
static int       hpssclapipy_api_config_set(hpssclapipy_api_config_t *self, PyObject *value, void *closure);
static PyObject* hpssclapipy_api_config_get(hpssclapipy_api_config_t *self, void     *closure);

/* Representation */
static PyObject* hpssclapipy_api_config_repr(hpssclapipy_api_config_t *self);

typedef enum {
  FIELD_Flags,
  FIELD_DebugValue,
  FIELD_TransferType,
  FIELD_NumRetries,
  FIELD_BusyDelay,
  FIELD_BusyRetries,
  FIELD_TotalDelay,
  FIELD_LimitedRetries,
  FIELD_MaxConnections,
  FIELD_ReuseDataConnections,
  FIELD_UsePortRange,
  FIELD_RetryStageInp,
  FIELD_DMAPWriteUpdates,
  FIELD_AuthnMech,
  FIELD_RPCProtLevel,
  FIELD_DescName,
  FIELD_DebugPath,
  FIELD_HostName,
  FIELD_XMLSize,
} field_t;

static PyGetSetDef hpssclapipy_api_config_getset[] = {
#define CONFIG(x, doc) \
  { #x, (getter)hpssclapipy_api_config_get, (setter)hpssclapipy_api_config_set, doc, (void*)FIELD_##x, }

  CONFIG(Flags,            "Configuration Flags\n"
                           "Bitwise-or of the following:\n"
                           "  API_ENABLE_LOGGING\n"
                           "  API_USE_CONFIG\n"
                           "  API_DISABLE_CROSS_REALM\n"
                           "  API_DISABLE_JUNCTIONS"
  ),
  CONFIG(DebugValue,       "Debug logging verbosity"
  ),
  CONFIG(TransferType,     "Data transfer mechanism\n"
                           "Any of the following:\n"
                           "  API_TRANSFER_TCP\n"
                           "  API_TRANSFER_IPI3\n"
                           "  API_TRANSFER_MVRSELECT"
  ),
  CONFIG(NumRetries,       "Number of retries to attempt when an operation fails\n"
                           "Special values:\n"
                           "  0: No retries\n"
                           " -1: Infinite retries"
  ),
  CONFIG(BusyDelay,        "Number of seconds between retry attempts"
  ),
  CONFIG(BusyRetries,      "Number of retries to attempt when the Core Server is busy\n"
                           "Special values:\n"
                           "  0: No retries\n"
                           " -1: Infinite or until a different error occurs"
  ),
  CONFIG(TotalDelay,       "Total seconds to continue retrying"
  ),
  CONFIG(LimitedRetries,   "Number of retries to attempt for limited retry type errors"
  ),
  CONFIG(MaxConnections,   "Max number of connections for use by HPSS connection service"
  ),
  CONFIG(ReuseDataConnections,
                           "Whether to keep connections open while a file is open\n"
                           "    0: Close when done with read/write\n"
                           "non-0: Keep connection open until file closed"
  ),
  CONFIG(UsePortRange,     "Whether Movers should use the configured port range for requests\n"
                           "    0: Do not use the port range\n"
                           "non-0: Use the port range"
  ),
  CONFIG(RetryStageInp,    "Whether to retry opening files that are set to background stage on open\n"
                           "    0: open will be retried\n"
                           "non-0: -EINPROGRESS is returned instead of retrying"
  ),
  CONFIG(DMAPWriteUpdates, "Frequency of cache invalidates issued to XDSM file system"
  ),
  CONFIG(AuthnMech,        "Authentication mechanism to use\n"
                           "  hpss_authn_mech_krb5\n"
                           "  hpss_authn_mech_unix"
  ),
  CONFIG(RPCProtLevel,     "Level of RPC protection to use\n"
                           "  hpss_rpc_protect_connect\n"
                           "  hpss_rpc_protect_pkt\n"
                           "  hpss_rpc_protect_pkt_integ\n"
                           "  hpss_rpc_protect_pkt_privacy"
  ),
  CONFIG(DescName,         "Name to use when generating HPSS log messages"
  ),
  CONFIG(DebugPath,        "Pathname of file to use for log output\n"
                           "Special values:\n"
                           "  stdout\n"
                           "  stderr"
  ),
  CONFIG(HostName,         "Interface hostname to use for TCP/IP communications"
  ),
  CONFIG(XMLSize,          "Maximum size for an XML Query"
  ),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definition */
PyTypeObject hpssclapipy_api_configType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.api_config",
  .tp_basicsize = sizeof(hpssclapipy_api_config_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS API Configuration",
  .tp_dealloc   = (destructor)hpssclapipy_api_config_dealloc,
  .tp_getset    = hpssclapipy_api_config_getset,
  .tp_repr      = (reprfunc)hpssclapipy_api_config_repr,
};


/* Deallocator
 * This class can only be created by hpss.GetConfiguration(),
 * so it has no allocator or initializer
 */
static void hpssclapipy_api_config_dealloc(hpssclapipy_api_config_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}


/* Getters and Setters */
static int hpssclapipy_api_config_set(hpssclapipy_api_config_t *self, PyObject *value, void *closure) {
  field_t field = (field_t)closure;

  if(value == NULL) {
    PyErr_SetString(PyExc_AttributeError, "Cannot delete this attribute");
    return -1;
  }

  switch(field) {
    SET_INT(Flags);
    SET_INT(DebugValue);
    SET_INT(TransferType);
    SET_INT(NumRetries);
    SET_INT(BusyDelay);
    SET_INT(BusyRetries);
    SET_INT(TotalDelay);
    SET_INT(LimitedRetries);
    SET_INT(MaxConnections);
    SET_INT(ReuseDataConnections);
    SET_INT(UsePortRange);
    SET_INT(RetryStageInp);
    SET_INT(DMAPWriteUpdates);
    SET_INT(AuthnMech);
    SET_INT(RPCProtLevel);
    SET_STR(DescName);
    SET_STR(DebugPath);
    SET_STR(HostName);
    SET_INT(XMLSize);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return -1;
  }
}

static PyObject* hpssclapipy_api_config_get(hpssclapipy_api_config_t *self, void *closure) {
  field_t field = (field_t)closure;

  switch(field) {
    GET_INT(Flags);
    GET_INT(DebugValue);
    GET_INT(TransferType);
    GET_INT(NumRetries);
    GET_INT(BusyDelay);
    GET_INT(BusyRetries);
    GET_INT(TotalDelay);
    GET_INT(LimitedRetries);
    GET_INT(MaxConnections);
    GET_INT(ReuseDataConnections);
    GET_INT(UsePortRange);
    GET_INT(RetryStageInp);
    GET_INT(DMAPWriteUpdates);
    GET_INT(AuthnMech);
    GET_INT(RPCProtLevel);
    GET_STR(DescName);
    GET_STR(DebugPath);
    GET_STR(HostName);
    GET_INT(XMLSize);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

/* Representation */
static PyObject* hpssclapipy_api_config_repr(hpssclapipy_api_config_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "api_config_t {\n");
  indent += 2;

  INDENT; strcat(str, "Flags: ");
  if((self->data.Flags & API_CONFIG_ALL_FLAGS) == 0)
    strcat(str, "0,\n");
  else {
    int  first = 1;
    PRINT_FLAG(Flags, API_ENABLE_LOGGING);
    PRINT_FLAG(Flags, API_DISABLE_CROSS_REALM);
    PRINT_FLAG(Flags, API_DISABLE_JUNCTIONS);
    PRINT_FLAG(Flags, API_USE_CONFIG);
    PRINT_FLAG(Flags, API_USE_SAN3P);
    strcat(str, ",\n");
  }

  PRINT_INT(DebugValue);

  switch(self->data.TransferType) {
    PRINT_ENUM(TransferType, API_TRANSFER_TCP);
    PRINT_ENUM(TransferType, API_TRANSFER_MVRSELECT);
    /* PRINT_ENUM(TransferType, API_TRANSFER_IPI3); */
    default:
      PRINT_INT(TransferType);
  }

  PRINT_INT(NumRetries);
  PRINT_INT(BusyDelay);
  PRINT_INT(BusyRetries);
  PRINT_INT(TotalDelay);
  PRINT_INT(LimitedRetries);
  PRINT_INT(MaxConnections);
  PRINT_INT(ReuseDataConnections);
  PRINT_INT(UsePortRange);
  PRINT_INT(RetryStageInp);
  PRINT_INT(DMAPWriteUpdates);

  switch(self->data.AuthnMech) {
    PRINT_ENUM(AuthnMech, hpss_authn_mech_invalid);
    PRINT_ENUM(AuthnMech, hpss_authn_mech_krb5);
    PRINT_ENUM(AuthnMech, hpss_authn_mech_unix);
    PRINT_ENUM(AuthnMech, hpss_authn_mech_gsi);
    PRINT_ENUM(AuthnMech, hpss_authn_mech_spkm);
    default:
      PRINT_INT(AuthnMech);
  }

  switch(self->data.RPCProtLevel) {
    PRINT_ENUM(RPCProtLevel, hpss_rpc_protect_connect);
    PRINT_ENUM(RPCProtLevel, hpss_rpc_protect_pkt);
    PRINT_ENUM(RPCProtLevel, hpss_rpc_protect_pkt_integ);
    PRINT_ENUM(RPCProtLevel, hpss_rpc_protect_pkt_privacy);
    default:
      PRINT_INT(RPCProtLevel);
  }

  PRINT_STR(DescName);
  PRINT_STR(DebugPath);
  PRINT_STR(HostName);
  PRINT_INT(XMLSize);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

PyObject* hpssclapipy_GetConfiguration(PyObject *self) {
  int rc;
  hpssclapipy_api_config_t *config;

  config = TP_ALLOC(api_config);
  if(config == NULL)
     return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_GetConfiguration(&config->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(config);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)config;
}

PyObject* hpssclapipy_SetConfiguration(PyObject *self, PyObject *args) {
  int rc;
  hpssclapipy_api_config_t *config;

  if(!PyArg_ParseTuple(args, "O!", &hpssclapipy_api_configType, &config))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_SetConfiguration(&config->data);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}
