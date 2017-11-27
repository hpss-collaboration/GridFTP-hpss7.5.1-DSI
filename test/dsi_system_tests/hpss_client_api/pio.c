#include <Python.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

/* Allocator */
static PyObject* hpssclapipy_pio_params_new     (PyTypeObject         *type, PyObject *args, PyObject *kwds);

/* Initializer */
static int       hpssclapipy_pio_params_init    (hpssclapipy_pio_params_t  *self, PyObject *args, PyObject *kwds);

/* Deallocator */
static void      hpssclapipy_pio_generic_dealloc(PyObject             *self);

/* Getters and Setters */
static PyObject* hpssclapipy_pio_gapinfo_get    (hpssclapipy_pio_gapinfo_t *self, void     *closure);
static PyObject* hpssclapipy_pio_params_get     (hpssclapipy_pio_params_t  *self, void     *closure);
static int       hpssclapipy_pio_params_set     (hpssclapipy_pio_params_t  *self, PyObject *value, void *closure);

/* Representation */
static PyObject* hpssclapipy_pio_gapinfo_repr   (hpssclapipy_pio_gapinfo_t *self);
static PyObject* hpssclapipy_pio_params_repr    (hpssclapipy_pio_params_t  *self);

typedef enum {
  FIELD_Offset,
  FIELD_Length,
} gapinfo_field_t;

typedef enum {
  FIELD_Operation,
  FIELD_ClntStripeWidth,
  FIELD_BlockSize,
  FIELD_FileStripeWidth,
  FIELD_IOTimeOutSecs,
  FIELD_Transport,
  FIELD_Options,
} params_field_t;

#define GAPINFO(x, doc) { #x, (getter)hpssclapipy_pio_gapinfo_get, (setter)NULL, doc, (void*)FIELD_##x, }
static PyGetSetDef hpssclapipy_pio_gapinfo_getset[] = {
  GAPINFO(Offset, "Starting offset of gap"
  ),
  GAPINFO(Length, "Length of gap"
  ),

  { NULL, NULL, NULL, NULL, NULL, },
};

#define PARAMS(x, doc) { #x, (getter)hpssclapipy_pio_params_get, (setter)hpssclapipy_pio_params_set, doc, (void*)FIELD_##x, }
static PyGetSetDef hpssclapipy_pio_params_getset[] = {
  PARAMS(Operation,       "HPSS_PIO_READ or HPSS_PIO_WRITE"
  ),
  PARAMS(ClntStripeWidth, "Number of data elements in client stripe"
  ),
  PARAMS(BlockSize,       "Number of bytes in each data element in data stripe"
  ),
  PARAMS(FileStripeWidth, "Number of data elements in file stripe"
  ),
  PARAMS(IOTimeOutSecs,   "Number of seconds to wait for I/O\n"
                          "  Special value: 0 means default (30 minutes)"
  ),
  PARAMS(Transport,       "HPSS_PIO_TCPIP or HPSS_PIO_MVR_SELECT"
  ),
  PARAMS(Options,         "Bitwise-or of the following:\n"
                          "  HPSS_PIO_PUSH\n"
                          "  HPSS_PIO_HANDLE_GAP\n"
                          "  HPSS_PIO_PORT_RANGE"
  ),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definitions */
PyTypeObject hpssclapipy_pio_gapinfoType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.pio_gapinfo",
  .tp_basicsize = sizeof(hpssclapipy_pio_gapinfo_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS PIO Gap Info",
  .tp_dealloc   = (destructor)hpssclapipy_pio_generic_dealloc,
  .tp_getset    = hpssclapipy_pio_gapinfo_getset,
  .tp_repr      = (reprfunc)hpssclapipy_pio_gapinfo_repr,
};

PyTypeObject hpssclapipy_pio_grpType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.pio_grp",
  .tp_basicsize = sizeof(hpssclapipy_pio_grp_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS PIO Group",
  .tp_dealloc   = (destructor)hpssclapipy_pio_generic_dealloc,
};

PyTypeObject hpssclapipy_pio_paramsType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.pio_params",
  .tp_basicsize = sizeof(hpssclapipy_pio_params_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS PIO Parameters",
  .tp_new       = hpssclapipy_pio_params_new,
  .tp_init      = (initproc)hpssclapipy_pio_params_init,
  .tp_dealloc   = (destructor)hpssclapipy_pio_generic_dealloc,
  .tp_getset    = hpssclapipy_pio_params_getset,
  .tp_repr      = (reprfunc)hpssclapipy_pio_params_repr,
};

/* Allocator */
static PyObject* hpssclapipy_pio_params_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  return type->tp_alloc(type, 0);
}

