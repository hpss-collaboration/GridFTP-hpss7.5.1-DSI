#include <Python.h>
#include <errno.h>
#include <string.h>
#include <hpss_InfraLog.h>
#include <hpss_sec.h>
#include <hpss_uuid.h>
#include <hpss_mech.h>
#include "hpssclapipy.h"
#include "doc.h"

int indent = 0;

static PyMethodDef hpss_methods[] = {
#define METHOD(x, m) { #x, (PyCFunction)hpssclapipy_##x, METH_##m, DOC_##x, },
#include "methods.h"
#undef METHOD

  { NULL, NULL, 0, NULL, },
};

typedef struct {
  int value;
  const char *name;
} hpss_macro_t;

static const hpss_macro_t macros[] = {
#define hpss_macro(x) { x, #x, }
  // hpss_Open() flags
  hpss_macro(O_RDONLY),
  hpss_macro(O_WRONLY),
  hpss_macro(O_RDWR),
  hpss_macro(O_APPEND),
  hpss_macro(O_CREAT),
  hpss_macro(O_EXCL),
  hpss_macro(O_TRUNC),
  hpss_macro(O_NONBLOCK),

  // NS object types
  hpss_macro(NS_OBJECT_TYPE_DIRECTORY),
  hpss_macro(NS_OBJECT_TYPE_FILE),
  hpss_macro(NS_OBJECT_TYPE_JUNCTION),
  hpss_macro(NS_OBJECT_TYPE_SYM_LINK),
  hpss_macro(NS_OBJECT_TYPE_HARD_LINK),

  // NS object handle flags
  hpss_macro(NS_OH_FLAG_FILESET_ROOT),

  // hpss_Migrate() flags
  hpss_macro(HPSS_MIGRATE_FORCE),
  hpss_macro(HPSS_MIGRATE_PURGE_DATA),
  hpss_macro(HPSS_MIGRATE_NO_COPY),

  // hpss_PurgeLock() flags
  hpss_macro(PURGE_LOCK),
  hpss_macro(PURGE_UNLOCK),

  // hpss_Stage() flags
  hpss_macro(BFS_STAGE_ALL),
  hpss_macro(BFS_ASYNCH_CALL),

  // api_config_t flags
  hpss_macro(API_ENABLE_LOGGING),
  hpss_macro(API_DISABLE_CROSS_REALM),
  hpss_macro(API_DISABLE_JUNCTIONS),
  hpss_macro(API_USE_CONFIG),
  hpss_macro(API_USE_SAN3P),

  // api_config_t transfer type
  hpss_macro(API_TRANSFER_TCP),
  /* hpss_macro(API_TRANSFER_IPI3), */
  hpss_macro(API_TRANSFER_MVRSELECT),

  // hpss_authn_mech_t
  hpss_macro(hpss_authn_mech_invalid),
  hpss_macro(hpss_authn_mech_krb5),
  hpss_macro(hpss_authn_mech_unix),
  hpss_macro(hpss_authn_mech_gsi),
  hpss_macro(hpss_authn_mech_spkm),

  // hpss_rpc_protect_connect
  hpss_macro(hpss_rpc_protect_connect),
  hpss_macro(hpss_rpc_protect_pkt),
  hpss_macro(hpss_rpc_protect_pkt_integ),
  hpss_macro(hpss_rpc_protect_pkt_privacy),

  // hpss_cos_hints_t flags
  hpss_macro(HINTS_FORCE_MAX_SSEG),
  hpss_macro(HINTS_NOTRUNC_FINAL_SEG),

  // hpss_cos_hints_t access frequency
  hpss_macro(FREQ_HOURLY),
  hpss_macro(FREQ_DAILY),
  hpss_macro(FREQ_WEEKLY),
  hpss_macro(FREQ_MONTHLY),
  hpss_macro(FREQ_ARCHIVE),

  // hpss_cos_hints_t read/write ops
  hpss_macro(HPSS_OP_WRITE),
  hpss_macro(HPSS_OP_APPEND),
  hpss_macro(HPSS_OP_READ),

  // hpss_cos_hints_t stage code
  hpss_macro(COS_STAGE_NO_STAGE),
  hpss_macro(COS_STAGE_ON_OPEN),
  hpss_macro(COS_STAGE_ON_OPEN_ASYNCH),
  hpss_macro(COS_STAGE_ON_OPEN_BACKGROUND),

  // hpss_cos_priorities_t values
  hpss_macro(NO_PRIORITY),
  hpss_macro(LOWEST_PRIORITY),
  hpss_macro(LOW_PRIORITY),
  hpss_macro(DESIRED_PRIORITY),
  hpss_macro(HIGHLY_DESIRED_PRIORITY),
  hpss_macro(REQUIRED_PRIORITY),

  // PIO values
  hpss_macro(HPSS_PIO_READ),
  hpss_macro(HPSS_PIO_WRITE),
  hpss_macro(HPSS_PIO_TCPIP),
  hpss_macro(HPSS_PIO_MVR_SELECT),
  hpss_macro(HPSS_PIO_PUSH),
  hpss_macro(HPSS_PIO_HANDLE_GAP),
  hpss_macro(HPSS_PIO_PORT_RANGE),
  hpss_macro(HPSS_PIO_STRIPE_UNKNOWN),

  // hpss net values
  hpss_macro(AI_ADDRCONFIG),
  hpss_macro(AI_ALL),
  hpss_macro(AI_CANONNAME),
  hpss_macro(AI_NUMERICHOST),
  hpss_macro(AI_PASSIVE),
  hpss_macro(AI_V4MAPPED),
  hpss_macro(HPSS_IPPROTO_TCP),
  hpss_macro(HPSS_IPPROTO_UDP),
  hpss_macro(HPSS_IPPROTO_UNSPECIFIED),
  hpss_macro(HPSS_NET_IPV4_ONLY),
  hpss_macro(HPSS_NET_IPV6),
  hpss_macro(HPSS_NET_IPV6_ONLY),
  hpss_macro(HPSS_NET_UNSPECIFIED),
  hpss_macro(NI_DGRAM),
  hpss_macro(NI_NAMEREQD),
  hpss_macro(NI_NOFQDN),
  hpss_macro(NI_NUMERICHOST),
  hpss_macro(NI_NUMERICSERV),
  hpss_macro(SOCK_STREAM),
  hpss_macro(SOCK_DGRAM),

  // hpss_stat_t.st_mode flags
  hpss_macro(S_IFMT),
  hpss_macro(S_IFREG),
  hpss_macro(S_IFDIR),
  hpss_macro(S_IFLNK),
  hpss_macro(S_IFBLK),
  hpss_macro(S_IFCHR),
  hpss_macro(S_IFIFO),
  hpss_macro(S_ISUID),
  hpss_macro(S_ISGID),
  hpss_macro(S_ISVTX),
  hpss_macro(S_IRWXU),
  hpss_macro(S_IRUSR),
  hpss_macro(S_IWUSR),
  hpss_macro(S_IXUSR),
  hpss_macro(S_IRWXG),
  hpss_macro(S_IRGRP),
  hpss_macro(S_IWGRP),
  hpss_macro(S_IXGRP),
  hpss_macro(S_IRWXO),
  hpss_macro(S_IROTH),
  hpss_macro(S_IWOTH),
  hpss_macro(S_IXOTH),

  // hpss_Attrs_t.FilesetStateFlags
  hpss_macro(CORE_FS_STATE_READ),
  hpss_macro(CORE_FS_STATE_WRITE),
  hpss_macro(CORE_FS_STATE_DESTROYED),

  // hpss_Attrs_t.FilesetType
  hpss_macro(CORE_FS_TYPE_HPSS_ONLY),
  hpss_macro(CORE_FS_TYPE_ARCHIVED),
  hpss_macro(CORE_FS_TYPE_NATIVE),
  hpss_macro(CORE_FS_TYPE_MIRRORED),

  // hpss_Attrs_t.RegisterBitMap flags
  hpss_macro(CORE_ATTR_ACCOUNT),
#ifdef HPSS751
  hpss_macro(CORE_ATTR_BITFILE_VALIDHASH),
  hpss_macro(CORE_ATTR_BITFILE_HASH),
  hpss_macro(CORE_ATTR_BITFILE_ID),
#else  
  hpss_macro(CORE_ATTR_BIT_FILE_ID),
  hpss_macro(CORE_ATTR_REALM_ID),
#endif  
  hpss_macro(CORE_ATTR_COMMENT),
  hpss_macro(CORE_ATTR_COMPOSITE_PERMS),
  hpss_macro(CORE_ATTR_COS_ID),
  hpss_macro(CORE_ATTR_DATA_LENGTH),
  hpss_macro(CORE_ATTR_ENTRY_COUNT),
  hpss_macro(CORE_ATTR_EXTENDED_ACLS),
  hpss_macro(CORE_ATTR_FAMILY_ID),
  hpss_macro(CORE_ATTR_FILESET_HANDLE),
  hpss_macro(CORE_ATTR_FILESET_ID),
#ifdef HPSS751
  hpss_macro(CORE_ATTR_FILESET_ROOT_OBJECT_ID),
#else
	hpss_macro(CORE_ATTR_FILESET_ROOT_ID),
#endif 
  hpss_macro(CORE_ATTR_FILESET_STATE_FLAGS),
  hpss_macro(CORE_ATTR_FILESET_TYPE),
  hpss_macro(CORE_ATTR_GID),
  hpss_macro(CORE_ATTR_GROUP_PERMS),
  hpss_macro(CORE_ATTR_LINK_COUNT),
  hpss_macro(CORE_ATTR_MODE_PERMS),
  hpss_macro(CORE_ATTR_OPEN_COUNT),
  hpss_macro(CORE_ATTR_OPTION_FLAGS),
  hpss_macro(CORE_ATTR_OTHER_PERMS),
  hpss_macro(CORE_ATTR_READ_COUNT),
#ifdef HPSS751  
  hpss_macro(CORE_ATTR_REALM_ID),
#endif
  hpss_macro(CORE_ATTR_REGISTER_BITMAP),
  hpss_macro(CORE_ATTR_SUB_SYSTEM_ID),
  hpss_macro(CORE_ATTR_TIME_CREATED),
  hpss_macro(CORE_ATTR_TIME_LAST_READ),
  hpss_macro(CORE_ATTR_TIME_LAST_WRITTEN),
  hpss_macro(CORE_ATTR_TIME_MODIFIED),
  // TODO: Add TrashInfo
  //hpss_macro(CORE_ATTR_TRASH_INFO),
  hpss_macro(CORE_ATTR_TYPE),
  hpss_macro(CORE_ATTR_UID),
  hpss_macro(CORE_ATTR_USER_PERMS),
  hpss_macro(CORE_ATTR_WRITE_COUNT),

  // hpss_vattr_t.va_type
  hpss_macro(F_VNON),
  hpss_macro(F_VREG),
  hpss_macro(F_VDIR),
  hpss_macro(F_VBLK),
  hpss_macro(F_VCHR),
  hpss_macro(F_VLNK),
  hpss_macro(F_VSOCK),
  hpss_macro(F_VBAD),
  hpss_macro(F_VFIFO),
  hpss_macro(F_VMPC),

  // hpss_rpc_cred_type
  hpss_macro(hpss_rpc_cred_server),
  hpss_macro(hpss_rpc_cred_client),
  hpss_macro(hpss_rpc_cred_both),

  // hpss_rpc_auth_type
  hpss_macro(hpss_rpc_auth_type_invalid),
  hpss_macro(hpss_rpc_auth_type_none),
  hpss_macro(hpss_rpc_auth_type_keytab),
  hpss_macro(hpss_rpc_auth_type_keyfile),
  hpss_macro(hpss_rpc_auth_type_key),
  hpss_macro(hpss_rpc_auth_type_passwd),

  // hpss_Access mode flags
  hpss_macro(F_OK),
  hpss_macro(R_OK),
  hpss_macro(W_OK),
  hpss_macro(X_OK),

  // hpss_UserAttrListAttrs and friends flags
  hpss_macro(XML_ATTR),
  hpss_macro(XML_NO_ATTR),

  // hpss_UserAttrGetAttrs and friends flags
  hpss_macro(UDA_API_VALUE),
  hpss_macro(UDA_API_XML),

  // hpss_SetFileOffset whence and direction flags
  hpss_macro(SEEK_SET),
  hpss_macro(SEEK_CUR),
  hpss_macro(SEEK_END),
  hpss_macro(HPSS_SET_OFFSET_FORWARD),
  hpss_macro(HPSS_SET_OFFSET_BACKWARD),

  // hpss_GetLastHPSSErrno error number
  hpss_macro(HPSS_E_NOERROR),

  // hpss_SetAttrHandle set flags
  hpss_macro(VA_TYPE),
  hpss_macro(VA_MODE),
  hpss_macro(VA_UID),
  hpss_macro(VA_GID),
  hpss_macro(VA_FSID),
  hpss_macro(VA_SERIALNO),
  hpss_macro(VA_NLINK),
  hpss_macro(VA_RDEV),
  hpss_macro(VA_NID),
  hpss_macro(VA_CHAN),
  hpss_macro(VA_ACLSIZE),
  hpss_macro(VA_SIZE),
  hpss_macro(VA_PREFIOSIZE),
  hpss_macro(VA_BLOCKSIZE),
  hpss_macro(VA_NUMBLOCKS),
  hpss_macro(VA_ATIME),
  hpss_macro(VA_MTIME),
  hpss_macro(VA_CTIME),
  hpss_macro(VA_ACL),
  hpss_macro(VA_OBJHANDLE),
  hpss_macro(VA_SOID),
  hpss_macro(VA_COS),
  hpss_macro(VA_ACCOUNT),

#ifndef HPSS743P2A
  // hpss HashType's
  hpss_macro(HASH_NONE),
  hpss_macro(HASH_SHA1),
  hpss_macro(HASH_SHA224),
  hpss_macro(HASH_SHA256),
  hpss_macro(HASH_SHA384),
  hpss_macro(HASH_SHA512),
  hpss_macro(HASH_MD5),
  hpss_macro(HASH_CRC32),
  hpss_macro(HASH_ADLER32),
#endif

  // hpss errno's
  hpss_macro(HPSS_EOM),
};
#define NUM_MACROS (sizeof(macros)/sizeof(*macros))

