// $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/doc.h $ $Id: doc.h 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
#pragma once

#define DOC_Access \
"Access(path, mode) -> True/False\n\n" \
"  Check file accessibility\n\n" \
"  Parameters:\n" \
"    path Path to file to check\n" \
"    mode Access mode to check\n" \
"         Can be F_OK or bitwise or of R_OK, W_OK, and X_OK"

#define DOC_AccessHandle \
"AccessHandle(objhandle=NULL, path=NULL, mode=0, ucred=NULL) -> True/False\n\n" \
"  Check file accessibility\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to file to check\n" \
"    mode      Access mode to check\n" \
"              Can be F_OK or bitwise or of R_OK, W_OK, and X_OK\n" \
"    ucred     User credentials"

#define DOC_AuthnMechTypeFromString \
"AuthnMechTypeFromString(str) -> int\n\n" \
"  Get an authentication mechanism from a string\n\n" \
"  Examples:\n" \
"    \"unix\" -> hpss.hpss_authn_mech_unix\n" \
"    \"krb5\" -> hpss.hpss_authn_mech_krb5\n\n" \
"  Returns authentication mechanism as an int"

#define DOC_BuildLevelString \
"BuildLevelString() -> str\n\n" \
"  Construct a string containing the HPSS version information\n\n" \
"  Returns the version as a string"

#define DOC_FileSetCOS \
"FileSetCOS(path, COSId) -> int\n\n" \
" Set a new class of service id for file specified by path\n\n" \
" Returns return code for class of service change operation"

#define DOC_BytesAtLevel \
"BytesAtLevel(path, level) -> int\n\n" \
" Obtain the number of bytes file has at a storage level\n\n" \
" Returns the number of bytes at the level specified"

#define DOC_RelativePosVVAttrib \
"RelativePosVVAttrib(path, level) -> list of unsigned int\n\n" \
" Obtain the storage device relative position at a storage level\n\n" \
" Returns the relative position at the level specified"

#define DOC_Chdir \
"Chdir(path)\n\n" \
"  Change current working directory"

#define DOC_Chmod \
"Chmod(path, mode)\n\n" \
"  Change file mode"

#define DOC_ChompXMLHeader \
"ChompXMLHeader(xml)\n\n" \
"  Get xml with header removed"

#define DOC_Chown \
"Chown(path, uid, gid)\n\n" \
"  Change file owner and group"

#define DOC_Chroot \
"Chroot(path)\n\n" \
"  Change the root directory"

#define DOC_Close \
"Close(fd)\n\n" \
"  Close an HPSS file descriptor"

#define DOC_Closedir \
"Closedir(fd)\n\n" \
"  Close an HPSS directory file descriptor"

#define DOC_CopyFile \
"CopyFile(src, dst)\n\n" \
"  Copy source file to destination file\n\n" \
"  Parameters:\n" \
"    src Source file descriptor\n" \
"    dst Destination file descriptor"

#define DOC_CopyrightStrings \
"CopyrightStrings() -> [str, ...]\n\n" \
"  Read the copyright file and return the strings, omitting the lines up to\n" \
"  and including \"All rights reserved\"\n\n" \
"  Returns a list of strings, each of which is a line from the copyright file"

#define DOC_Creat \
"Creat(path, mode=0777, hints=NULL, priorities=NULL) -> (int, hpss.cos_hints)\n\n" \
"  Create an HPSS file\n\n" \
"  Parameters:\n" \
"    path       Pathname of the file\n" \
"    mode       Create mode; same as creat(2) system call\n" \
"    hints      COS hints for file creation (type is hpss.cos_hints)\n" \
"    priorities COS hints priorities for file creation (type is hpss.cos_priorities)\n\n" \
"  Returns tuple containing file descriptor and cos hints"

#define DOC_Create \
"Create(path, mode=0777, hints=NULL, priorities=NULL) -> hpss.cos_hints\n\n" \
"  Create an HPSS file\n\n" \
"    path       Pathname of the file\n" \
"    mode       Create mode; same as creat(2) system call\n" \
"    hints      COS hints for file creation (type is hpss.cos_hints)\n" \
"    priorities COS hints priorities for file creation (type is hpss.cos_priorities)\n\n" \
"  Returns cos hints"

#define DOC_CreateHandle \
"CreateHandle(objhandle=NULL, path=NULL, mode=0777, ucred=NULL, hints=NULL, priorities=NULL) -> (hpss.cos_hints, hpss.vattr)\n\n" \
"  Create an HPSS file.\n\n" \
"  Parameters:\n" \
"    objhandle  Parent object handle\n" \
"    path       Path to open\n" \
"    mode       Create mode; same as open(2) system call\n" \
"    ucred      User credentials\n" \
"    hints      COS hints for file creation (type is hpss.cos_hints)\n" \
"    priorities COS hints priorities for file creation (type is hpss.cos_priorities)\n\n" \
"  Returns tuple containing cos hints and vattrs"

#define DOC_FileGetAttributes \
"FileGetAttributes(path) -> hpss.fileattr\n\n" \
"  Get attributes for a file\n\n" \
"  Returns file attributes as an hpss.fileattr"