/* Initializer */
static int hpssclapipy_pio_params_init(hpssclapipy_pio_params_t *self, PyObject *args, PyObject *kwds) {
  memset(&self->data, 0, sizeof(self->data));
  return 0;
}

/* Deallocator - generic for all our PIO types */
static void hpssclapipy_pio_generic_dealloc(PyObject *self) {
  self->ob_type->tp_free((PyObject*)self);
}

/* Getters and Setters */
static PyObject* hpssclapipy_pio_gapinfo_get(hpssclapipy_pio_gapinfo_t *self, void *closure) {
  gapinfo_field_t field = (gapinfo_field_t)closure;

  switch(field) {
    GET_U64(Offset);
    GET_U64(Length);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_pio_params_get(hpssclapipy_pio_params_t *self, void *closure) {
  params_field_t field = (params_field_t)closure;

  switch(field) {
    GET_INT(Operation);
    GET_INT(ClntStripeWidth);
    GET_INT(BlockSize);
    GET_INT(FileStripeWidth);
    GET_INT(IOTimeOutSecs);
    GET_INT(Transport);
    GET_INT(Options);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static int hpssclapipy_pio_params_set(hpssclapipy_pio_params_t *self, PyObject *value, void *closure) {
  params_field_t field = (params_field_t)closure;

  switch(field) {
    SET_INT(Operation);
    SET_INT(ClntStripeWidth);
    SET_INT(BlockSize);
    SET_INT(FileStripeWidth);
    SET_INT(IOTimeOutSecs);
    SET_INT(Transport);
    SET_INT(Options);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return -1;
  }
}

/* Representation */
static PyObject* hpssclapipy_pio_gapinfo_repr(hpssclapipy_pio_gapinfo_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_pio_gapinfo_t {\n");
  indent += 2;

  PRINT_U64(Offset);
  PRINT_U64(Length);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

static PyObject* hpssclapipy_pio_params_repr(hpssclapipy_pio_params_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_pio_params_t {\n");
  indent += 2;

  switch(self->data.Operation) {
    PRINT_ENUM(Operation, HPSS_PIO_READ);
    PRINT_ENUM(Operation, HPSS_PIO_WRITE);
    default:
      PRINT_INT(Operation);
  }

  switch(self->data.ClntStripeWidth) {
    PRINT_ENUM(ClntStripeWidth, HPSS_PIO_STRIPE_UNKNOWN);
    default:
      PRINT_INT(ClntStripeWidth);
  }

  PRINT_INT(BlockSize);

  switch(self->data.FileStripeWidth) {
    PRINT_ENUM(FileStripeWidth, HPSS_PIO_STRIPE_UNKNOWN);
    default:
      PRINT_INT(FileStripeWidth);
  }

  PRINT_INT(IOTimeOutSecs);

  switch(self->data.Transport) {
    PRINT_ENUM(Transport, HPSS_PIO_TCPIP);
    PRINT_ENUM(Transport, HPSS_PIO_MVR_SELECT);
    default:
      PRINT_INT(Transport);
  }

  INDENT; strcat(str, "Options: ");
  if(self->data.Options == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(Options, HPSS_PIO_PUSH);
    PRINT_FLAG(Options, HPSS_PIO_HANDLE_GAP);
    PRINT_FLAG(Options, HPSS_PIO_PORT_RANGE);
    strcat(str, ",\n");
  }

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}


PyObject* hpssclapipy_PIOStart(PyObject *self, PyObject *args) {
  int rc;
  hpssclapipy_pio_params_t *params = NULL;
  hpssclapipy_pio_grp_t    *group  = NULL;

  if(!PyArg_ParseTuple(args, "O!", &hpssclapipy_pio_paramsType, &params))
    return NULL;

  group = TP_ALLOC(pio_grp);
  if(group == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_PIOStart(&params->data, &group->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(group);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)group;
}

PyObject* hpssclapipy_PIOExportGrp(PyObject *self, PyObject *args) {
  int rc;
  unsigned int buflen;
  void *buf;
  hpssclapipy_pio_grp_t *group = NULL;
  PyObject *export;

  if(!PyArg_ParseTuple(args, "O!", &hpssclapipy_pio_grpType, &group))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_PIOExportGrp(group->data, &buf, &buflen);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  export = Py_BuildValue("s#", buf, buflen);
  free(buf);
  return export;
}

PyObject* hpssclapipy_PIOExecute(PyObject *self, PyObject *args) {
  int fd, status;
  u_signed64 offset, length, bytes;
  unsigned long long off, len;
  hpssclapipy_pio_grp_t     *group   = NULL;
  hpssclapipy_pio_gapinfo_t *gapinfo = NULL;
  hpss_pio_gapinfo_t   *gap     = NULL;
  PyObject *rc;

  if(!PyArg_ParseTuple(args, "iKKO!", &fd, &off, &len,
                             &hpssclapipy_pio_grpType, &group))
    return NULL;

  gapinfo = TP_ALLOC(pio_gapinfo);
  if(gapinfo == NULL)
    return PyErr_NoMemory();
  gap = &gapinfo->data;
  memset(gap, 0, sizeof(*gap));

  offset = ULL_TO_U64(off);
  length = ULL_TO_U64(len);

  Py_BEGIN_ALLOW_THREADS
  status = hpss_PIOExecute(fd, offset, length, group->data, gap, &bytes);
  Py_END_ALLOW_THREADS

  if(status != 0 && !(status == HPSS_EOM && neqz64m(gap->Length))) {
    Py_DECREF(gapinfo);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(K,N)", U64_TO_ULL(bytes), gapinfo);
  if(rc == NULL) {
    Py_DECREF(gapinfo);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_PIOImportGrp(PyObject *self, PyObject *args) {
  int rc;
  void *buf;
  unsigned int len;
  hpssclapipy_pio_grp_t *group = NULL;

  if(!PyArg_ParseTuple(args, "s#", &buf, &len))
    return NULL;

  group = TP_ALLOC(pio_grp);
  if(group == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_PIOImportGrp(buf, len, &group->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(group);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)group;
}

typedef struct {
  PyObject   *callback;
  PyObject   *callback_arg;
  PyObject   *buf;
  unsigned32 len;
#ifdef WITH_THREAD
  // This will only be used when Python is compiled with threading support
  PyThreadState *_save;
#endif
} pio_callback_arg_t;

static int pio_read_callback(void *arg, u_signed64 offset, unsigned int *len, void **start) {
  pio_callback_arg_t *cb_arg = (pio_callback_arg_t*)arg;
  PyObject *arglist, *result, *data;
  int rc;

  /* PIO doesn't support double buffering, so we must always make a copy
   * Just stub it out for now
   */
#ifdef PIO_DOUBLE_BUFFERING
  // let's try to use our buffer object directly
  if(PyString_AS_STRING(cb_arg->buf) == *start && PyString_GET_SIZE(cb_arg->buf) == *len) {
    data = cb_arg->buf;
    Py_INCREF(data);
  }
  // otherwise we need to make a copy :'(
  else
#endif
    data = PyString_FromStringAndSize(*start, *len);

  if(data == NULL)
    return -ENOMEM;

  /* build the argument tuple
   * cb_arg->callback_arg needs a ref-count, so use O
   * data is already ref-counted, so use N, since we are giving it away
   */
  arglist = Py_BuildValue("(OKN)", cb_arg->callback_arg, U64_TO_ULL(offset), data);
  if(arglist == NULL) {
    Py_DECREF(data);
    return -ENOMEM;
  }

  /* We need to restore the interpreter thread state before proceed with
   * performing the callback, which will be run by the interpreter
   */
#ifdef WITH_THREAD
  PyEval_RestoreThread(cb_arg->_save);
#endif
  result = PyEval_CallObject(cb_arg->callback, arglist);
#ifdef WITH_THREAD
  cb_arg->_save = PyEval_SaveThread();
#endif

  /* don't need the arglist anymore
   * will decref cb_arg->callback_arg and data as necessary
   */
  Py_DECREF(arglist);

  /* get the return value from the callback
   * will return -1 if result is NULL or not an integer
   * use XDECREF in case result is NULL
   */
  rc = PyInt_AsLong(result);
  Py_XDECREF(result);

#ifdef PIO_DOUBLE_BUFFERING
  /* Since we don't have the only reference to our buffer object anymore,
   * let's make a new one so we aren't modifiying it while other threads may
   * be accessing it.
   */
  if(Py_REFCNT(cb_arg->buf) != 1) {
    Py_DECREF(cb_arg->buf); // we won't reference it anymore
    cb_arg->buf = PyString_FromStringAndSize(NULL, cb_arg->len);
    if(cb_arg->buf == NULL)
      return -1;
  }

  // update string pointer and length
  *start = PyString_AS_STRING(cb_arg->buf);
  *len   = PyString_GET_SIZE(cb_arg->buf);
#endif

  return rc;
}

static int pio_write_callback(void *arg, u_signed64 offset, unsigned int *len, void **start) {
  pio_callback_arg_t *cb_arg = (pio_callback_arg_t*)arg;
  PyObject *arglist, *result;

  // build an argument list for the callback
  arglist = Py_BuildValue("(OKO)", cb_arg->callback_arg, U64_TO_ULL(offset), Py_None);
  if(arglist == NULL)
    return -ENOMEM;

#ifdef WITH_THREAD
  PyEval_RestoreThread(cb_arg->_save);
#endif
  result = PyEval_CallObject(cb_arg->callback, arglist);
#ifdef WITH_THREAD
  cb_arg->_save = PyEval_SaveThread();
#endif

  // we don't need the arglist anymore
  Py_DECREF(arglist);

  /* get the return value from the callback
   * the callback is expected to return a buffer which is provided to PIO
   * for writing
   */
  if(!PyString_CheckExact(result))
    return -EINVAL;

  *start = PyString_AS_STRING(result);
  *len   = PyString_GET_SIZE(result);

  return 0;
}

PyObject* hpssclapipy_PIORegister(PyObject *self, PyObject *args, PyObject *keywds) {
  int rc, rw;
  unsigned32 stripe, buflen;
  void *buf;
  hpssclapipy_pio_grp_t *group = NULL;
  pio_callback_arg_t *cb_arg;
  hpssclapipy_sockaddr_t *saddr = NULL;
  hpss_sockaddr_t   *addr  = NULL;
  int (*callback)(void*,u_signed64,unsigned int*,void**);
  static char *kwlist[] = { "stripe", "size", "group", "rw",
                            "callback", "callback_arg", 
                            "sockaddr", NULL };

  cb_arg = (pio_callback_arg_t*)malloc(sizeof(*cb_arg));
  if(cb_arg == NULL)
    return PyErr_NoMemory();

  cb_arg->callback     = NULL;
  cb_arg->callback_arg = Py_None;

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iiO!iO|OO!", kwlist,
                                  &stripe, &buflen,
                                  &hpssclapipy_pio_grpType, &group, &rw,
                                  &cb_arg->callback, &cb_arg->callback_arg,
                                  &hpssclapipy_sockaddrType, &saddr))
  {
    free(cb_arg);
    return NULL;
  }

  if(rw == HPSS_PIO_READ)
    callback = pio_read_callback;
  else if(rw == HPSS_PIO_WRITE)
    callback = pio_write_callback;
  else {
    free(cb_arg);
    return PyErr_SetErrno(EINVAL);
  }

  if(!PyCallable_Check(cb_arg->callback)) {
    free(cb_arg);
    return NULL;
  }

  if(saddr != NULL)
    addr = &saddr->data;

  /* Create a new Python buffer of size 'buflen'. This will be used to
   * eradicate a copy that would otherwise happen in the callback
   */
  cb_arg->buf = PyString_FromStringAndSize(NULL, buflen);
  if(cb_arg->buf == NULL) {
    free(cb_arg);
    return PyErr_NoMemory();
  }
  buf = PyString_AS_STRING(cb_arg->buf);
  cb_arg->len = buflen;

  Py_INCREF(cb_arg->callback);
  Py_INCREF(cb_arg->callback_arg);

  /* We have to do some manual interpreter thread state saving here due to the
   * nature of using a callback
   */
#ifdef WITH_THREAD
  cb_arg->_save = PyEval_SaveThread();
#endif
  rc = hpss_PIORegister(stripe, addr, buf, buflen, group->data, callback, cb_arg);
#ifdef WITH_THREAD
  PyEval_RestoreThread(cb_arg->_save);
#endif

  Py_DECREF(cb_arg->callback);
  Py_DECREF(cb_arg->callback_arg);
  Py_XDECREF(cb_arg->buf);
  free(cb_arg);

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_PIOEnd(PyObject *self, PyObject *args) {
  int rc;
  hpssclapipy_pio_grp_t *group = NULL;

  if(!PyArg_ParseTuple(args, "O!", &hpssclapipy_pio_grpType, &group))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_PIOEnd(group->data);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}