PyMODINIT_FUNC inithpssclapi(void) {
  PyObject *m, *typeObject;
  int i;
 
  // initialize the types
  if(0
#define TYPE_INIT(x) (PyType_Ready(&hpssclapipy_##x##Type) == 0)
  || !TYPE_INIT(api_config)
  || !TYPE_INIT(Attrs)
  || !TYPE_INIT(cos_hints)
  || !TYPE_INIT(cos_priorities)
  || !TYPE_INIT(dirent)
  || !TYPE_INIT(errno_state)
  || !TYPE_INIT(FILE)
  || !TYPE_INIT(fileattr)
#ifndef HPSS743P2A
  || !TYPE_INIT(hash)
#endif
  || !TYPE_INIT(ns_ObjHandle)
  || !TYPE_INIT(pio_gapinfo)
  || !TYPE_INIT(pio_grp)
  || !TYPE_INIT(pio_params)
  || !TYPE_INIT(sec_cred)
  || !TYPE_INIT(sockaddr)
  || !TYPE_INIT(stat)
  || !TYPE_INIT(statfs)
  || !TYPE_INIT(statvfs)
  || !TYPE_INIT(uuid)
  || !TYPE_INIT(vattr)
  )
    return;

   /* Client API initializations since they won't get
   * automatically called by the shared object loader
   */
  hpss_EnvInitialize();
  hpss_GASInitialize();
  hpss_InfraLogInitialize();
  hpss_SECInitialize();
  hpss_ThreadInitialize();
  hpss_ThreadPostInit();
#ifndef HPSS751  
  hpss_UUIDInitialize();
#endif

  m = Py_InitModule3("hpssclapi", hpss_methods, "HPSS Client API Python Bindings");

  // add the types
#define TYPE_ADD(x) \
  Py_INCREF(typeObject = (PyObject*)&hpssclapipy_##x##Type); \
  PyModule_AddObject(m, #x, (PyObject*)&hpssclapipy_##x##Type)
  TYPE_ADD(api_config);
  TYPE_ADD(Attrs);
  TYPE_ADD(cos_hints);
  TYPE_ADD(cos_priorities);
  TYPE_ADD(dirent);
  TYPE_ADD(errno_state);
  TYPE_ADD(FILE);
  TYPE_ADD(fileattr);
#ifndef HPSS743P2A
  TYPE_ADD(hash);
#endif
  TYPE_ADD(ns_ObjHandle);
  TYPE_ADD(pio_gapinfo);
  TYPE_ADD(pio_grp);
  TYPE_ADD(pio_params);
  TYPE_ADD(sec_cred);
  TYPE_ADD(sockaddr);
  TYPE_ADD(stat);
  TYPE_ADD(statfs);
  TYPE_ADD(statvfs);
  TYPE_ADD(uuid);
  TYPE_ADD(vattr);

  for(i = 0; i < NUM_MACROS; i++)
    PyModule_AddIntConstant(m, macros[i].name, macros[i].value);
}