#define DOC_FileGetXAttributes \
"FileGetXAttributes(path, flags, level) -> hpssclapi.xfileattr\n\n" \
"  Get attributes for a file\n\n" \
"  Returns file extended attributes as an hpssclapi.xfileattr"

#define DOC_FileGetAttributesHandle \
"FileGetAttributesHandle(objhandle, path=NULL, ucred=NULL) -> hpss.fileattr\n\n" \
"  Get attributes for a file\n\n" \
"  Parameters:\n" \
"    objhandle Object handle of parent\n" \
"    path      Path relative to parent object handle\n" \
"    ucred     User credentials\n\n" \
"  Returns file attributes as an hpss.fileattr"

#define DOC_Fclear \
"Fclear(fd, length)\n\n" \
"  Create a hole beginning at the current offset\n\n" \
"  Parameters:\n" \
"    fd     File descriptor\n" \
"    length Length of hole to create"

#define DOC_FclearOffset \
"FclearOffset(fd, offset, length)\n\n" \
"  Create a hole beginning at the specified offset\n\n" \
"  Parameters:\n" \
"    fd     File descriptor\n" \
"    offset Offset to start hole at\n" \
"    length Length of hole to create"

#define DOC_Fclose \
"Fclose(stream)\n\n" \
"  Close an hpss.FILE stream"

#define DOC_Fflush \
"Fflush(stream)\n\n" \
"  Flush an hpss.FILE stream"

#define DOC_Fgetc \
"Fgetc(stream) -> str\n\n" \
"  Read 1 byte from an hpss.FILE stream\n\n" \
"  Returns the byte as a str object, or None if EOF was reached"

#define DOC_Fgets \
"Fgets(size, stream) -> str\n\n" \
"  Read up to size-1 bytes from an hpss.FILE stream.\n" \
"  See fgets(3) for more detail\n\n" \
"  Parameters:\n" \
"    size   Maximum amount of data to read\n" \
"    stream The hpss.FILE stream to read from\n" \
"  Returns the data read as a str object"

#define DOC_Fopen \
"Fopen(path, mode) -> hpss.FILE\n\n" \
"  Open an hpss.FILE stream\n\n" \
"  Parameters:\n" \
"    path Path of file to open\n" \
"    mode Mode to open; same as fopen(3) system call\n\n" \
"  Returns an hpss.FILE stream"

#define DOC_Fpreallocate \
"Fpreallocate(fd, length)\n\n" \
"  Set the file length and preallocate storage"

#define DOC_Fread \
"Fread(size, stream) -> str\n\n" \
"  Read from an hpss.FILE stream\n\n" \
"  Parameters:\n" \
"    size   Number of bytes to read\n" \
"    stream hpss.FILE stream to read from\n\n" \
"  Returns the data read as a str object"

#define DOC_Fseek \
"Fseek(stream, offset, whence)\n\n" \
"  Reposition an HPSS_FILE stream\n\n" \
"  Parameters:\n" \
"    stream hpss.FILE stream to seek\n" \
"    offset Offset to seek to\n" \
"    whence What the offset is relative to; same as fseek(3) system call"

#define DOC_Ftell \
"Ftell(stream) -> int\n\n" \
"  Get the file position from an HPSS_FILE stream\n\n" \
"  Parameters:\n" \
"    stream hpss.FILE stream to get position of\n\n" \
"  Returns the stream position as an int object"

#define DOC_Ftruncate \
"Ftruncate(fd, length)\n\n" \
"  Set the file length"

#define DOC_Fwrite \
"Fwrite(buf, stream) -> int\n\n" \
"  Write to an HPSS_FILE stream\n\n" \
"  Parameters:\n" \
"    buf    Buffer to write\n" \
"    stream hpss.FILE stream to write to\n\n" \
"  Returns the number of bytes written"

#define DOC_Fstat \
"Fstat(fd) -> hpss.stat\n\n" \
"  Stat a file by file descriptor.\n\n" \
"  Returns the file statistics as an hpss.stat object"

#define DOC_Fsync \
"Fsync(fd)\n\n" \
"  Synchronize a file with storage\n\n" \
"  Parameters:\n" \
"    fd File descriptor"

#define DOC_GetAttrHandle \
"GetAttrHandle(objhandle, path=NULL, ucred=NULL) -> hpss.vattr\n\n" \
"  Get attributes for a file\n\n" \
"  Returns file attributes as an hpss.vattr"

#define DOC_GetConfiguration \
"GetConfiguration() -> hpss.api_config\n\n" \
"  Get the Client API configuration.\n\n" \
"  Returns the configuration as an hpss.api_config object"

#define DOC_GetObjId \
"GetObjId(objhandle) -> int\n\n" \
"  Get object id from an object handle"

#define DOC_GetObjType \
"GetObjType(objhandle) -> int\n\n" \
"  Get object type from an object handle"

#define DOC_GetPathHandleObjId \
"GetPathHandleObjId(objid, subsysid=1) -> (hpss.ns_ObjHandle, hpss.ns_ObjHandle, str)\n\n" \
"  Get object handle, fileset root handle, and path of object by object ID and subsys ID\n\n" \
"  Parameters:\n" \
"    objid    Object ID\n" \
"    subsysid Subsys ID\n\n" \
"  Returns tuple containing fileset root handle, object handle, and path"

