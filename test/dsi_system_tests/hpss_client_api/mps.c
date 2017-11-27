// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/mps.c $ $Id: mps.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <hpss_api.h>
#include "hpssclapipy.h"

PyObject* hpssclapipy_Migrate(PyObject *self, PyObject *args) {
  int fd, rc;
  unsigned32 level, flags;
  u_signed64 migrated;

  if(!PyArg_ParseTuple(args, "iII", &fd, &level, &flags))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Migrate(fd, level, flags, &migrated);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("K", U64_TO_ULL(migrated));
}

PyObject* hpssclapipy_Purge(PyObject *self, PyObject *args) {
  int fd, rc;
  u_signed64 purged, offset, length;
  unsigned32 level, flags;

  offset = cast64m(0);
  length = cast64m(0);
  flags  = BFS_PURGE_ALL;

  if(!PyArg_ParseTuple(args, "ii", &fd, &level))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Purge(fd, offset, length, level, flags, &purged);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  return Py_BuildValue("K", U64_TO_ULL(purged));
}

PyObject* hpssclapipy_PurgeLock(PyObject *self, PyObject *args) {
  int fd, rc;
  purgelock_flag_t flag;

  if(!PyArg_ParseTuple(args, "ii", &fd, &flag))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_PurgeLock(fd, flag);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Stage(PyObject *self, PyObject *args) {
  int fd, rc;
  unsigned32 level, flags;
  u_signed64 offset, length;
  unsigned long long off, len;

  if(!PyArg_ParseTuple(args, "iKKII", &fd, &off, &len, &level, &flags))
    return NULL;

  offset = ULL_TO_U64(off);
  length = ULL_TO_U64(len);

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Stage(fd, offset, length, level, flags);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}
