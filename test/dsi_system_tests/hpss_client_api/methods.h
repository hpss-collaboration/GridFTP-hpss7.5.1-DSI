/* use ":sort i" in vim to sort list */
METHOD(Access,                          KEYWORDS)
METHOD(AccessHandle,                    KEYWORDS)
METHOD(AuthnMechTypeFromString,         VARARGS)
METHOD(BuildLevelString,                NOARGS)
METHOD(BytesAtLevel,                    VARARGS)
METHOD(RelativePosVVAttrib,             VARARGS)
METHOD(Chdir,                           KEYWORDS)
METHOD(Chmod,                           VARARGS)
METHOD(ChompXMLHeader,                  KEYWORDS)
METHOD(Chown,                           VARARGS)
METHOD(Chroot,                          KEYWORDS)
METHOD(Close,                           KEYWORDS)
METHOD(Closedir,                        KEYWORDS)
METHOD(CopyFile,                        KEYWORDS)
METHOD(CopyrightStrings,                NOARGS)
METHOD(Creat,                           KEYWORDS)
METHOD(Create,                          KEYWORDS)
METHOD(CreateHandle,                    KEYWORDS)
METHOD(Fclear,                          KEYWORDS)
METHOD(FclearOffset,                    KEYWORDS)
METHOD(Fclose,                          KEYWORDS)
METHOD(Fflush,                          KEYWORDS)
METHOD(Fgetc,                           KEYWORDS)
METHOD(Fgets,                           KEYWORDS)
METHOD(FileGetAttributes,               VARARGS)
METHOD(FileGetAttributesHandle,         KEYWORDS)
METHOD(FileGetXAttributes,              VARARGS)
METHOD(Fopen,                           KEYWORDS)
METHOD(Fpreallocate,                    KEYWORDS)
METHOD(Fread,                           KEYWORDS)
METHOD(Fseek,                           KEYWORDS)
METHOD(FileSetCOS,                      VARARGS)
METHOD(Fstat,                           VARARGS)
METHOD(Fsync,                           KEYWORDS)
METHOD(Ftell,                           KEYWORDS)
METHOD(Ftruncate,                       KEYWORDS)
METHOD(Fwrite,                          KEYWORDS)
METHOD(GetAttrHandle,                   VARARGS)
METHOD(GetConfiguration,                NOARGS)
METHOD(Getcwd,                          NOARGS)
METHOD(Getenv,                          VARARGS)
METHOD(GetLastHPSSErrno,                NOARGS)
METHOD(GetThreadUcred,                  NOARGS)
#ifndef HPSS751
METHOD(GetObjId,                        KEYWORDS)
METHOD(GetObjType,                      KEYWORDS)
METHOD(GetPathHandleObjId,              KEYWORDS)
METHOD(HandleCompare,                   KEYWORDS)
#endif
METHOD(HashAppend,                      KEYWORDS)
METHOD(HashAppendBuf,                   KEYWORDS)
METHOD(HashAppendStr,                   KEYWORDS)
METHOD(HashCreate,                      KEYWORDS)
METHOD(HashCreateAdler32,               NOARGS)
METHOD(HashCreateCrc32,                 NOARGS)
METHOD(HashCreateMD5,                   NOARGS)
METHOD(HashCreateSHA1,                  NOARGS)
METHOD(HashCreateSHA224,                NOARGS)
METHOD(HashCreateSHA256,                NOARGS)
METHOD(HashCreateSHA384,                NOARGS)
METHOD(HashCreateSHA512,                NOARGS)
METHOD(HashDuplicate,                   KEYWORDS)
METHOD(HashExtract,                     KEYWORDS)
METHOD(HashFinish,                      KEYWORDS)
METHOD(HashFinishHex,                   KEYWORDS)
METHOD(HashLoad,                        KEYWORDS)
METHOD(HashReset,                       KEYWORDS)
METHOD(Link,                            KEYWORDS)
METHOD(LinkHandle,                      KEYWORDS)
METHOD(Lstat,                           VARARGS)
METHOD(Migrate,                         VARARGS)
METHOD(Mkdir,                           KEYWORDS)
METHOD(MkdirHandle,                     KEYWORDS)
METHOD(net_accept,                      KEYWORDS)
METHOD(net_addrmatch,                   KEYWORDS)
METHOD(net_bind,                        KEYWORDS)
METHOD(net_connect,                     KEYWORDS)
METHOD(net_generatemask,                KEYWORDS)
METHOD(net_get_family_option,           NOARGS)
METHOD(net_get_family_string,           NOARGS)
METHOD(net_getaddrinfo,                 KEYWORDS)
METHOD(net_getaddrinfo_ex,              KEYWORDS)
METHOD(net_getinaddr,                   KEYWORDS)
METHOD(net_getnameinfo,                 KEYWORDS)
METHOD(net_getpeername,                 KEYWORDS)
METHOD(net_getport,                     KEYWORDS)
METHOD(net_getsockname,                 KEYWORDS)
METHOD(net_getuniversaladdress,         KEYWORDS)
METHOD(net_listen,                      KEYWORDS)
METHOD(net_setport,                     KEYWORDS)
METHOD(net_socket,                      KEYWORDS)
METHOD(net_universaladdresstoport,      KEYWORDS)
METHOD(Open,                            KEYWORDS)
METHOD(Opendir,                         KEYWORDS)
METHOD(OpenHandle,                      KEYWORDS)
METHOD(ParseAuthString,                 KEYWORDS)
METHOD(PIOEnd,                          VARARGS)
METHOD(PIOExecute,                      VARARGS)
METHOD(PIOExportGrp,                    VARARGS)
METHOD(PIOImportGrp,                    VARARGS)
METHOD(PIORegister,                     KEYWORDS)
METHOD(PIOStart,                        VARARGS)
METHOD(Purge,                           VARARGS)
METHOD(PurgeLock,                       VARARGS)
METHOD(PurgeLoginCred,                  NOARGS)
METHOD(Read,                            KEYWORDS)
METHOD(Readdir,                         KEYWORDS)
METHOD(ReaddirHandle,                   KEYWORDS)
METHOD(Readlink,                        KEYWORDS)
METHOD(ReadlinkHandle,                  KEYWORDS)
METHOD(Rename,                          KEYWORDS)
METHOD(RenameHandle,                    KEYWORDS)
METHOD(Rewinddir,                       KEYWORDS)
METHOD(Rmdir,                           KEYWORDS)
METHOD(RmdirHandle,                     KEYWORDS)
METHOD(SECGetCredsByUid,                VARARGS)
METHOD(SECGetCredsByName,                VARARGS)
METHOD(SECGetGroupNameById,             VARARGS)
METHOD(SetAttrHandle,                   KEYWORDS)
METHOD(SetConfiguration,                VARARGS)
METHOD(SetFileOffset,                   KEYWORDS)
METHOD(SetLoginCred,                    KEYWORDS)
METHOD(Stage,                           VARARGS)
METHOD(Stat,                            VARARGS)
METHOD(Statfs,                          KEYWORDS)
METHOD(Statvfs,                         KEYWORDS)
METHOD(Symlink,                         KEYWORDS)
METHOD(SymlinkHandle,                   KEYWORDS)
METHOD(Truncate,                        KEYWORDS)
METHOD(TruncateHandle,                  KEYWORDS)
METHOD(Umask,                           VARARGS)
METHOD(Undelete,                        KEYWORDS)
METHOD(UndeleteHandle,                  KEYWORDS)
METHOD(Unlink,                          KEYWORDS)
METHOD(UnlinkHandle,                    KEYWORDS)
METHOD(UserAttrDeleteAttrHandle,        KEYWORDS)
METHOD(UserAttrDeleteAttrs,             KEYWORDS)
// TODO: Fix UDA functions for 751
METHOD(UserAttrGetAttrHandle,           KEYWORDS)
METHOD(UserAttrGetAttrs,                KEYWORDS)
#ifndef HPSS751
METHOD(UserAttrLenGetAttrHandle,        KEYWORDS)
METHOD(UserAttrLenGetAttrs,             KEYWORDS)
METHOD(UserAttrLenListAttrHandle,       KEYWORDS)
METHOD(UserAttrLenListAttrs,            KEYWORDS)
#endif
METHOD(UserAttrListAttrHandle,          KEYWORDS)
METHOD(UserAttrListAttrs,               KEYWORDS)
METHOD(UserAttrSetAttrHandle,           KEYWORDS)
METHOD(UserAttrSetAttrs,                KEYWORDS)
METHOD(UserAttrXQueryGet,               KEYWORDS)
METHOD(UserAttrXQueryGetHandle,         KEYWORDS)
METHOD(UserAttrXQueryUpdate,            KEYWORDS)
METHOD(UserAttrXQueryUpdateHandle,      KEYWORDS)
METHOD(Write,                           KEYWORDS)