#define DOC_Getcwd \
"Getcwd() -> str\n\n" \
"  Get current working directory\n\n" \
"  Returns current working directory as a string"

#define DOC_Getenv \
"Getenv(name) -> value\n\n" \
"  Get value from environment variable 'name'\n" \
"  This will lookup 'name' in the HPSS environment variables, then use the\n" \
"  system environment variables as a fallback. If 'name' cannot be found in\n" \
"  either, this will return None\n\n" \
"  Returns the value of the environment variable as a string"

#define DOC_GetLastHPSSErrno \
"GetLastHPSSErrno() -> hpss.errno_state\n\n" \
"  Get the last HPSS error number state\n\n" \
"  Returns the last HPSS error number state"

#define DOC_GetThreadUcred \
"GetThreadUcred() -> hpss_sec_cred_t\n\n" \
"  Get the user credentials for the currently running thread\n\n" \
"  Returns a structure of the HPSS API st_sec_cred_t type"

#define DOC_HandleCompare \
"HandleCompare(first, second) -> True/False\n\n" \
"  Compare two objhandles to see if they are for the same object"

#define DOC_HashAppend \
"HashAppend(hash, buffer)\n\n" \
"  Append a buffer to the hash stream. This is equivalent to HashAppendBuf\n" \
"  Parameters:\n" \
"    hash   The hpss.hash object to append to\n" \
"    buffer The buffer to append"

#define DOC_HashAppendBuf \
"HashAppendBuf(hash, buffer)\n\n" \
"  Append a buffer to the hash stream. This is equivalent to HashAppend\n" \
"  Parameters:\n" \
"    hash   The hpss.hash object to append to\n" \
"    buffer The buffer to append"

#define DOC_HashAppendStr \
"HashAppendBuf(hash, str)\n\n" \
"  Append a NULL-terminated string to the hash stream.\n" \
"  Parameters:\n" \
"    hash The hpss.hash object to append to\n" \
"    str  The string to append"

#define DOC_HashCreate \
"HashCreate(type) -> hpss.hash\n\n" \
"  Create an hpss.hash object\n\n" \
"  Parameters:\n" \
"    type The type of hash object to create. Can be any of:\n" \
"           hpss.HASH_ADLER32\n" \
"           hpss.HASH_CRC32\n" \
"           hpss.HASH_MD5\n" \
"           hpss.HASH_SHA1\n" \
"           hpss.HASH_SHA224\n" \
"           hpss.HASH_SHA256\n" \
"           hpss.HASH_SHA384\n" \
"           hpss.HASH_SHA512\n\n" \
"  Returns an hpss.hash object."

#define DOC_HashCreateAdler32 \
"HashCreateAdler32() -> hpss.hash\n\n" \
"  Create an Adler32 hpss.hash object."

#define DOC_HashCreateCrc32 \
"HashCreateCrc32() -> hpss.hash\n\n" \
"  Create an CRC32 hpss.hash object."

#define DOC_HashCreateMD5 \
"HashCreateMD5() -> hpss.hash\n\n" \
"  Create an MD5 hpss.hash object."

#define DOC_HashCreateSHA1 \
"HashCreateSHA1() -> hpss.hash\n\n" \
"  Create an SHA1 hpss.hash object."

#define DOC_HashCreateSHA224 \
"HashCreateSHA224() -> hpss.hash\n\n" \
"  Create an SHA224 hpss.hash object."

#define DOC_HashCreateSHA256 \
"HashCreateSHA256() -> hpss.hash\n\n" \
"  Create an SHA256 hpss.hash object."

#define DOC_HashCreateSHA384 \
"HashCreateSHA384() -> hpss.hash\n\n" \
"  Create an SHA384 hpss.hash object."

#define DOC_HashCreateSHA512 \
"HashCreateSHA512() -> hpss.hash\n\n" \
"  Create an SHA512 hpss.hash object."

#define DOC_HashDuplicate \
"HashDuplicate(hash) -> hpss.hash\n\n" \
"  Create a deep copy of an existing hpss.hash object,\n" \
"  maintaining the state of the original hash."

#define DOC_HashExtract \
"HashExtract(hash) -> (str, int)\n\n" \
"  Get the hash as a tuple of (str, int). These values can be\n" \
"  plugged into HashLoad() to create a new hash."

#define DOC_HashFinish \
"HashFinish(hash) -> str\n\n" \
"  Finalize appending to hash stream and get the actual hash\n" \
"  as a binary blob. The hash object will be reset and ready\n" \
"  to begin appending as a new hash stream."

#define DOC_HashFinishHex \
"HashFinishHex(hash) -> str\n\n" \
"  Finalize appending to hash stream and get the actual hash\n" \
"  as a hexadecimal string. The hash object will be reset and\n" \
"  ready to begin appending as a new hash stream."

#define DOC_HashLoad \
"HashLoad(buffer, type) -> hpss.hash\n\n" \
"  Create a new hash object from a buffer and type that was\n" \
"  returned from HashExtract."

#define DOC_HashReset \
"HashReset(hash)\n\n" \
"  Reset the hash stream to its pre-appended state."

