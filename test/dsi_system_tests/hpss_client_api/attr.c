// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/attr.c $ $Id: attr.c 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#include <Python.h>
#include <structmember.h>
#include <hpss_api.h>
#include <hpss_String.h>
#include <time.h>
#include <toolslib.h>
#include "hpssclapipy.h"

/* Allocators */
static PyObject* hpssclapipy_vattr_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* Initializers */
static int hpssclapipy_vattr_init(hpssclapipy_cos_hints_t *self, PyObject *args, PyObject *kwds);

/* Deallocators */
static void hpssclapipy_stat_dealloc    (hpssclapipy_stat_t     *self);
static void hpssclapipy_statfs_dealloc  (hpssclapipy_statfs_t   *self);
static void hpssclapipy_statvfs_dealloc (hpssclapipy_statvfs_t  *self);
static void hpssclapipy_vattr_dealloc   (hpssclapipy_vattr_t    *self);
static void hpssclapipy_fileattr_dealloc(hpssclapipy_fileattr_t *self);
static void hpssclapipy_xfileattr_dealloc(hpssclapipy_xfileattr_t *self);
static void hpssclapipy_bf_sc_attrib_dealloc(hpssclapipy_bf_sc_attrib_t *self);

static void hpssclapipy_Attrs_dealloc   (hpssclapipy_Attrs_t    *self);

/* Getters */
static PyObject* hpssclapipy_stat_get    (hpssclapipy_stat_t     *self, void *closure);
static PyObject* hpssclapipy_statfs_get  (hpssclapipy_statfs_t   *self, void *closure);
static PyObject* hpssclapipy_statvfs_get (hpssclapipy_statvfs_t  *self, void *closure);
static PyObject* hpssclapipy_vattr_get   (hpssclapipy_vattr_t    *self, void *closure);
static PyObject* hpssclapipy_fileattr_get(hpssclapipy_fileattr_t *self, void *closure);
static PyObject* hpssclapipy_xfileattr_get(hpssclapipy_xfileattr_t *self, void *closure);
static PyObject* hpssclapipy_bf_sc_attrib_get(hpssclapipy_bf_sc_attrib_t *self, void *closure);
static PyObject* hpssclapipy_Attrs_get   (hpssclapipy_Attrs_t    *self, void *closure);

/* Setters */
static int hpssclapipy_vattr_set(hpssclapipy_vattr_t *self, PyObject *value, void *closure);

/* Representation */
static PyObject* hpssclapipy_stat_repr    (hpssclapipy_stat_t     *self);
static PyObject* hpssclapipy_statfs_repr  (hpssclapipy_statfs_t   *self);
static PyObject* hpssclapipy_statvfs_repr (hpssclapipy_statvfs_t  *self);
static PyObject* hpssclapipy_vattr_repr   (hpssclapipy_vattr_t    *self);
static PyObject* hpssclapipy_fileattr_repr(hpssclapipy_fileattr_t *self);
static PyObject* hpssclapipy_xfileattr_repr(hpssclapipy_xfileattr_t *self);
static PyObject* hpssclapipy_bf_sc_attrib_repr(hpssclapipy_bf_sc_attrib_t *self);
static PyObject* hpssclapipy_Attrs_repr   (hpssclapipy_Attrs_t    *self);

typedef enum {
  FIELD_st_dev,
  FIELD_st_ino,
  FIELD_st_nlink,
  FIELD_st_flag,
  FIELD_st_uid,
  FIELD_st_gid,
  FIELD_st_rdev,
  FIELD_st_ssize,
  FIELD_hpss_st_atime,
  FIELD_st_atime_n,
  FIELD_hpss_st_mtime,
  FIELD_st_mtime_n,
  FIELD_hpss_st_ctime,
  FIELD_st_ctime_n,
  FIELD_st_blksize,
  FIELD_st_blocks,
  FIELD_st_vfstype,
  FIELD_st_vfs,
  FIELD_st_type,
  FIELD_st_gen,
  FIELD_st_size,
  FIELD_st_mode,
} stat_field_t;

typedef enum {
  FIELD_f_bsize,
  FIELD_f_blocks,
  FIELD_f_bfree,
  FIELD_f_bavail,
  FIELD_f_files,
  FIELD_f_ffree,
  FIELD_f_fsid,
  FIELD_f_namemax,
  FIELD_f_fname,
  FIELD_f_fstr,
  FIELD_f_fpack,
} statfs_field_t;

typedef enum {
  FIELD_va_type,
  FIELD_va_mode,
  FIELD_va_uid,
  FIELD_va_gid,
  FIELD_va_fsid,
  FIELD_va_serialno,
  FIELD_va_nlink,
  FIELD_va_rdev,
  FIELD_va_nid,
  FIELD_va_chan,
  FIELD_va_aclsize,
  FIELD_va_size,
  FIELD_va_prefiosize,
  FIELD_va_blocksize,
  FIELD_va_numblocks,
  FIELD_va_ftid,
  FIELD_va_atime,
  FIELD_va_mtime,
  FIELD_va_ctime,
  FIELD_va_acl,
  FIELD_va_realm,
  FIELD_va_objhandle,
  FIELD_va_soid,
  FIELD_va_cos,
  FIELD_va_account,
} vattr_field_t;

typedef enum {
  FIELD_ObjectHandle,
  FIELD_Attrs,
} fileattr_field_t;

typedef enum {
  FIELD_XAttr_ObjectHandle,
  FIELD_XAttr_Attrs,
  FIELD_XAttr_SCAttrib
} xfileattr_field_t;

typedef enum {
  FIELD_NumberOfVVs,
  FIELD_BytesAtLevel,
  FIELD_OptimumAccessSize,
} bf_sc_attrib_field_t;

typedef enum {
  FIELD_Account,
#ifdef HPSS751
  FIELD_BitfileObj,
#else
  FIELD_BitfileId,
  FIELD_RealmId,  
#endif
  FIELD_Comment,
  FIELD_CompositePerms,
  FIELD_COSId,
  FIELD_DataLength,
  FIELD_EntryCount,
  FIELD_ExtendedACLs,
  FIELD_FamilyId,
  FIELD_FilesetHandle,
  FIELD_FilesetId,
#ifdef HPSS751
  FIELD_FilesetRootObjectId,
#else
  FIELD_FilesetRootId,
#endif
  FIELD_FilesetStateFlags,
  FIELD_FilesetType,
  FIELD_GID,
  FIELD_GroupPerms,
  FIELD_LinkCount,
  FIELD_ModePerms,
  FIELD_OpenCount,
  FIELD_OptionFlags,
  FIELD_OtherPerms,
  FIELD_ReadCount,
  FIELD_RegisterBitMap,
  FIELD_SubSystemId,
  FIELD_TimeCreated,
  FIELD_TimeLastRead,
  FIELD_TimeLastWritten,
  FIELD_TimeModified,
  // TODO: Add TrashInfo field
  FIELD_Type,
  FIELD_UID,
  FIELD_UserPerms,
  FIELD_WriteCount,
} Attrs_field_t;

