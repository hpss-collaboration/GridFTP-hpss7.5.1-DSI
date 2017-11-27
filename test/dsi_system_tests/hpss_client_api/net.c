// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/net.c $ $Id: net.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <structmember.h>
#include <hpss_api.h>
#include <hpss_net.h>
#include "hpssclapipy.h"

/* Deallocator */
static void hpssclapipy_sockaddr_dealloc(hpssclapipy_sockaddr_t *self);

/* Representation */
static PyObject* hpssclapipy_sockaddr_repr(hpssclapipy_sockaddr_t *self);

/* Type definition */
PyTypeObject hpssclapipy_sockaddrType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpss.sockaddr",
  .tp_basicsize = sizeof(hpssclapipy_sockaddr_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Socket Address",
  .tp_dealloc   = (destructor)hpssclapipy_sockaddr_dealloc,
  .tp_repr      = (reprfunc)hpssclapipy_sockaddr_repr,
};


/* Deallocator
 * This class can only be created by hpss.net_getaddrinfo() and friends,
 * so it has no allocator or initializer
 */
static void hpssclapipy_sockaddr_dealloc(hpssclapipy_sockaddr_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

/* Representation */
static PyObject* hpssclapipy_sockaddr_repr(hpssclapipy_sockaddr_t *self) {
  int status;
  PyObject *rc;
  char addrstr[NI_MAXHOST], errbuf[HPSS_NET_MAXBUF];

  memset(addrstr, 0, sizeof(addrstr));

  Py_BEGIN_ALLOW_THREADS
  status = hpss_net_getuniversaladdress(&self->data,
                addrstr, sizeof(addrstr),
                errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(status < 0)
    return PyErr_SetErrno(-status);

  rc = Py_BuildValue("s", addrstr);
  return rc;
}

PyObject* hpssclapipy_net_accept(PyObject *self, PyObject *args, PyObject *keywds) {
  int sockfd, status;
  hpssclapipy_sockaddr_t *addr;
  char errbuf[HPSS_NET_MAXBUF];
  PyObject *rc;

  static char *kwlist[] = { "sockfd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &sockfd))
    return NULL;

  addr = TP_ALLOC(sockaddr);
  if(addr == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  status = hpss_net_accept(sockfd, &addr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(status == -1) {
    Py_DECREF(addr);
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  rc = Py_BuildValue("(iN)", status, addr);
  if(rc == NULL) {
    Py_DECREF(addr);
    close(status);
    return PyErr_NoMemory();
  }

  return rc;
}

PyObject* hpssclapipy_net_addrmatch(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr, *entry, *mask = NULL;
  hpss_sockaddr_t   *Mask = NULL;
  int rc;

  static char *kwlist[] = { "addr", "entry", "mask", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!O!|O!", kwlist,
                                  &hpssclapipy_sockaddrType, &addr,
                                  &hpssclapipy_sockaddrType, &entry,
                                  &hpssclapipy_sockaddrType, &mask))
    return NULL;

  if(mask != NULL)
    Mask = &mask->data;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_addrmatch(&addr->data, Mask, &entry->data);
  Py_END_ALLOW_THREADS

  if(rc == 0)
    Py_RETURN_FALSE;

  Py_RETURN_TRUE;
}

PyObject* hpssclapipy_net_bind(PyObject *self, PyObject *args, PyObject *keywds) {
  int sockfd, rc;
  hpssclapipy_sockaddr_t *addr;
  char errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "sockfd", "addr", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iO!", kwlist, &sockfd,
                                  &hpssclapipy_sockaddrType, &addr))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_bind(sockfd, &addr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_net_connect(PyObject *self, PyObject *args, PyObject *keywds) {
  int sockfd, rc;
  hpssclapipy_sockaddr_t *addr;
  char errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "sockfd", "addr", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "iO!", kwlist, &sockfd,
                                  &hpssclapipy_sockaddrType, &addr))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_connect(sockfd, &addr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_net_generatemask(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr, *mask;
  int bits;

  static char *kwlist[] = { "addr", "bits", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!i", kwlist,
                                  &hpssclapipy_sockaddrType, &addr, &bits))
    return NULL;

  mask = TP_ALLOC(sockaddr);
  if(mask == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  hpss_net_generatemask(&addr->data, &mask->data, bits);
  Py_END_ALLOW_THREADS

  return (PyObject*)mask;
}

PyObject* hpssclapipy_net_get_family_option(PyObject *self) {
  char errbuf[HPSS_NET_MAXBUF];
  int  rc;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_get_family_option(errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc == HPSS_NET_UNSPECIFIED) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return Py_BuildValue("i", rc);
}

PyObject* hpssclapipy_net_get_family_string(PyObject *self) {
  char *family;

  Py_BEGIN_ALLOW_THREADS
  family = hpss_net_get_family_string();
  Py_END_ALLOW_THREADS

  if(family == NULL)
    return PyErr_SetErrno(ENOENT);

  return PyString_FromString(family);
}

PyObject* hpssclapipy_net_getaddrinfo(PyObject *self, PyObject *args, PyObject *keywds) {
  char              *hostname = NULL, *service = NULL, errbuf[HPSS_NET_MAXBUF];
  int               rc, flags = 0;
  hpss_ipproto_t    protocol = HPSS_IPPROTO_UNSPECIFIED;
  hpssclapipy_sockaddr_t *saddr;

  static char *kwlist[] = { "hostname", "service", "flags", "protocol", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|ssii", kwlist,
                                  &hostname, &service, &flags, &protocol))
    return NULL;

  saddr = TP_ALLOC(sockaddr);
  if(saddr == NULL)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getaddrinfo(hostname, service, flags, protocol,
                            &saddr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(saddr);
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return (PyObject*)saddr;
}

PyObject* hpssclapipy_net_getaddrinfo_ex(PyObject *self, PyObject *args, PyObject *keywds) {
  int               rc, flags, i, addrcnt;
  char              *hostname = NULL, *service = NULL,
                    errbuf[HPSS_NET_MAXBUF], name[NI_MAXHOST];
  hpss_sockaddr_t   *addrs;
  hpss_ipproto_t    protocol;
  PyObject          *list, *tuple;
  hpssclapipy_sockaddr_t *addr;

  static char *kwlist[] = { "hostname", "service", "flags", "protocol", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|ssii", kwlist,
                                  &hostname, &service, &flags, &protocol))
    return NULL;

  /* always get the canonical name */
  flags |= AI_CANONNAME;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getaddrinfo_ex(hostname, service, flags, protocol, &addrs, &addrcnt,
                               name, sizeof(name), errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  list = PyList_New(addrcnt);
  for(i = 0; i < addrcnt; i++) {
    addr = TP_ALLOC(sockaddr);
    if(addr == NULL) {
      free(addrs);
      Py_DECREF(list);
      return PyErr_NoMemory();
    }

    addr->data = addrs[i];
    PyList_SET_ITEM(list, i, (PyObject*)addr);
  }

  free(addrs);

  tuple = Py_BuildValue("(Ns)", list, name);
  if(tuple == NULL) {
    Py_DECREF(list);
    return PyErr_NoMemory();
  }

  return tuple;
}

PyObject* hpssclapipy_net_getinaddr(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  unsigned int      addrfamily;
  socklen_t         addrlen;
  void              *rc;

  static char *kwlist[] = { "addr", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_sockaddrType, &addr))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getinaddr(&addr->data, &addrfamily, &addrlen);
  Py_END_ALLOW_THREADS

  if(rc == NULL)
    return PyErr_SetErrno(EINVAL);

  return Py_BuildValue("(s#I)", rc, (int)addrlen, addrfamily);
}

PyObject* hpssclapipy_net_getnameinfo(PyObject *self, PyObject *args, PyObject *keywds) {
  char              host[NI_MAXHOST], service[NI_MAXSERV], errbuf[HPSS_NET_MAXBUF];
  hpssclapipy_sockaddr_t *addr;
  int               flags, rc;

  static char *kwlist[] = { "addr", "flags", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!i", kwlist,
                                  &hpssclapipy_sockaddrType, &addr, &flags))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getnameinfo(&addr->data, host, sizeof(host), service, sizeof(service),
                            flags, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return Py_BuildValue("(ss)", host, service);
}

PyObject* hpssclapipy_net_getpeername(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  int               rc, sockfd;
  char              errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "sockfd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &sockfd))
    return NULL;

  addr = TP_ALLOC(sockaddr);
  if(addr == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getpeername(sockfd, &addr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(addr);
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return (PyObject*)addr;
}

PyObject* hpssclapipy_net_getport(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  int               rc;
  char              errbuf[HPSS_NET_MAXBUF];
  
  static char *kwlist[] = { "addr", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_sockaddrType, &addr))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getport(&addr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc < 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return Py_BuildValue("H", (unsigned short)rc);
}

PyObject* hpssclapipy_net_getsockname(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  int               rc, sockfd;
  char              errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "sockfd", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "i", kwlist, &sockfd))
    return NULL;

  addr = TP_ALLOC(sockaddr);
  if(addr == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getsockname(sockfd, &addr->data, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(addr);
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return (PyObject*)addr;
}

PyObject* hpssclapipy_net_getuniversaladdress(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  int               rc;
  char              buf[NI_MAXHOST], errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "addr", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!", kwlist,
                                  &hpssclapipy_sockaddrType, &addr))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_getuniversaladdress(&addr->data, buf, sizeof(buf), errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc < 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return PyString_FromStringAndSize(buf, rc);
}

PyObject* hpssclapipy_net_listen(PyObject *self, PyObject *args, PyObject *keywds) {
  int sockfd, backlog, rc;
  char errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "sockfd", "backlog", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "ii", kwlist, &sockfd, &backlog))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_listen(sockfd, backlog, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_net_setport(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  unsigned short    port;
  int               rc;
  char              errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "addr", "port", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!H", kwlist,
                                  &hpssclapipy_sockaddrType, &addr, &port))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_setport(&addr->data, port, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_net_socket(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_sockaddr_t *addr;
  int               type, protocol, rc;
  char              errbuf[HPSS_NET_MAXBUF];

  static char *kwlist[] = { "addr", "type", "protocol", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!ii", kwlist,
                                  &hpssclapipy_sockaddrType, &addr,
                                  &type, &protocol))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_socket(&addr->data, type, protocol, errbuf, sizeof(errbuf));
  Py_END_ALLOW_THREADS

  if(rc < 0) {
    PyErr_SetString(PyExc_IOError, errbuf);
    return NULL;
  }

  return Py_BuildValue("i", rc);
}

PyObject* hpssclapipy_net_universaladdresstoport(PyObject *self, PyObject *args, PyObject *keywds) {
  char *address;
  int  rc;

  static char *kwlist[] = { "address", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &address))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_net_universaladdresstoport(address);
  Py_END_ALLOW_THREADS

  return Py_BuildValue("H", (unsigned short)rc);
}