#define DOC_Link \
"Link(oldpath, newpath)\n\n" \
"  Create a hard link to an existing HPSS file\n\n" \
"  Parameters:\n" \
"  oldpath Existing file to link to\n" \
"  newpath Path of new link"

#define DOC_LinkHandle \
"LinkHandle(objhandle, dirhandle, path, ucred)\n\n" \
"  Create a hard link to an existing HPSS file\n\n" \
"  Parameters:\n" \
"    objhandle Object handle of existing file to link to\n" \
"    dirhandle Parent object handle for new entry\n" \
"    path      Path name of new link\n" \
"    ucred     User credentials"

#define DOC_Lstat \
"Lstat(path) -> hpss.stat\n\n" \
"  Stat a file by pathname. If the path refers to a link, get the\n" \
"  statistics of the link itself.\n\n" \
"  Returns an hpss.stat object"

#define DOC_Migrate \
"Migrate(fd, level, flags) -> int\n\n" \
"  Migrate a file from a specified level in the storage hierarchy\n\n" \
"  Parameters:\n" \
"    fd    File descriptor\n" \
"    level Level in the hierarchy to migrate from\n" \
"    flags Controls behavior of the migration:\n" \
"            HPSS_MIGRATE_FORCE      migrate data even with copies at lower levels\n" \
"            HPSS_MIGRATE_PURGE_DATA purge data after migration\n" \
"            HPSS_MIGRATE_NO_COPY    do not migrate multiple copies\n\n" \
"  Returns number of bytes migrated"

#define DOC_Mkdir \
"Mkdir(path, mode)\n\n" \
"  Create a directory\n\n" \
"  Parameters:\n" \
"    path Path to create as a directory\n" \
"    mode       Create mode; same as open(2) system call"

#define DOC_MkdirHandle \
"MkdirHandle(objhandle, path, mode, ucred=NULL) -> (hpss.ns_ObjHandle, hpss.vattr)\n\n" \
"  Create a directory\n\n" \
"  Parameters:\n" \
"    objhandle Object handle of parent directory\n" \
"    path      Name to create in the parent directory\n" \
"    mode      Create mode; same as open(2) system call\n" \
"    ucred     User credentials\n\n" \
"  Returns a tuple containing object handle and vattrs of newly created directory"

#define DOC_Open \
"Open(path, oflags, mode=0777, hints=NULL, priorities=NULL) -> (int, hpss.cos_hints)\n\n" \
"  Open an HPSS file.\n\n" \
"  Parameters:\n" \
"    path       Pathname of the file\n" \
"    oflags     Open flags; same as open(2) system call\n" \
"    mode       Create mode; same as open(2) system call\n" \
"    hints      COS hints for file creation (type is hpss.cos_hints)\n" \
"    priorities COS hints priorities for file creation (type is hpss.cos_priorities)\n\n" \
"  Returns tuple containing file descriptor and cos hints"

#define DOC_OpenHandle \
"OpenHandle(objhandle=NULL, path=NULL, oflags=0, mode=0777, ucred=NULL, hints=NULL, priorities=NULL) -> (int, hpss.cos_hints, hpss.vattr)\n\n" \
"  Open an HPSS file.\n\n" \
"  Parameters:\n" \
"    objhandle  Parent object handle\n" \
"    path       Path to open\n" \
"    oflags     Open flags; same as open(2) system call\n" \
"    mode       Create mode; same as open(2) system call\n" \
"    ucred      User credentials\n" \
"    hints      COS hints for file creation (type is hpss.cos_hints)\n" \
"    priorities COS hints priorities for file creation (type is hpss.cos_priorities)\n\n" \
"  Returns tuple containing file descriptor, cos hints, and vattrs"

#define DOC_Opendir \
"Opendir(path) -> int\n\n" \
"  Open a directory by pathname.\n\n" \
"  Returns file descriptor as an int object"

#define DOC_ParseAuthString \
"ParseAuthString(auth_str, auth_mech) -> (int, str)\n\n" \
"  Parses an authentication string.\n\n" \
"  Parameters:\n" \
"    auth_str   Authenticator string\n" \
"    auth_mech  Authentication mechanism\n\n" \
"  Returns tuple with authentication type and authenticator string."

#define DOC_PIOEnd \
"PIOEnd(group)\n\n" \
"  End a PIO activity. The group is no longer valid after this"

#define DOC_PIOExecute \
"PIOExecute(fd, offset, length, group) -> (int, hpss.pio_gapinfo)\n\n" \
"  Execute a PIO transfer (coordinator thread)\n\n" \
"  Parameters:\n" \
"    fd      File descriptor\n" \
"    offset  File offset to begin transfer\n" \
"    length  Length of data to transfer\n" \
"    group   hpss.pio_grp data for this transfer\n\n" \
"  Returns tuple containing bytes moved and gapinfo"

#define DOC_PIOExportGrp \
"PIOExportGrp(group) -> str\n\n" \
"  Export an hpss.pio_grp\n\n" \
"  Returns a string object suitable for use in PIOImportGrp"

#define DOC_PIOImportGrp \
"PIOImportGrp(xport) -> hpss.pio_grp\n\n" \
"  Import an exported hpss.pio_grp\n\n" \
"  Returns an hpss.pio_grp object"