static PyGetSetDef hpssclapipy_stat_getset[] = {
#define STAT(x, doc) \
  { #x, (getter)hpssclapipy_stat_get, (setter)NULL, doc, (void*)FIELD_##x, }
  STAT(st_dev,         "Identifier of device containing directory entry"),
  STAT(st_ino,         "File serial number"),
  STAT(st_nlink,       "Number of links"),
  STAT(st_flag,        "Flag word"),
  STAT(st_uid,         "User ID"),
  STAT(st_gid,         "Group ID"),
  STAT(st_rdev,        "Identifier of a character or block special file"),
  STAT(st_ssize,       "File size"),
  STAT(hpss_st_atime,  "Last access time"),
  STAT(st_atime_n,     "Not used"),
  STAT(hpss_st_mtime,  "Last modification time"),
  STAT(st_mtime_n,     "Not used"),
  STAT(hpss_st_ctime,  "Last status change"),
  STAT(st_ctime_n,     "Not used"),
  STAT(st_blksize,     "Optimal block size for I/O"),
  STAT(st_blocks,      "Number of blocks allocated"),
  STAT(st_vfstype,     "Not used"),
  STAT(st_vfs,         "Not used"),
  STAT(st_type,        "Not used"),
  STAT(st_gen,         "Generation number"),
  STAT(st_size,        "File size"),
  STAT(st_mode,        "File mode"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_statfs_getset[] = {
#define STATFS(x, doc) \
  { #x, (getter)hpssclapipy_statfs_get, (setter)NULL, doc, (void*)FIELD_##x, }
  STATFS(f_bsize,   "Preferred file system block size"),
  STATFS(f_blocks,  "Total data blocks in file system"),
  STATFS(f_bfree,   "Free blocks in file system"),
  STATFS(f_bavail,  "Free blocks available to non-superuser"),
  STATFS(f_files,   "Total number of file nodes in file system"),
  STATFS(f_ffree,   "Free file nodes in file system"),
  STATFS(f_fsid,    "File system ID"),
  STATFS(f_namemax, "Maximum filename length"),
  STATFS(f_fname,   "File system name"),
  STATFS(f_fpack,   "File system pack name"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_statvfs_getset[] = {
#define STATVFS(x, doc) \
  { #x, (getter)hpssclapipy_statvfs_get, (setter)NULL, doc, (void*)FIELD_##x, }
  STATVFS(f_bsize,   "Preferred file system block size"),
  STATVFS(f_blocks,  "Total data blocks in file system"),
  STATVFS(f_bfree,   "Free blocks in file system"),
  STATVFS(f_bavail,  "Free blocks available to non-superuser"),
  STATVFS(f_files,   "Total number of file nodes in file system"),
  STATVFS(f_ffree,   "Free file nodes in file system"),
  STATVFS(f_fsid,    "File system ID"),
  STATVFS(f_namemax, "Maximum filename length"),
  STATVFS(f_fstr,    "File system name"),
  STATVFS(f_fpack,   "File system pack name"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_vattr_getset[] = {
#define VATTR(x, doc) \
  { #x, (getter)hpssclapipy_vattr_get, (setter)hpssclapipy_vattr_set, doc, (void*)FIELD_##x, }
  VATTR(va_type,       "HPSS object type\n"
                       "  F_VNON\n"
                       "  F_VREG\n"
                       "  F_VDIR\n"
                       "  F_VBLK\n"
                       "  F_VCHR\n"
                       "  F_VLNK\n"
                       "  F_VSOCK\n"
                       "  F_VBAD\n"
                       "  F_VFIFO\n"
                       "  F_VMPC\n"
  ),
  VATTR(va_mode,       "Unix permissions"),
  VATTR(va_uid,        "Unix user owner"),
  VATTR(va_gid,        "Unix group owner"),
  VATTR(va_fsid,       "Not used"),
  VATTR(va_serialno,   "Not used"),
  VATTR(va_nlink,      "Number of links"),
  VATTR(va_rdev,       "Not used"),
  VATTR(va_nid,        "Network ID"),
  VATTR(va_chan,       "Channel for object device"),
  VATTR(va_aclsize,    "ACL size"),
  VATTR(va_size,       "File size"),
  VATTR(va_prefiosize, "Preferred I/O size"),
  VATTR(va_blocksize,  "Block size"),
  VATTR(va_numblocks,  "Number of blocks"),
  VATTR(va_ftid,       "Fileset ID"),
  VATTR(va_atime,      "Last access time"),
  VATTR(va_mtime,      "Last modification time"),
  VATTR(va_ctime,      "Last status change time"),
  VATTR(va_acl,        "Access Control List"),
  VATTR(va_realm,      "Realm ID"),
  VATTR(va_objhandle,  "NS Object Handle"),
  VATTR(va_soid,       "Bitfile ID"),
  VATTR(va_cos,        "Class of Service ID"),
  VATTR(va_account,    "Account index"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_fileattr_getset[] = {
#define FILEATTR(x, doc) \
  { #x, (getter)hpssclapipy_fileattr_get, (setter)NULL, doc, (void*)FIELD_##x, }
  FILEATTR(ObjectHandle, "NS Object Handle"),
  FILEATTR(Attrs,        "File attributes managed by Core Server"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_xfileattr_getset[] = {
#define XFILEATTR(x, doc) \
  { #x, (getter)hpssclapipy_xfileattr_get, (setter)NULL, \
      doc, (void*)FIELD_##x, }
  XFILEATTR(XAttr_ObjectHandle, "NS Object Handle"),
  XFILEATTR(XAttr_Attrs,        "File attributes managed by Core Server"),
  XFILEATTR(XAttr_SCAttrib,     "Storage Class attributes managed by Core Server"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_bf_sc_attrib_getset[] = {
#define SCATTR(x, doc) \
  { #x, (getter)hpssclapipy_bf_sc_attrib_get, (setter)NULL, \
      doc, (void*)FIELD_##x, }
  SCATTR(NumberOfVVs,        "Number of VVs"),
  SCATTR(BytesAtLevel,       "Bytes at storage level"),
  SCATTR(OptimumAccessSize,  "Optimum access size"),

  { NULL, NULL, NULL, NULL, NULL, },
};

static PyGetSetDef hpssclapipy_Attrs_getset[] = {
#define ATTRS(x, doc) \
  { #x, (getter)hpssclapipy_Attrs_get, (setter)NULL, doc, (void*)FIELD_##x, }
  ATTRS(Account,           "Opaque accounting information"),
#ifdef HPSS751
  ATTRS(BitfileObj,        "Bitfile Identifier"),
#else
  ATTRS(BitfileId,         "Bitfile Identifier"),
  ATTRS(RealmId,           "Realm Identifier"),
#endif
  ATTRS(Comment,           "Uninterpreted client-supplied ASCII text"),
  ATTRS(CompositePerms,    "Permission after all ACLs have been examined and applied"),
  ATTRS(COSId,             "Class of Service ID"),
  ATTRS(DataLength,        "File size"),
  ATTRS(EntryCount,        "Number of entries in a directory"),
  ATTRS(ExtendedACLs,      "Whether object has extended ACLs"),
  ATTRS(FamilyId,          "Fileset Family ID"),
  ATTRS(FilesetHandle,     "ns_ObjHandle for root node of a fileset"),
  ATTRS(FilesetId,         "Fileset ID"),
#ifdef HPSS751
  ATTRS(FilesetRootObjectId, "Root ID of fileset"),
#else
  ATTRS(FilesetRootId,     "Root ID of fileset"),
#endif
  ATTRS(FilesetStateFlags, "State of the fileset\n"
                           "Bitwise-or of the following:\n"
                           "  CORE_FS_STATE_READ\n"
                           "  CORE_FS_STATE_WRITE\n"
                           "  CORE_FS_STATE_DESTROYED\n"
  ),
  ATTRS(FilesetType,       "Type of the fileset:\n"
                           "  CORE_FS_TYPE_HPSS_ONLY\n"
                           "  CORE_FS_TYPE_ARCHIVED\n"
                           "  CORE_FS_TYPE_NATIVE\n"
                           "  CORE_FS_TYPE_MIRRORED\n"
  ),
  ATTRS(GID,               "Principal Group ID"),
  ATTRS(GroupPerms,        "Permissions granted to group members"),
  ATTRS(LinkCount,         "Number of links"),
  ATTRS(ModePerms,         "SetUID, SetGID, SetSticky options"),
  ATTRS(OpenCount,         "Number of opens"),
  ATTRS(OptionFlags,       "File Options\n"
                           "Bitwise-or of the following:\n"
                           "  CORE_OPTION_FLAG_DONT_PURGE"
  ),
  ATTRS(OtherPerms,        "Permissions granted to others"),
  ATTRS(ReadCount,         "Number of times read"),
#ifndef HPSS751
  ATTRS(RealmId,           "Realm Identifier"),
#endif
  ATTRS(RegisterBitMap,    "Flags used to indicate which fields in the attribute structure\n"
                           "should trigger SSM notifications when changed\n"
                           "Bitwise-or of the following:\n"
                           "  CORE_ATTR_ACCOUNT\n"
#ifdef HPSS751
                           "  CORE_ATTR_BITFILE_VALIDHASH\n"
                           "  CORE_ATTR_BITFILE_HASH\n"
                           "  CORE_ATTR_BITFILE_ID\n"
#else
                           "  CORE_ATTR_BIT_FILE_ID\n"
                           "  CORE_ATTR_REALM_ID\n"
#endif
                           "  CORE_ATTR_COMMENT\n"
                           "  CORE_ATTR_COMPOSITE_PERMS\n"
                           "  CORE_ATTR_COS_ID\n"
                           "  CORE_ATTR_DATA_LENGTH\n"
                           "  CORE_ATTR_ENTRY_COUNT\n"
                           "  CORE_ATTR_EXTENDED_ACLS\n"
                           "  CORE_ATTR_FAMILY_ID\n"
                           "  CORE_ATTR_FILESET_HANDLE\n"
                           "  CORE_ATTR_FILESET_ID\n"
#ifdef HPSS751                           
                           "  CORE_ATTR_FILESET_ROOT_OBJECT_ID\n"

#else
                           "  CORE_ATTR_FILESET_ROOT_ID\n"									
#endif
                           "  CORE_ATTR_FILESET_STATE_FLAGS\n"
                           "  CORE_ATTR_FILESET_TYPE\n"
                           "  CORE_ATTR_GID\n"
                           "  CORE_ATTR_GROUP_PERMS\n"
                           "  CORE_ATTR_LINK_COUNT\n"
                           "  CORE_ATTR_MODE_PERMS\n"
                           "  CORE_ATTR_OPEN_COUNT\n"
                           "  CORE_ATTR_OPTION_FLAGS\n"
                           "  CORE_ATTR_OTHER_PERMS\n"
                           "  CORE_ATTR_READ_COUNT\n"
#ifdef HPSS751
									"  CORE_ATTR_REALM_ID\n"
#endif
                           "  CORE_ATTR_REGISTER_BITMAP\n"
                           "  CORE_ATTR_SUB_SYSTEM_ID\n"
                           "  CORE_ATTR_TIME_CREATED\n"
                           "  CORE_ATTR_TIME_LAST_READ\n"
                           "  CORE_ATTR_TIME_LAST_WRITTEN\n"
                           "  CORE_ATTR_TIME_MODIFIED\n"
#ifdef HPSS751                           
                           "  CORE_ATTR_TRASH_INFO\n"
#endif                           
                           "  CORE_ATTR_TYPE\n"
                           "  CORE_ATTR_UID\n"
                           "  CORE_ATTR_USER_PERMS\n"
                           "  CORE_ATTR_WRITE_COUNT\n"
  ),
  ATTRS(SubSystemId,       "Subsystem ID"),
  ATTRS(TimeCreated,       "Creation time"),
  ATTRS(TimeLastRead,      "Last read time"),
  ATTRS(TimeLastWritten,   "Last write time"),
  ATTRS(TimeModified,      "Last modification time"),
  // TODO: Add TrashInfo attribute
  ATTRS(Type,              "Type of the object:\n"
                           "  NS_OBJECT_TYPE_DIRECTORY\n"
                           "  NS_OBJECT_TYPE_FILE\n"
                           "  NS_OBJECT_TYPE_JUNCTION\n"
                           "  NS_OBJECT_TYPE_SYM_LINK\n"
                           "  NS_OBJECT_TYPE_HARD_LINK\n"
  ),
  ATTRS(UID,               "Principal Owner ID"),
  ATTRS(UserPerms,         "Permission granted to owner"),
  ATTRS(WriteCount,        "Number of writes"),

  { NULL, NULL, NULL, NULL, NULL, },
};

/* Type definitions */
PyTypeObject hpssclapipy_statType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.stat",
  .tp_basicsize = sizeof(hpssclapipy_stat_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS File Statistics",
  .tp_dealloc   = (destructor)hpssclapipy_stat_dealloc,
  .tp_getset    = hpssclapipy_stat_getset,
  .tp_repr      = (reprfunc)hpssclapipy_stat_repr,
};

PyTypeObject hpssclapipy_statfsType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.statfs",
  .tp_basicsize = sizeof(hpssclapipy_statfs_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS File System Statistics",
  .tp_dealloc   = (destructor)hpssclapipy_statfs_dealloc,
  .tp_getset    = hpssclapipy_statfs_getset,
  .tp_repr      = (reprfunc)hpssclapipy_statfs_repr,
};

PyTypeObject hpssclapipy_statvfsType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.statvfs",
  .tp_basicsize = sizeof(hpssclapipy_statvfs_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS File System Statistics",
  .tp_dealloc   = (destructor)hpssclapipy_statvfs_dealloc,
  .tp_getset    = hpssclapipy_statvfs_getset,
  .tp_repr      = (reprfunc)hpssclapipy_statvfs_repr,
};

PyTypeObject hpssclapipy_vattrType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.vattr",
  .tp_basicsize = sizeof(hpssclapipy_vattr_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Name Space Attributes",
  .tp_new       = hpssclapipy_vattr_new,
  .tp_init      = (initproc)hpssclapipy_vattr_init,
  .tp_dealloc   = (destructor)hpssclapipy_vattr_dealloc,
  .tp_getset    = hpssclapipy_vattr_getset,
  .tp_repr      = (reprfunc)hpssclapipy_vattr_repr,
};

PyTypeObject hpssclapipy_fileattrType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.fileattr",
  .tp_basicsize = sizeof(hpssclapipy_fileattr_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS File Attributes",
  .tp_dealloc   = (destructor)hpssclapipy_fileattr_dealloc,
  .tp_getset    = hpssclapipy_fileattr_getset,
  .tp_repr      = (reprfunc)hpssclapipy_fileattr_repr,
};

PyTypeObject hpssclapipy_xfileattrType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.xfileattr",
  .tp_basicsize = sizeof(hpssclapipy_xfileattr_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Extended File Attributes",
  .tp_dealloc   = (destructor)hpssclapipy_xfileattr_dealloc,
  .tp_getset    = hpssclapipy_xfileattr_getset,
  .tp_repr      = (reprfunc)hpssclapipy_xfileattr_repr,
};

PyTypeObject hpssclapipy_AttrsType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.Attrs",
  .tp_basicsize = sizeof(hpssclapipy_Attrs_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Core Server Attributes",
  .tp_dealloc   = (destructor)hpssclapipy_Attrs_dealloc,
  .tp_getset    = hpssclapipy_Attrs_getset,
  .tp_repr      = (reprfunc)hpssclapipy_Attrs_repr,
};

PyTypeObject hpssclapipy_bf_sc_attribType = {
  PyObject_HEAD_INIT(&PyType_Type)
  .ob_size      = 0,
  .tp_name      = "hpssclapi.bf_sc_attrib",
  .tp_basicsize = sizeof(hpssclapipy_bf_sc_attrib_t),
  .tp_flags     = Py_TPFLAGS_DEFAULT,
  .tp_doc       = "HPSS Storage Class Attributes",
  .tp_dealloc   = (destructor)hpssclapipy_bf_sc_attrib_dealloc,
  .tp_getset    = hpssclapipy_bf_sc_attrib_getset,
  .tp_repr      = (reprfunc)hpssclapipy_bf_sc_attrib_repr,
};

/* Allocators */
static PyObject* hpssclapipy_vattr_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  return type->tp_alloc(type, 0);
}

/* Initializers */
static int hpssclapipy_vattr_init(hpssclapipy_cos_hints_t *self, PyObject *args, PyObject *kwds) {
  memset(&self->data, 0, sizeof(self->data));
  return 0;
}

/* Deallocators */
static void hpssclapipy_stat_dealloc(hpssclapipy_stat_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}
static void hpssclapipy_statfs_dealloc(hpssclapipy_statfs_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

static void hpssclapipy_statvfs_dealloc(hpssclapipy_statvfs_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

static void hpssclapipy_vattr_dealloc(hpssclapipy_vattr_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

static void hpssclapipy_fileattr_dealloc(hpssclapipy_fileattr_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

static void hpssclapipy_xfileattr_dealloc(hpssclapipy_xfileattr_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

static void hpssclapipy_bf_sc_attrib_dealloc(hpssclapipy_bf_sc_attrib_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}
static void hpssclapipy_Attrs_dealloc(hpssclapipy_Attrs_t *self) {
  self->ob_type->tp_free((PyObject*)self);
}

/* Getters */
static PyObject* hpssclapipy_stat_get(hpssclapipy_stat_t *self, void *closure) {
  stat_field_t field = (stat_field_t)closure;

  switch(field) {
    GET_INT(st_dev);
    GET_U64(st_ino);
    GET_INT(st_nlink);
    GET_INT(st_flag);
    GET_INT(st_uid);
    GET_INT(st_gid);
    GET_INT(st_rdev);
    GET_U64(st_ssize);
    GET_INT(hpss_st_atime);
    GET_INT(hpss_st_mtime);
    GET_INT(hpss_st_ctime);
    GET_INT(st_blksize);
    GET_INT(st_blocks);
    GET_INT(st_vfstype);
    GET_INT(st_vfs);
    GET_INT(st_type);
    GET_INT(st_gen);
    GET_U64(st_size);
    GET_INT(st_mode);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_statfs_get(hpssclapipy_statfs_t *self, void *closure) {
  statfs_field_t field = (statfs_field_t)closure;

  switch(field) {
    GET_U64(f_bsize);
    GET_U32(f_blocks);
    GET_U32(f_bfree);
    GET_U32(f_bavail);
    GET_U32(f_files);
    GET_U32(f_ffree);
    GET_U32(f_fsid);
    GET_U32(f_namemax);
    GET_STR(f_fname);
    GET_STR(f_fpack);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_statvfs_get(hpssclapipy_statvfs_t *self, void *closure) {
  statfs_field_t field = (statfs_field_t)closure;

  switch(field) {
    GET_U64(f_bsize);
    GET_U32(f_blocks);
    GET_U32(f_bfree);
    GET_U32(f_bavail);
    GET_U32(f_files);
    GET_U32(f_ffree);
    GET_U32(f_fsid);
    GET_U32(f_namemax);
    GET_STR(f_fstr);
    GET_STR(f_fpack);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_vattr_get(hpssclapipy_vattr_t *self, void *closure) {
  vattr_field_t field = (vattr_field_t)closure;

  switch(field) {
    GET_INT(va_type);
    GET_INT(va_mode);
    GET_INT(va_uid);
    GET_INT(va_gid);
    GET_INT(va_fsid);
    GET_INT(va_serialno);
    GET_INT(va_nlink);
    GET_INT(va_rdev);
    GET_INT(va_nid);
    GET_INT(va_chan);
    GET_INT(va_aclsize);
    GET_U64(va_size);
    GET_U64(va_prefiosize);
    GET_U64(va_blocksize);
    GET_U64(va_numblocks);
    GET_U64(va_ftid);
    GET_INT(va_atime);
    GET_INT(va_mtime);
    GET_INT(va_ctime);
    GET_STR(va_acl);
    GET_INT(va_realm);
    GET_OBJ(va_objhandle, ns_ObjHandle);

    case FIELD_va_soid:
      return NULL;

    GET_INT(va_cos);
    GET_INT(va_account);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_fileattr_get(hpssclapipy_fileattr_t *self, void *closure) {
  fileattr_field_t field = (fileattr_field_t)closure;

  switch(field) {
    GET_OBJ(ObjectHandle, ns_ObjHandle);
    GET_OBJ(Attrs,        Attrs);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_xfileattr_get(hpssclapipy_xfileattr_t *self, void *closure) {
  xfileattr_field_t field = (xfileattr_field_t)closure;

  switch(field) {
    GET_OBJ(ObjectHandle, ns_ObjHandle);
    GET_OBJ(Attrs,        Attrs);
//    GET_OBJ_LIST(SCAttrib,     SCAttrib, )

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static PyObject* hpssclapipy_bf_sc_attrib_get(hpssclapipy_bf_sc_attrib_t *self, void *closure) {
   /*
  xfileattr_field_t field = (xfileattr_field_t)closure;

  switch(field) {
    GET_OBJ(ObjectHandle, ns_ObjHandle);
    GET_OBJ(Attrs,        Attrs);
    GET_OBJ_LIST(SCAttrib,     SCAttrib)

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
  */

   /* TODO: complete this function implementation */
   return NULL;
}

static PyObject* hpssclapipy_Attrs_get(hpssclapipy_Attrs_t *self, void *closure) {
  Attrs_field_t field = (Attrs_field_t)closure;

  switch(field) {
    GET_INT(Account);
#ifdef HPSS751
	 case FIELD_BitfileObj:
#else	
    case FIELD_BitfileId:
#endif    
      return NULL;

    GET_STR(Comment);
    GET_INT(CompositePerms);
    GET_INT(COSId);
    GET_U64(DataLength);
    GET_INT(EntryCount);
    GET_INT(ExtendedACLs);
    GET_INT(FamilyId);
    GET_OBJ(FilesetHandle, ns_ObjHandle);
    GET_U64(FilesetId);
#ifdef HPSS751
    GET_U64(FilesetRootObjectId);
#else    
    GET_U64(FilesetRootId);
#endif    
    GET_INT(FilesetStateFlags);
    GET_INT(FilesetType);
    GET_INT(GID);
    GET_INT(GroupPerms);
    GET_INT(LinkCount);
    GET_INT(ModePerms);
    GET_INT(OpenCount);
    GET_INT(OptionFlags);
    GET_INT(OtherPerms);
    GET_INT(ReadCount);
#ifndef HPSS751
    GET_INT(RealmId);
#endif
    GET_U64(RegisterBitMap);
    GET_INT(SubSystemId);
    GET_INT(TimeCreated);
    GET_INT(TimeLastRead);
    GET_INT(TimeLastWritten);
    GET_INT(TimeModified);
    // TODO: Get TrashInfo
    GET_INT(Type);
    GET_INT(UID);
    GET_INT(UserPerms);
    GET_INT(WriteCount);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return NULL;
  }
}

static int hpssclapipy_vattr_set(hpssclapipy_vattr_t *self, PyObject *value, void *closure) {
  vattr_field_t field = (vattr_field_t)closure;

  if(value == NULL) {
    PyErr_SetString(PyExc_AttributeError, "Cannot delete this attribute");
    return -1;
  }

  switch(field) {
    SET_INT(va_type);
    SET_INT(va_mode);
    SET_INT(va_uid);
    SET_INT(va_gid);
    SET_INT(va_fsid);
    SET_INT(va_serialno);
    SET_INT(va_nlink);
    SET_INT(va_rdev);
    SET_INT(va_nid);
    SET_INT(va_chan);
    SET_INT(va_aclsize);
    SET_U64(va_size);
    SET_U64(va_prefiosize);
    SET_U64(va_blocksize);
    SET_U64(va_numblocks);
    SET_U64(va_ftid);
    SET_INT(va_atime);
    SET_INT(va_mtime);
    SET_INT(va_ctime);
    SET_STR(va_acl);
    SET_INT(va_realm);
    SET_OBJ(va_objhandle, ns_ObjHandle);

    case FIELD_va_soid:
      return -1;

    SET_INT(va_cos);
    SET_INT(va_account);

    default:
      PyErr_SetString(PyExc_ValueError, "This shouldn't happen");
      return -1;
  }
}

/* Representation */
static PyObject* hpssclapipy_stat_repr(hpssclapipy_stat_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_stat_t {\n");
  indent += 2;

  PRINT_INT(st_dev);
  PRINT_U64(st_ino);
  PRINT_INT(st_nlink);
  PRINT_INT(st_flag);
  PRINT_INT(st_uid);
  PRINT_INT(st_gid);
  PRINT_INT(st_rdev);
  PRINT_U64(st_ssize);
  PRINT_TIME(hpss_st_atime);
  PRINT_TIME(hpss_st_mtime);
  PRINT_TIME(hpss_st_ctime);
  PRINT_INT(st_blksize);
  PRINT_INT(st_blocks);
  PRINT_INT(st_vfstype);
  PRINT_INT(st_vfs);
  PRINT_INT(st_type);
  PRINT_INT(st_gen);
  PRINT_U64(st_size);
  PRINT_INT(st_mode);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

static PyObject* hpssclapipy_statfs_repr(hpssclapipy_statfs_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_statfs_t {\n");
  indent += 2;

  PRINT_U64(f_bsize);
  PRINT_U32(f_blocks);
  PRINT_U32(f_bfree);
  PRINT_U32(f_bavail);
  PRINT_U32(f_files);
  PRINT_U32(f_ffree);
  PRINT_U32(f_fsid);
  PRINT_U32(f_namemax);
  PRINT_STR(f_fname);
  PRINT_STR(f_fpack);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

static PyObject* hpssclapipy_statvfs_repr(hpssclapipy_statvfs_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_statvfs_t {\n");
  indent += 2;

  PRINT_U64(f_bsize);
  PRINT_U32(f_blocks);
  PRINT_U32(f_bfree);
  PRINT_U32(f_bavail);
  PRINT_U32(f_files);
  PRINT_U32(f_ffree);
  PRINT_U32(f_fsid);
  PRINT_U32(f_namemax);
  PRINT_STR(f_fstr);
  PRINT_STR(f_fpack);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

static PyObject* hpssclapipy_vattr_repr(hpssclapipy_vattr_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_vattr_t {\n");
  indent += 2;

  switch(self->data.va_type) {
    PRINT_ENUM(va_type, F_VNON);
    PRINT_ENUM(va_type, F_VREG);
    PRINT_ENUM(va_type, F_VDIR);
    PRINT_ENUM(va_type, F_VBLK);
    PRINT_ENUM(va_type, F_VCHR);
    PRINT_ENUM(va_type, F_VLNK);
    PRINT_ENUM(va_type, F_VSOCK);
    PRINT_ENUM(va_type, F_VBAD);
    PRINT_ENUM(va_type, F_VFIFO);
    PRINT_ENUM(va_type, F_VMPC);
    default:
      PRINT_INT(va_type);
  }

  PRINT_INT(va_mode);
  PRINT_INT(va_uid);
  PRINT_INT(va_gid);
  PRINT_INT(va_fsid);
  PRINT_INT(va_serialno);
  PRINT_INT(va_nlink);
  PRINT_INT(va_rdev);
  PRINT_INT(va_nid);
  PRINT_INT(va_chan);
  PRINT_INT(va_aclsize);
  PRINT_U64(va_size);
  PRINT_U64(va_prefiosize);
  PRINT_U64(va_blocksize);
  PRINT_U64(va_numblocks);
  PRINT_U64(va_ftid);
  PRINT_TIME(va_atime);
  PRINT_TIME(va_mtime);
  PRINT_TIME(va_ctime);
  PRINT_STR(va_acl);
  PRINT_INT(va_realm);
  PRINT_OBJ(va_objhandle);

  // va_soid

  PRINT_INT(va_cos);
  PRINT_INT(va_account);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

static PyObject* hpssclapipy_fileattr_repr(hpssclapipy_fileattr_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_fileattr_t {\n");
  indent += 2;

  PRINT_OBJ(ObjectHandle);
  PRINT_OBJ(Attrs);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

static PyObject* hpssclapipy_xfileattr_repr(hpssclapipy_xfileattr_t *self) {
   char *str;
   PyObject *rc;

   str = (char*)malloc(32768);
   if(str == NULL)
     return PyErr_NoMemory();

   memset(str, 0, 32768);
   strcat(str, "hpss_xfileattr_t {\n");
   indent += 2;

   PRINT_OBJ(XAttr_ObjectHandle);
   PRINT_OBJ(XAttr_Attrs);
   PRINT_OBJ(XAttr_SCAttrib);

   indent -= 2;
   INDENT; strcat(str, "}");

   rc = Py_BuildValue("s", str);
   free(str);
   return rc;
}

static PyObject* hpssclapipy_bf_sc_attrib_repr(hpssclapipy_bf_sc_attrib_t *self) {
   /* TODO: complete function implementation */
   return 0;
}

static PyObject* hpssclapipy_Attrs_repr(hpssclapipy_Attrs_t *self) {
  char *str;
  PyObject *rc;

  str = (char*)malloc(32768);
  if(str == NULL)
    return PyErr_NoMemory();

  memset(str, 0, 32768);
  strcat(str, "hpss_fileattr_t {\n");
  indent += 2;

  PRINT_INT(Account);
  // BitfileId
  // TODO: Check into RealId printing
  // PRINT_INT(RealmId);
  PRINT_STR(Comment);
  PRINT_INT(CompositePerms);
  PRINT_INT(COSId);
  PRINT_U64(DataLength);
  PRINT_INT(EntryCount);
  PRINT_INT(ExtendedACLs);
  PRINT_INT(FamilyId);
  PRINT_OBJ(FilesetHandle);
  PRINT_U64(FilesetId);
#ifdef HPSS751
	PRINT_U64(FilesetRootObjectId);
#else  
  PRINT_U64(FilesetRootId);
#endif  

  INDENT; strcat(str, "FilesetStateFlags: ");
  if((self->data.FilesetStateFlags &
      (CORE_FS_STATE_READ
      |CORE_FS_STATE_WRITE
      |CORE_FS_STATE_DESTROYED)) == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(FilesetStateFlags, CORE_FS_STATE_READ);
    PRINT_FLAG(FilesetStateFlags, CORE_FS_STATE_WRITE);
    PRINT_FLAG(FilesetStateFlags, CORE_FS_STATE_DESTROYED);
    strcat(str, ",\n");
  }

  switch(self->data.FilesetType) {
    PRINT_ENUM(FilesetType, CORE_FS_TYPE_HPSS_ONLY);
    PRINT_ENUM(FilesetType, CORE_FS_TYPE_ARCHIVED);
    PRINT_ENUM(FilesetType, CORE_FS_TYPE_NATIVE);
    PRINT_ENUM(FilesetType, CORE_FS_TYPE_MIRRORED);
    default:
      PRINT_INT(FilesetType);
  }

  PRINT_INT(GID);
  PRINT_INT(GroupPerms);
  PRINT_INT(LinkCount);
  PRINT_INT(ModePerms);
  PRINT_INT(OpenCount);

  INDENT; strcat(str, "OptionFlags: ");
  if((self->data.OptionFlags & CORE_OPTION_FLAG_DONT_PURGE) == 0)
    strcat(str, "0,\n");
  else {
    int first = 1;
    PRINT_FLAG(OptionFlags, CORE_OPTION_FLAG_DONT_PURGE);
    strcat(str, ",\n");
  }

  PRINT_INT(OtherPerms);
  PRINT_INT(ReadCount);
#ifdef HPSS751  
  PRINT_INT(RealmId);
#endif  

  INDENT; strcat(str, "RegisterBitMap: ");

  {
    int first = 1;
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_ACCOUNT);
#ifdef HPSS751
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_BITFILE_VALIDHASH);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_BITFILE_HASH);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_BITFILE_ID);
#else    
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_BIT_FILE_ID);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_REALM_ID);
#endif    
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_COMMENT);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_COMPOSITE_PERMS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_COS_ID);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_DATA_LENGTH);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_ENTRY_COUNT);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_EXTENDED_ACLS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FAMILY_ID);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FILESET_HANDLE);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FILESET_ID);
#ifdef HPSS751
	PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FILESET_ROOT_OBJECT_ID);
#else    
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FILESET_ROOT_ID);
#endif    
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FILESET_STATE_FLAGS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_FILESET_TYPE);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_GID);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_GROUP_PERMS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_LINK_COUNT);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_MODE_PERMS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_OPEN_COUNT);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_OPTION_FLAGS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_OTHER_PERMS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_READ_COUNT);
#ifdef HPSS751
	 PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_REALM_ID);
#endif
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_REGISTER_BITMAP);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_SUB_SYSTEM_ID);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_TIME_CREATED);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_TIME_LAST_READ);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_TIME_LAST_WRITTEN);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_TIME_MODIFIED);
#ifdef HPSS751    
	 PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_TRASH_INFO);
#endif
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_TYPE);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_UID);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_USER_PERMS);
    PRINT_FLAGU64(RegisterBitMap, CORE_ATTR_WRITE_COUNT);
    if(first == 1)
      strcat(str, "0,\n");
    else
      strcat(str, ",\n");
  }

  PRINT_INT(SubSystemId);
  PRINT_TIME(TimeCreated);
  PRINT_TIME(TimeLastRead);
  PRINT_TIME(TimeLastWritten);
  PRINT_TIME(TimeModified);

  // TODO: print TrashInfo
  switch(self->data.Type) {
    PRINT_ENUM(Type, NS_OBJECT_TYPE_DIRECTORY);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_FILE);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_JUNCTION);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_SYM_LINK);
    PRINT_ENUM(Type, NS_OBJECT_TYPE_HARD_LINK);
    default:
      PRINT_INT(Type);
  }

  PRINT_INT(UID);
  PRINT_INT(UserPerms);
  PRINT_INT(WriteCount);

  indent -= 2;
  INDENT; strcat(str, "}");

  rc = Py_BuildValue("s", str);
  free(str);
  return rc;
}

PyObject* hpssclapipy_Fstat(PyObject *self, PyObject *args) {
  int fd, rc;
  hpssclapipy_stat_t *stat;

  if(!PyArg_ParseTuple(args, "i", &fd))
    return NULL;

  stat = TP_ALLOC(stat);
  if(stat == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Fstat(fd, &stat->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(stat);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)stat;
}

PyObject* hpssclapipy_Lstat(PyObject *self, PyObject *args) {
  int rc;
  char *filename;
  hpssclapipy_stat_t *stat;

  if(!PyArg_ParseTuple(args, "s", &filename))
    return NULL;

  stat = TP_ALLOC(stat);
  if(stat == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Lstat(filename, &stat->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(stat);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)stat;
}

PyObject* hpssclapipy_Stat(PyObject *self, PyObject *args) {
  int rc;
  char *filename;
  hpssclapipy_stat_t *stat;

  if(!PyArg_ParseTuple(args, "s", &filename))
    return NULL;

  stat = TP_ALLOC(stat);
  if(stat == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Stat(filename, &stat->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(stat);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)stat;
}

PyObject* hpssclapipy_Statfs(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_statfs_t *statfs;
  unsigned int cos;
  int rc;

  static char *kwlist[] = { "cos", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "I", kwlist, &cos))
    return NULL;

  statfs = TP_ALLOC(statfs);
  if(statfs == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Statfs(cos, &statfs->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(statfs);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)statfs;
}

PyObject* hpssclapipy_Statvfs(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_statvfs_t *statvfs;
  unsigned int cos;
  int rc;

  static char *kwlist[] = { "cos", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "I", kwlist, &cos))
    return NULL;

  statvfs = TP_ALLOC(statvfs);
  if(statvfs == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Statvfs(cos, &statvfs->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(statvfs);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)statvfs;
}

PyObject* hpssclapipy_Umask(PyObject *self, PyObject *args) {
  mode_t mode;

  if(!PyArg_ParseTuple(args, "i", &mode))
    return NULL;

  return Py_BuildValue("i", hpss_Umask(mode));
}

PyObject* hpssclapipy_GetAttrHandle(PyObject *self, PyObject *args) {
  int  status;
  char *path = NULL;
  hpssclapipy_ns_ObjHandle_t *objhandle;
  hpssclapipy_vattr_t        *vattr;
  hpssclapipy_sec_cred_t     *creds = NULL;
  sec_cred_t            *ucred = NULL;

  if(!PyArg_ParseTuple(args, "O!|sO!", &hpssclapipy_ns_ObjHandleType, &objhandle,
                                       &path,
                                       &hpssclapipy_sec_credType, &creds))
    return NULL;

  vattr = TP_ALLOC(vattr);
  if(vattr == NULL)
    return PyErr_NoMemory();

  if(creds != NULL)
    ucred = &creds->data;

  Py_BEGIN_ALLOW_THREADS
  status = hpss_GetAttrHandle(&objhandle->data, path, ucred, NULL, &vattr->data);
  Py_END_ALLOW_THREADS

  if(status != 0) {
    Py_DECREF(vattr);
    return PyErr_SetErrno(-status);
  }

  return (PyObject*)vattr;
}

PyObject* hpssclapipy_FileGetAttributes(PyObject *self, PyObject *args) {
  char *path;
  int  rc;
  hpssclapipy_fileattr_t *attr;

  if(!PyArg_ParseTuple(args, "s", &path))
    return NULL;

  attr = TP_ALLOC(fileattr);
  if(attr == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_FileGetAttributes(path, &attr->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(attr);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)attr;
}

PyObject* hpssclapipy_FileGetAttributesHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  char *path = NULL;
  hpssclapipy_ns_ObjHandle_t *objhandle;
  hpssclapipy_sec_cred_t *ucred = NULL;
  hpssclapipy_fileattr_t *attr;
  sec_cred_t *Ucred = NULL;
  int rc;

  static char *kwlist[] = { "objhandle", "path", "ucred", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "O!|sO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred))
    return NULL;

  if(ucred != NULL)
    Ucred = &ucred->data;

  attr = TP_ALLOC(fileattr);
  if(attr == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_FileGetAttributesHandle(&objhandle->data, path, Ucred, &attr->data);
  Py_END_ALLOW_THREADS

  if(rc != 0) {
    Py_DECREF(attr);
    return PyErr_SetErrno(-rc);
  }

  return (PyObject*)attr;
}

PyObject* hpssclapipy_FileGetXAttributes(PyObject *self, PyObject *args) {
   char *path;
   unsigned int flags;
   unsigned int storagelevel;
   hpss_xfileattr_t attrs;
   int rc;

   if(!PyArg_ParseTuple(args, "sII", &path, &flags, &storagelevel))
     return NULL;

   memset(&attrs, 0x0, sizeof(attrs));

   Py_BEGIN_ALLOW_THREADS
   rc = hpss_FileGetXAttributes(path, flags, storagelevel,
                                &attrs);
   Py_END_ALLOW_THREADS
   if (rc < 0)
   {
      return PyErr_SetErrno(-rc);
   }

   /* TODO: complete returning a Python object for the extended attrs */

   Py_RETURN_NONE;

}

PyObject* hpssclapipy_FileSetCOS(PyObject *self, PyObject *args) {
      char           *path;
      unsigned int   COSId;
      unsigned int   StreamId;
      int            rc;

      if(!PyArg_ParseTuple(args, "sII", &path, &COSId, &StreamId))
        return NULL;

      Py_BEGIN_ALLOW_THREADS
      rc = hpss_FileSetCOS(path, COSId, StreamId);
      Py_END_ALLOW_THREADS

      if (rc < 0)
      {
         return PyErr_SetErrno(-rc);
      }

      return Py_BuildValue("i", rc);
}

PyObject * hpssclapipy_BytesAtLevel(PyObject *self, PyObject *args) {

   /* TODO: Due to time constraints implemented this function
    * that is not part of the HPSS client API. This function
    * will facilitate obtaining the number of bytes a file
    * has at a level. Look into removing this function and use
    * hpss_FileGetXAttributes (06/17/2015)
    */
   char *path;
   unsigned int storagelevel;
   hpss_xfileattr_t attrs;
   char msg[80];
   unsigned long bytesAtLevel = 0UL;
   int j;

   int rc;

   if(!PyArg_ParseTuple(args, "sI", &path, &storagelevel))
     return NULL;

   /*
    * Check the level requested exist
    */
   if (storagelevel > HPSS_MAX_STORAGE_LEVELS) {
      sprintf(msg, "Invalid storage level specified: %u", storagelevel);
      PyErr_SetString(PyExc_IOError, msg);
      return NULL;
   }

   memset(&attrs, 0x0, sizeof(attrs));

   Py_BEGIN_ALLOW_THREADS
   rc = hpss_FileGetXAttributes(path, API_GET_STATS_FOR_LEVEL, storagelevel,
                                &attrs);
   Py_END_ALLOW_THREADS
   if (rc < 0)
   {
      return PyErr_SetErrno(-rc);
   }

   bytesAtLevel = attrs.SCAttrib[storagelevel].BytesAtLevel;
   for (j = 0; j < attrs.SCAttrib[storagelevel].NumberOfVVs; j++) {
      free(attrs.SCAttrib[storagelevel].VVAttrib[j].PVList);
   }

   return Py_BuildValue("k", bytesAtLevel);
}

PyObject * hpssclapipy_RelativePosVVAttrib(PyObject *self, PyObject *args) {

   /* TODO: Due to time constraints implemented this function
    * that is not part of the HPSS client API. This function
    * will facilitate obtaining the file relative position
    * has at a level. Look into removing this function and use
    * hpss_FileGetXAttributes (08/18/2015)
    */
   char              *path;
   unsigned int      storagelevel;
   hpss_xfileattr_t  attrs;
   char              msg[80];
   int               j, k, i;
   // SOIDs are 32 characters
   char              *vvid_str;
   byte              *byteP;
   size_t            vvid_len;
   FILE              *vvid_fp;

   int               rc;
   PyObject          *value = NULL, *vvid_pos_dict;

   if(!PyArg_ParseTuple(args, "sI", &path, &storagelevel))
     return NULL;

   /*
    * Check the level requested exist
    */
   if (storagelevel > HPSS_MAX_STORAGE_LEVELS) {
      sprintf(msg, "Invalid storage level specified: %u", storagelevel);
      PyErr_SetString(PyExc_IOError, msg);
      return NULL;
   }

   memset(&attrs, 0x0, sizeof(attrs));

   Py_BEGIN_ALLOW_THREADS
   rc = hpss_FileGetXAttributes(path, API_GET_STATS_FOR_LEVEL, storagelevel,
                                &attrs);
   Py_END_ALLOW_THREADS
   if (rc < 0)
   {
      return PyErr_SetErrno(-rc);
   }

   vvid_fp = open_memstream(&vvid_str, &vvid_len);
   if (!vvid_fp)
   {
      return PyErr_SetErrno(errno);
   }

   vvid_pos_dict = PyDict_New();
   if (NULL == vvid_pos_dict)
   {
      return PyErr_NoMemory();
   }

   for (j = 0; j < attrs.SCAttrib[storagelevel].NumberOfVVs; j++) {
      free(attrs.SCAttrib[storagelevel].VVAttrib[j].PVList);
      value = Py_BuildValue("I",
            attrs.SCAttrib[storagelevel].VVAttrib[j].RelPosition);
      byteP = (byte *) &attrs.SCAttrib[storagelevel].VVAttrib[j].VVID;
      for (i =0; i < sizeof(hpssoid_t); i++) {
         fprintf(vvid_fp, "%02X", *byteP);
         byteP++;
      }

      fflush(vvid_fp);

      if (NULL == value ||
            PyDict_SetItemString(vvid_pos_dict, vvid_str, value) < 0)
      {
         for (k = j; k < attrs.SCAttrib[storagelevel].NumberOfVVs; k++) {
            free(attrs.SCAttrib[storagelevel].VVAttrib[k].PVList);
         }

         fclose(vvid_fp);
         sprintf(msg, "Could not create key, value dictionary for storage "
               "level: %u vvid: %s", storagelevel, vvid_str);
         PyErr_SetString(PyExc_IOError, msg);
         free(vvid_str);
         if (NULL != value)
            Py_DECREF(value);
         Py_DECREF(vvid_pos_dict);
         return NULL;
      }

      Py_DECREF(value);
      fseek(vvid_fp, 0, SEEK_SET);
   }
   fclose(vvid_fp);
   return vvid_pos_dict;
}

PyObject* hpssclapipy_Chmod(PyObject *self, PyObject *args) {
  char   *path;
  mode_t mode;
  int    rc;

  if(!PyArg_ParseTuple(args, "si", &path, &mode))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Chmod(path, mode);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_Chown(PyObject *self, PyObject *args) {
  char  *path;
  uid_t uid;
  gid_t gid;
  int   rc;

  if(!PyArg_ParseTuple(args, "sii", &path, &uid, &gid))
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  rc = hpss_Chown(path, uid, gid);
  Py_END_ALLOW_THREADS

  if(rc != 0)
    return PyErr_SetErrno(-rc);

  Py_RETURN_NONE;
}

PyObject* hpssclapipy_SetAttrHandle(PyObject *self, PyObject *args, PyObject *keywds) {
  hpssclapipy_ns_ObjHandle_t *objhandle = NULL;
  ns_ObjHandle_t        *ObjHandle = NULL;
  char                  *path      = NULL;
  hpssclapipy_sec_cred_t     *ucred     = NULL;
  sec_cred_t            *Ucred     = NULL;
  unsigned long long    flagsin    = 0;
  u_signed64            SelFlagsIn = cast64m(0), SelFlagsOut = cast64m(0);
  hpssclapipy_vattr_t        *vattrin   = NULL, *vattrout;
  int status;
  PyObject *rc;

  static char *kwlist[] = { "objhandle", "path", "ucred", "flags", "attr", NULL };

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "|O!sO!KO!", kwlist,
                                  &hpssclapipy_ns_ObjHandleType, &objhandle,
                                  &path,
                                  &hpssclapipy_sec_credType, &ucred,
                                  &flagsin,
                                  &hpssclapipy_vattrType, &vattrin))
    return NULL;

  if(objhandle != NULL)
    ObjHandle = &objhandle->data;
  if(ucred != NULL)
    Ucred = &ucred->data;
  SelFlagsIn = ULL_TO_U64(flagsin);

  vattrout = TP_ALLOC(vattr);
  if(vattrout == NULL)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  status = hpss_SetAttrHandle(ObjHandle, path, Ucred, SelFlagsIn, &vattrin->data, &SelFlagsOut, &vattrout->data);
  Py_END_ALLOW_THREADS

  if(status != 0) {
    Py_DECREF(vattrout);
    return PyErr_SetErrno(-status);
  }

  rc = Py_BuildValue("(KN)", U64_TO_ULL(SelFlagsOut), vattrout);
  if(rc == NULL) {
    Py_DECREF(vattrout);
    return PyErr_NoMemory();
  }

  return rc;
}