#define DOC_PIORegister \
"PIORegister(stripe, size, group, rw, callback, callback_arg=None, sockaddr=NULL)\n\n" \
"  Execute a PIO transfer (participant thread)\n\n" \
"  Parameters:\n" \
"    stripe       Unique stripe element among participant peers\n" \
"    size         Buffer size to use for transfers\n" \
"    group        hpss.pio_grp data for this transfer\n" \
"    rw           hpss.HPSS_PIO_READ or hpss.HPSS_PIO_WRITE\n" \
"    callback     Object which is called when a piece of the transfer is complete.\n" \
"                 It must have the following prototype:\n" \
"                 callback(arg, offset, data)\n" \
"    callback_arg Argument which will be passed as the first parameter to the callback.\n" \
"                 If no argument is provided, the callback will receive None.\n" \
"    sockaddr     hpss.sockaddr to pass into hpss_PIORegister.\n" \
"                 If no address is provided, NULL will be passed into hpss_PIORegister\n\n" \
"  Note that sockaddr must be passed in as a keyword argument"

#define DOC_PIOStart \
"PIOStart(pio_params) -> hpss.pio_grp\n\n" \
"  Start a PIO activity with the provided parameters.\n\n" \
"  Returns an hpss.pio_grp object"

#define DOC_Purge \
"Purge(fd, level) -> int\n\n" \
"  Purge a file by file descriptor from the specified hierarchy level\n\n" \
"  Returns number of bytes purged as an int object"

#define DOC_PurgeLock \
"PurgeLock(fd, flag)\n\n" \
"  Purge lock or unlock a file\n\n" \
"  flag must be one of the following:\n" \
"    hpss.PURGE_LOCK\n" \
"    hpss.PURGE_UNLOCK\n"

#define DOC_PurgeLoginCred \
"PurgeLoginCred()\n\n" \
"  Purge login credentials\n\n"

#define DOC_Read \
"Read(fd, size) -> str\n\n" \
"  Read 'size' bytes from a file\n\n" \
"  Returns the data read as a string object"

#define DOC_Readdir \
"Readdir(fd) -> hpss.dirent\n\n" \
"  Read the next entry in a directory\n\n" \
"  Returns the directory entry as an hpss.dirent object"

#define DOC_ReaddirHandle \
"ReaddirHandle(objhandle, offset, ucred=NULL, maxentries=1) -> ([hpss.dirent, ...], int, int)\n\n" \
"  Read directory entries\n\n" \
"  Parameters:\n" \
"    objhandle Object Handle of directory\n" \
"    offset    Directory offset to read\n" \
"    ucred     User credentials\n" \
"    maxentries Maximum number of directory entries to retrieve\n\n" \
"  Returns a tuple containing the list of directory entries, next offset, and whether you reached the end"

#define DOC_Readlink \
"Readlink(path) -> str\n\n" \
"  Read the value of a symbolic link\n\n" \
"  Returns the value of the symbolic link as a string"

#define DOC_ReadlinkHandle \
"ReadlinkHandle(objhandle, path=NULL, ucred=NULL) -> str\n\n" \
"  Read the value of a symbolic link\n\n" \
"  Returns the value of the symbolic link as a string"

#define DOC_Rename \
"Rename(oldpath, newpath)\n\n" \
"  Rename a file or directory"

#define DOC_RenameHandle \
"RenameHandle(oldhandle=NULL, oldpath=NULL, newhandle=NULL, newpath=NULL, ucred=NULL)\n\n" \
"  Rename a file or directory\n\n" \
"  Parameters:\n" \
"    oldhandle Current parent object handle\n" \
"    oldpath   Current path name relative to oldhandle\n" \
"    newhandle New parent object handle\n" \
"    newpath   New path name relative to newhandle\n" \
"    ucred     User credentials" \

#define DOC_Rewinddir \
"Rewinddir(fd)\n\n" \
"  Reset position of open directory stream to the beginning"

#define DOC_Rmdir \
"Rmdir(path)\n\n" \
"  Remove a directory"

#define DOC_RmdirHandle \
"RmdirHandle(objhandle, path=NULL, ucred=NULL)\n\n" \
"  Remove a directory\n\n" \
"  Parameters:\n" \
"    objhandle Parent directory object handle\n" \
"    path      Path name of directory to be removed\n" \
"    ucred     User credentials"

#define DOC_SECGetCredsByUid \
"SECGetCredsByUid(uid, realm=NULL) -> hpss.sec_cred\n\n" \
"  Acquire user credentials for the user specified\n\n" \
"  Returns credentials as an hpss.sec_cred object"

#define DOC_SECGetCredsByName \
"SECGetCredsByUid(user_name, realm=NULL) -> hpss.sec_cred\n\n" \
"  Acquire user credentials for the user specified\n\n" \
"  Returns credentials as an hpss.sec_cred object"

#define DOC_SECGetGroupNameById \
"SECGetGroupNameById(gid, realm=NULL)\n\n" \
"  Retrieve group name by group id\n\n" \
"  Returns the group name as a string"

#define DOC_SetAttrHandle \
"SetAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, flags=0, attr=NULL) -> (int, hpss.vattr)\n\n" \
"  Alter file attribute values.\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path name of entry to modify attributes for\n" \
"    ucred     User credentials\n" \
"    flags     Bitmask of attributes to modify\n" \
"    attr      New attribute values\n" \

#define DOC_SetConfiguration \
"SetConfiguration(config)\n\n" \
"  Set the Client API configuration with an hpss.api_config object"

#define DOC_SetFileOffset \
"SetFileOffset(fd, offset, whence, direction) -> int\n\n" \
"  Set the current file offset for an open file\n\n" \
"  Parameters:\n" \
"    fd        File descriptor\n" \
"    offset    Number of bytes you will seek\n" \
"    whence    Starting point flag\n" \
"                SEEK_SET\n" \
"                SEEK_CUR\n" \
"                SEEK_END\n" \
"    direction Direction of seek\n" \
"                HPSS_SET_OFFSET_FORWARD\n" \
"                HPSS_SET_OFFSET_BACKWARD\n" \
"  Returns the resulting offset"

#define DOC_SetLoginCred \
"SetLoginCred(princ, auth_mech, cred_type, auth_type, authenticator)\n\n" \
"  Set the login credentials\n\n" \
"  Parameters:\n" \
"    princ         HPSS principal\n" \
"    auth_mech     Authentication mechanism\n" \
"    cred_type     Credential type\n" \
"    auth_type     Authentication type\n" \
"    authenticator Authenticator string"

#define DOC_Stage \
"Stage(fd, offset, length, level, flags)\n\n" \
"  Stage a piece of a file to a specified level in the storage hierarchy\n\n" \
"  Parameters:\n" \
"    fd     File descriptor\n" \
"    offset Offset of start of data to be staged\n" \
"    length Length of data to be staged\n" \
"    level  Level to stage data to\n" \
"    flags  Controls behavior of stage request:\n" \
"             BFS_STAGE_ALL   Stage entire file\n" \
"             BFS_ASYNCH_CALL Return after initiating stage"

#define DOC_Stat \
"Stat(path) -> hpss.stat\n\n" \
"  Stat a file by pathname. If the path refers to a link, get the\n" \
"  statistics of the file it points to.\n\n" \
"  Returns the file statistics as an hpss.stat object"

#define DOC_Statfs \
"Statfs(cos) -> hpss.statfs\n\n" \
"  Get file system statistics\n\n" \
"  Parameters:\n" \
"    cos Class of Service to get statistics for\n\n" \
"  Returns statistics of COS as hpss.statfs"

#define DOC_Statvfs \
"Statvfs(cos) -> hpss.statvfs\n\n" \
"  Get file system statistics\n\n" \
"  Parameters:\n" \
"    cos Class of Service to get statistics for\n\n" \
"  Returns statistics of COS as hpss.statvfs"

#define DOC_Symlink \
"Symlink(contents, path)\n\n" \
"  Create a symbolic link\n\n" \
"  Parameters:\n" \
"    contents Contents for link to contain\n" \
"    path     Name of link to create"

#define DOC_SymlinkHandle \
"SymlinkHandle(objhandle=NULL, contents=NULL, path=NULL, ucred=NULL) -> hpss.vattr\n\n" \
"  Create a symbolic link\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    contents  Contents for link to contain\n" \
"    path      Name of link to create\n" \
"    ucred     User credentials\n\n" \
"  Returns attributes of new symbolic link"

#define DOC_Truncate \
"Truncate(path, length)\n\n" \
"  Set the length for a file\n\n" \
"  Parameters:\n" \
"    path   Path to file to truncate\n" \
"    length Length to set file to"

#define DOC_TruncateHandle \
"TruncateHandle(objhandle=NULL, path=NULL, length=0, ucred=NULL)\n\n" \
"  Set the length for a file\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to file to truncate\n" \
"    length    Length to set file to\n" \
"    ucred     User credentials"

#define DOC_Undelete \
"Undelete(path, flags)\n\n" \
"  Undelete a file from the trash\n\n" \
"  Parameters:\n" \
"    path   Name of the file\n" \
"    flags  Controls the behavior of the undelete:\n" \
"              overwrite - undelete over an existing file\n" \
"              restore   - restore timestamps from delete time\n" \
"              overwrite+restore - both overwrite and restore"

#define DOC_UndeleteHandle \
"UndeleteHandle(objhandle=NULL, path=NULL, ucred=NULL, flags=NULL)\n\n" \
"  Undelete a file from the trash\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Name of the file\n" \
"    flags     Controls the behavior of the undelete:\n" \
"                 overwrite - undelete over an existing file\n" \
"                 restore   - restore timestamps from delete time\n" \
"                 overwrite+restore - both overwrite and restore\n" \
"    ucred     User credentials"

#define DOC_Umask \
"Umask(mask) -> int\n\n" \
"  Set the file creation mask\n\n" \
"  Returns the previous mask"

#define DOC_Unlink \
"Unlink(path)\n\n" \
"  Unlink a file or symbolic link"

#define DOC_UnlinkHandle \
"UnlinkHandle(objhandle=NULL, path=NULL, ucred=NULL)\n\n" \
"  Unlink a file or symbolic link\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to name to unlink\n" \
"    ucred     User credentials"

#define DOC_UserAttrDeleteAttrs \
"UserAttrDeleteAttrs(path, keys, schema=NULL)\n\n" \
"  Delete user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    path   Path to namespace object to delete UDAs for\n" \
"    keys   List of keynames to delete\n" \
"    schema Optional XML schema name to use to validate the operation"

#define DOC_UserAttrDeleteAttrHandle \
"UserAttrDeleteAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, keys=NULL, schema=NULL)\n\n" \
"  Delete user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to namespace object to delete UDAs for\n" \
"    ucred     User credentials\n" \
"    keys      List of keynames to delete\n" \
"    schema    Optional XML schema name to use to validate the operation"

#define DOC_UserAttrGetAttrs \
"UserAttrGetAttrs(path, keys, flags=UDA_API_VALUE, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  Retrieve specified user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    path    Path to namespace object to retrieve UDAs for\n" \
"    keys    List of keynames to retrieve values for\n" \
"    flags   Value to determine format of return data\n" \
"              UDA_API_VALUE for text-only\n" \
"              UDA_API_XML   for XML\n" \
"    xmlsize Maximum XML length\n\n" \
"  Returns dictionary of the key-value pairs"

#define DOC_UserAttrGetAttrHandle \
"UserAttrGetAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, keys=NULL, flags=UDA_API_VALUE, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  Retrieve specified user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to namespace object to retrieve UDAs for\n" \
"    ucred     User credentials\n" \
"    keys      List of keynames to retrieve values for\n" \
"    flags     Value to determine format of return data\n" \
"                UDA_API_VALUE for text-only\n" \
"                UDA_API_XML   for XML\n" \
"    xmlsize   Maximum XML length\n\n" \
"  Returns dictionary of the key-value pairs"

#ifndef HPSS751
#define DOC_UserAttrLenGetAttrs \
"UserAttrLenGetAttrs(path, keys, flags=UDA_API_VALUE, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  A synonym for UserAttrGetAttrs"

#define DOC_UserAttrLenGetAttrHandle \
"UserAttrLenGetAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, keys=NULL, flags=UDA_API_VALUE, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  A synonym for UserAttrGetAttrHandle"

#define DOC_UserAttrLenListAttrs \
"UserAttrLenListAttrs(path, flags=XML_NO_ATTR, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  A synonym for UserAttrListAttrs"

#define DOC_UserAttrLenListAttrHandle \
"UserAttrLenListAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, flags=XML_NO_ATTR, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  A synonym for UserAttrLenListAttrHandle"

#endif

#define DOC_UserAttrListAttrs \
"UserAttrListAttrs(path, flags=XML_NO_ATTR, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  Retrieve all user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    path    Path to namespace object to retrieve UDAs for\n" \
"    flags   Whether to return XML element attributes\n" \
"            Can be either XML_ATTR or XML_NO_ATTR\n" \
"    xmlsize Maximum XML length\n\n" \
"  Returns a dictionary of the key-value pairs"

#define DOC_UserAttrListAttrHandle \
"UserAttrListAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, flags=XML_NO_ATTR, xmlsize=HPSS_XML_SIZE) -> { str: str, ... }\n\n" \
"  Retrieve all user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to namespace object to retrieve UDAs for\n" \
"    ucred     User credentials\n" \
"    flags     Whether to return XML element attributes\n" \
"              Can be either XML_ATTR or XML_NO_ATTR\n" \
"    xmlsize   Maximum XML length\n\n" \
"  Returns a dictionary of the key-value pairs"

#define DOC_UserAttrSetAttrs \
"UserAttrSetAttrs(path, attrs, schema=NULL)\n\n" \
"  Create or update user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    path   Path to namespace object to update UDAs for\n" \
"    attrs  Dictionary of { str: str, ... } keypairs to set\n" \
"    schema Optional XML schema name to use to validate the operation"

#define DOC_UserAttrSetAttrHandle \
"UserAttrSetAttrHandle(objhandle=NULL, path=NULL, ucred=NULL, attrs=NULL, schema=NULL)\n\n" \
"  Create or update user-defined attributes on an HPSS namespace object\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to namespace object to update UDAs for\n" \
"    ucred     User credentials\n" \
"    attrs     Dictionary of { str: str, ... } keypairs to set\n" \
"    schema    Optional XML schema name to use to validate the operation"


#define DOC_UserAttrXQueryGet \
"UserAttrXQueryGet(path, xquery) -> str\n\n" \
"  Get user-defined attributes on an HPSS namespace object using a user-defined XQuery\n\n" \
"  Parameters:\n" \
"    path   Path to namespace object to retrieve UDAs for\n" \
"    xquery User-defined XQuery to be executed\n\n" \
"  Returns XQuery result"

#define DOC_UserAttrXQueryGetHandle \
"UserAttrXQueryUpdateHandle(objhandle=NULL, path=NULL, ucred=NULL, xquery=NULL, schema=NULL) -> str\n\n" \
"  Get user-defined attributes on an HPSS namespace object using a user-defined XQuery\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to namespace object to retrieve UDAs for\n" \
"    ucred     User credentials\n" \
"    xquery    User-defined XQuery to be executed\n\n" \
"  Returns XQuery result"

#define DOC_UserAttrXQueryUpdate \
"UserAttrXQueryUpdate(path, xquery, schema=NULL)\n\n" \
"  Create or Update user-defined attributes on an HPSS namespace object using a user-defined XQuery\n\n" \
"  Parameters:\n" \
"    path   Path to namespace object to update UDAs for\n" \
"    xquery User-defined XQuery to be executed\n" \
"    schema Optional XML Schema to use to validate the operation"

#define DOC_UserAttrXQueryUpdateHandle \
"UserAttrXQueryUpdateHandle(objhandle=NULL, path=NULL, ucred=NULL, xquery=NULL, schema=NULL)\n\n" \
"  Create or Update user-defined attributes on an HPSS namespace object using a user-defined XQuery\n\n" \
"  Parameters:\n" \
"    objhandle Parent object handle\n" \
"    path      Path to namespace object to update UDAs for\n" \
"    ucred     User credentials\n" \
"    xquery    User-defined XQuery to be executed\n" \
"    schema    Optional XML Schema to use to validate the operation"

#define DOC_Write \
"Write(fd, buf) -> int\n\n" \
"  Write 'buf' to a file\n\n" \
"  Returns number of bytes written as an int object"

#define DOC_net_accept \
"net_accept(sockfd) -> (int, hpss.sockaddr)\n\n" \
"  Accept a remote connection from sockfd\n\n" \
"  Returns a tuple containing new socket and a sockaddr of the remote peer"

#define DOC_net_addrmatch \
"net_addrmatch(addr, entry, mask=NULL) -> True/False\n\n" \
"  Compare two addresses with an optional mask"

#define DOC_net_bind \
"net_bind(sockfd, addr)\n\n" \
"  Bind a socket to an address"

#define DOC_net_connect \
"net_connect(sockfd, addr)\n\n" \
"  Connect a socket to a remote address"

#define DOC_net_generatemask \
"net_generatemask(addr, bits) -> hpss.sockaddr\n\n" \
"  Generate a netmask based on a template address and a number of bits"

#define DOC_net_get_family_option \
"net_get_family_option() -> int\n\n" \
"  Get the value of the net family option"

#define DOC_net_get_family_string \
"net_get_family_string() -> str\n\n" \
"  Get a string representation of the net family option"

#define DOC_net_getaddrinfo \
"net_getaddrinfo(hostname=NULL, service=NULL, flags=0, protocol=HPSS_IPPROTO_UNSPECIFIED) -> hpss.sockaddr\n\n" \
"  Get address info\n\n" \
"  Parameters:\n" \
"    hostname Hostname or IP address\n" \
"    service  Service name or port number\n" \
"    flags    AI flags\n" \
"    protocol IP Protocol. Must be one of the following:\n" \
"               HPSS_IPPROTO_UNSPECIFIED\n" \
"               HPSS_IPPROTO_TCP\n" \
"               HPSS_IPPROTO_UDP\n\n" \
"  Returns a socket address as an hpss.sockaddr object"

#define DOC_net_getaddrinfo_ex \
"net_getaddrinfo_ex(hostname=NULL, service=NULL, flags=0, protocol=HPSS_IPPROTO_UNSPECIFIED) -> ([hpss.sockaddr, ...], str)\n\n" \
"  Get address info\n\n" \
"  Parameters:\n" \
"    hostname Hostname or IP address\n" \
"    service  Service name or port number\n" \
"    flags    AI flags\n" \
"    protocol IP Protocol. Must be one of the following:\n" \
"               HPSS_IPPROTO_UNSPECIFIED\n" \
"               HPSS_IPPROTO_TCP\n" \
"               HPSS_IPPROTO_UDP\n\n" \
"  Returns a tuple containing a list of sockaddrs and the canonical hostname"

#define DOC_net_getinaddr \
"getinaddr(addr) -> (str, int)\n\n" \
"  Get the internal sin6_addr or sin_addr for an address\n\n" \
"  Returns a tuple containing the internal sin6_addr or sin_addr and the family"

#define DOC_net_getnameinfo \
"getnameinfo(addr) -> (str, str)\n\n" \
"  Get the hostname and service of an address"

#define DOC_net_getpeername \
"getpeername(sockfd) -> hpss.sockaddr\n\n" \
"  Get the remote address associated with a socket"

#define DOC_net_getport \
"getport(addr) -> int\n\n" \
"  Get the port associated with an address"

#define DOC_net_getsockname \
"getpeername(sockfd) -> hpss.sockaddr\n\n" \
"  Get the local address associated with a socket"

#define DOC_net_getuniversaladdress \
"getuniversaladdress(addr) -> str\n\n" \
"  Get a string representation of an address suitable for RPC"

#define DOC_net_listen \
"listen(sockfd, backlog)\n\n" \
"  Set a socket to accept incoming connections"

#define DOC_net_setport \
"setport(addr, port)\n\n" \
"  Set the port associated with an address"

#define DOC_net_socket \
"socket(addr, type, protocol) -> int\n\n" \
"  Create a socket associated with the address, type, and protocol"

#define DOC_net_universaladdresstoport \
"universaladdresstoport(address) -> int\n\n" \
"  Get the port from a universal address"
