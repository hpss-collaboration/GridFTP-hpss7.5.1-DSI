/*
 *  begin_generated_IBM_prolog
 *
 *  This is an automatically generated prolog
 *  ------------------------------------------------------------------
 *  Product(s): HPSS Data Storage Interface
 *
 *  Licensed Materials
 *
 *  Copyright (C) 2016, 2017
 *  International Business Machines Corporation, The Regents of the
 *  University of California, Los Alamos National Security, LLC,
 *  Lawrence Livermore National Security, LLC, Sandia Corporation,
 *  and UT-Battelle.
 *
 *  All rights reserved.
 *
 *  Portions of this work were produced by Lawrence Livermore National
 *  Security, LLC, Lawrence Livermore National Laboratory (LLNL)
 *  under Contract No. DE-AC52-07NA27344 with the U.S. Department
 *  of Energy (DOE); by the University of California, Lawrence
 *  Berkeley National Laboratory (LBNL) under Contract
 *  No. DE-AC02-05CH11231 with DOE; by Los Alamos National
 *  Security, LLC, Los Alamos National Laboratory (LANL) under
 *  Contract No. DE-AC52-06NA25396 with DOE; by Sandia Corporation,
 *  Sandia National Laboratories (SNL) under Contract
 *  No. DE-AC04-94AL85000 with DOE; and by UT-Battelle, Oak Ridge
 *  National Laboratory (ORNL) under Contract No. DE-AC05-00OR22725
 *  with DOE.  The U.S. Government has certain reserved rights under
 *  its prime contracts with the Laboratories.
 *
 *                            DISCLAIMER
 *  Portions of this software were sponsored by an agency of the
 *  United States Government.  Neither the United States, DOE, The
 *  Regents of the University of California, Los Alamos National
 *  Security, LLC, Lawrence Livermore National Security, LLC, Sandia
 *  Corporation, UT-Battelle, nor any of their employees, makes any
 *  warranty, express or implied, or assumes any liability or
 *  responsibility for the accuracy, completeness, or usefulness of
 *  any information, apparatus, product, or process disclosed, or
 *  represents that its use would not infringe privately owned rights.
 *
 *  High Performance Storage System is a trademark of International
 *  Business Machines Corporation.
 *  ------------------------------------------------------------------
 *
 *  end_generated_IBM_prolog
 * University of Illinois/NCSA Open Source License
 *
 * Copyright © 2015 NCSA.  All rights reserved.
 *
 * Developed by:
 *
 * Storage Enabling Technologies (SET)
 *
 * Nation Center for Supercomputing Applications (NCSA)
 *
 * http://www.ncsa.illinois.edu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the .Software.),
 * to deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *    + Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *
 *    + Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimers in the
 *      documentation and/or other materials provided with the distribution.
 *
 *    + Neither the names of SET, NCSA
 *      nor the names of its contributors may be used to endorse or promote
 *      products derived from this Software without specific prior written
 *      permission.
 *
 * THE SOFTWARE IS PROVIDED .AS IS., WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 */

/*
 * System includes
 */
#include <assert.h>

/*
 * HPSS includes
 */
#include <hpss_api.h>
#include <hpss_xml.h>

/*
 * Local includes
 */
#include "cksm.h"
#include "stat.h"
#include "pio.h"

int
cksm_pio_callout(char     * Buffer,
                 uint32_t * Length,
                 uint64_t   Offset,
                 void     * CallbackArg);

void
cksm_transfer_complete_callback(globus_result_t Result,
                                void          * UserArg);

static globus_result_t
algorithm_from_string(const char         * Algorithm,
                      hpss_hash_type_t   * Type)
{
    /*
     *  At first opportunity this should be changed to used the
     *  hash type enums
     */
    if(!strcasecmp(Algorithm,"SHA1") || !strcasecmp(Algorithm,"SHA"))
        *Type = HASH_SHA1;
    else if(!strcasecmp(Algorithm,"SHA224"))
        *Type = HASH_SHA224;
    else if(!strcasecmp(Algorithm,"SHA256"))
        *Type = HASH_SHA256;
    else if(!strcasecmp(Algorithm,"SHA384"))
        *Type = HASH_SHA384;
    else if(!strcasecmp(Algorithm,"SHA512"))
        *Type = HASH_SHA512;
    else if(!strcasecmp(Algorithm,"MD5"))
        *Type  = HASH_MD5;
    else if(!strcasecmp(Algorithm,"CRC32"))
        *Type = HASH_CRC32;
    else if(!strcasecmp(Algorithm,"ADLER32"))
        *Type = HASH_ADLER32;
    else
        return(GLOBUS_FAILURE);

    return(GLOBUS_SUCCESS);
}

static globus_result_t
digest_length(hpss_hash_type_t     Type,
              unsigned int       * Length)
{
    globus_result_t result = GLOBUS_SUCCESS;
    int             retval = 0;

    GlobusGFSName(digest_length);

    retval = hpss_HashDigestLength(Type);
    if(retval < 0)
    {
        result = GlobusGFSErrorSystemError("hpss_HashDigestLength", -retval);
    }
    else
    {
        *Length = retval;
    }
    return result;
}

static inline int char_to_val(const char cval)
{
    if(toupper(cval) >= 'A' && toupper(cval) <= 'F')
        return((toupper(cval) - 'A') + 10);
    else if(toupper(cval) >= '0' && toupper(cval) <= '9')
        return(toupper(cval) - '0');
    return(-1);
}

//Converts hex string to byte array digest
static globus_result_t
digest_from_string(const char         * String,
                   char               * Buffer,
                   unsigned int         Length)
{
    unsigned int     ii, len;

    GlobusGFSName(digest_from_string);

    if((String == NULL) || ((len = strlen(String)) % 2) ||
            (len == 0) || ((len / 2) != Length))
        return(GLOBUS_FAILURE);

    for(ii=0; ii < len; ii++) {
        int v;
        if((ii%2) == 0) {
            if((v = char_to_val(String[ii])) == -1) return(GLOBUS_FAILURE);
            *Buffer = (v & 0xF);
        }
        else {
            if((v = char_to_val(String[ii])) == -1) return(GLOBUS_FAILURE);
            *Buffer <<= 4;
            *Buffer |= (v & 0xF);
            Buffer++;
        }
    }

    return(GLOBUS_SUCCESS);
}

//Converts byte array digest to hex string
static char *
digest_to_string(char          * Buffer,
                 unsigned int    Length)
{
    static char   *hexchars = "0123456789abcdef";
    char          *ret = NULL;

    GlobusGFSName(digest_to_string);
    ret = globus_malloc(Length * 2 + 1);
    if (ret != NULL) {
        char *out = ret;
        char *in = Buffer;
        while (Length-- > 0)
        {
            char c = *in++;
            *out++ = hexchars[(c >> 4) & 0xf];
            *out++ = hexchars[(c) & 0xf];
        }
        *out = '\0';
    }
    return(ret);
}

static globus_result_t
set_digest_info(const char              * Algorithm,
                const char              * ChecksumString,
                hpss_file_hash_digest_t * DigestInfo)
{
    static const char       *creator      = "GRIDFTP";
    globus_result_t         result        = GLOBUS_SUCCESS;
    unsigned int            digest_len    = 0;
    GlobusGFSName(set_digest_info);

    memset(DigestInfo,'\0',sizeof(*DigestInfo));

    result = algorithm_from_string(Algorithm, &DigestInfo->Type);
    if(result != GLOBUS_SUCCESS)
        return result;

    result = digest_length(DigestInfo->Type, &digest_len);
    if(result != GLOBUS_SUCCESS)
        return result;

    result = digest_from_string(ChecksumString, DigestInfo->Buffer, digest_len);
    if(result != GLOBUS_SUCCESS)
        return result;

    DigestInfo->Flags = HPSS_FILE_HASH_DIGEST_VALID;
    sprintf(DigestInfo->Creator,creator);

    return GLOBUS_SUCCESS;
}


void
cksm_update_markers(cksm_marker_t * Marker, globus_off_t Bytes)
{
    GlobusGFSName(cksm_update_markers);
    if (Marker)
    {
        pthread_mutex_lock(&Marker->Lock);
        {
            Marker->TotalBytes += Bytes;
        }
        pthread_mutex_unlock(&Marker->Lock);
    }
}

void
cksm_send_markers(void * UserArg)
{
    GlobusGFSName(cksm_send_markers);

    cksm_marker_t * marker = (cksm_marker_t *) UserArg;
    char            total_bytes_string[128];

    pthread_mutex_lock(&marker->Lock);
    {
        /* Convert the byte count to a string. */
        sprintf(total_bytes_string, "%"GLOBUS_OFF_T_FORMAT, marker->TotalBytes);

        /* Send the intermediate response. */
        globus_gridftp_server_intermediate_command(marker->Operation,
                GLOBUS_SUCCESS,
                total_bytes_string);
    }
    pthread_mutex_unlock(&marker->Lock);
}

globus_result_t
cksm_start_markers(cksm_marker_t ** Marker, globus_gfs_operation_t Operation)
{
    int marker_freq = 0;
    globus_reltime_t delay;
    globus_result_t result = GLOBUS_SUCCESS;

    GlobusGFSName(cksm_start_markers);

    /* Get the frequency for maker updates. */
    globus_gridftp_server_get_update_interval(Operation, &marker_freq);

    if (marker_freq > 0)
    {
        *Marker = malloc(sizeof(cksm_marker_t));
        if (!*Marker)
            return GlobusGFSErrorMemory("cksm_marker_t");

        pthread_mutex_init(&(*Marker)->Lock, NULL);
        (*Marker)->TotalBytes = 0;
        (*Marker)->Operation  = Operation;

        /* Setup the periodic callback. */
        GlobusTimeReltimeSet(delay, marker_freq, 0);
        result = globus_callback_register_periodic(&(*Marker)->CallbackHandle,
                &delay,
                &delay,
                cksm_send_markers,
                (*Marker));
        if (result)
        {
            free(*Marker);
            *Marker = NULL;
        }
    }

    return result;
}

void
cksm_stop_markers(cksm_marker_t * Marker)
{
    GlobusGFSName(cksm_stop_markers);
    if (Marker)
    {
        globus_callback_unregister(Marker->CallbackHandle, NULL, NULL, NULL);
        pthread_mutex_destroy(&Marker->Lock);
        free(Marker);
    }
}

globus_result_t
cksm_open_for_reading(char * Pathname,
        int  * FileFD,
        int  * FileStripeWidth)
{
    hpss_cos_hints_t      hints_in;
    hpss_cos_hints_t      hints_out;
    hpss_cos_priorities_t priorities;

    GlobusGFSName(cksm_open_for_reading);

    *FileFD = -1;

    /* Initialize the hints in. */
    memset(&hints_in, 0, sizeof(hpss_cos_hints_t));

    /* Initialize the hints out. */
    memset(&hints_out, 0, sizeof(hpss_cos_hints_t));

    /* Initialize the priorities. */
    memset(&priorities, 0, sizeof(hpss_cos_priorities_t));



    /* Open the HPSS file. */
    *FileFD = hpss_Open(Pathname,
            O_RDWR|O_NDELAY|O_NONBLOCK,
            S_IRUSR|S_IWUSR,
            &hints_in,
            &priorities,
            &hints_out);
    if (*FileFD < 0)
        return GlobusGFSErrorSystemError("hpss_Open", -(*FileFD));

    int rc = hpss_Stage(*FileFD, 0, cast64m(0), 0, BFS_STAGE_ALL);
    if (rc != 0)
    {
        GlobusGFSErrorSystemError("Failed to stage", -rc);
        return GLOBUS_FAILURE;
    }

    /* Copy out the file stripe width. */
    *FileStripeWidth = hints_out.StripeWidth;

    return GLOBUS_SUCCESS;
}
int
cksm_pio_callout(char     * Buffer,
        uint32_t * Length,
        uint64_t   Offset,
        void     * CallbackArg)
{
    int           rc        = 0;
    cksm_info_t * cksm_info = CallbackArg;

    GlobusGFSName(cksm_pio_callout);

    assert(*Length <= cksm_info->BlockSize);
    rc = hpss_HashAppendBuf(cksm_info->HashContext, (unsigned char *)Buffer, *Length);
    if (rc != 0)
    {
        cksm_info->Result = GlobusGFSErrorSystemError("hpss_HashAppendBuf failed", -rc);
        return cksm_info->Result;
    }

    cksm_update_markers(cksm_info->Marker, *Length);

    return 0;
}

void
cksm_range_complete_callback(globus_off_t * Offset,
        globus_off_t * Length,
        int          * Eot,
        void         * UserArg)
{
    GlobusGFSName(cksm_range_complete_callback);

    cksm_info_t * cksm_info = UserArg;

    *Offset                += *Length;
    cksm_info->RangeLength -= *Length;
    *Length                 = cksm_info->RangeLength;

    if (*Length == 0)
        *Eot = 1;
}

static globus_result_t
hpss_get_checksum(char * Pathname, const char *Algorithm,
                  char ** ChecksumString)
{
    extern ns_ObjHandle_t   *API_RootNSHandle;
    int                     retval        = 0;
    globus_result_t         result        = GLOBUS_SUCCESS;
    unsigned int            digest_len    = 0;
    hpss_file_hash_digest_t digest_info;
    hpss_hash_type_t        type;


    GlobusGFSName(hpss_get_checksum);

    memset(&digest_info,'\0',sizeof(digest_info));
    retval = hpss_FileGetDigestHandle(API_RootNSHandle,
            Pathname,
            NULL,
            &digest_info);

    if(retval != HPSS_E_NOERROR)
    {
        result = GlobusGFSErrorSystemError("hpss_FileGetDigestHandle", -(retval));
        return result;
    }

    if(digest_info.Type == hpss_hash_type_none)
        /* No hash record */
        return result;

    if((digest_info.Flags &  HPSS_FILE_HASH_DIGEST_VALID) == 0)
        /* Valid record but, no valid hash value */
        return result;

    result = algorithm_from_string(Algorithm, &type);
    if(result != GLOBUS_SUCCESS)
        return result;

    result = digest_length(digest_info.Type, &digest_len);
    if(result != GLOBUS_SUCCESS)
    {
        return result;
    }

    *ChecksumString = digest_to_string(digest_info.Buffer, digest_len);
    if((ChecksumString) == NULL)
    {
        result = GlobusGFSErrorMemory("ChecksumString");
        return result;
    }

    return result;
}


static globus_result_t
uda_get_checksum(char * Pathname, const char * Algorithm,
                 char ** ChecksumString)
{
    int                  retval = 0;
    char               * tmp    = NULL;
    char                 value[HPSS_XML_SIZE];
    char                 state[HPSS_XML_SIZE];
    char                 algorithm[HPSS_XML_SIZE];
    char                 checksum[HPSS_XML_SIZE];
    hpss_userattr_t      user_attrs[3];
    hpss_userattr_list_t attr_list;

    GlobusGFSName(uda_get_checksum);

    *ChecksumString = NULL;
    attr_list.len  = sizeof(user_attrs)/sizeof(*user_attrs);
    attr_list.Pair = user_attrs;

    attr_list.Pair[0].Key   = "/hpss/user/cksum/algorithm";
    attr_list.Pair[0].Value = algorithm;
    attr_list.Pair[1].Key   = "/hpss/user/cksum/checksum";
    attr_list.Pair[1].Value = checksum;
    attr_list.Pair[2].Key   = "/hpss/user/cksum/state";
    attr_list.Pair[2].Value = state;

    retval = hpss_UserAttrGetAttrs(Pathname, &attr_list, UDA_API_VALUE, HPSS_XML_SIZE-1);

    switch (retval)
    {
        case 0:
            break;
        case -ENOENT:
            return GLOBUS_SUCCESS;
        default:
            return GlobusGFSErrorSystemError("hpss_UserAttrGetAttrs", -retval);
    }

    tmp = hpss_ChompXMLHeader(attr_list.Pair[0].Value,  NULL);
    if (!tmp)
        return GLOBUS_SUCCESS;

    strcpy(value, tmp);
    free(tmp);
    tmp = hpss_ChompXMLHeader(attr_list.Pair[2].Value, NULL);
    if (!tmp)
        return GLOBUS_SUCCESS;

    strcpy(value, tmp);
    free(tmp);

    if (strcmp(value, "Valid") != 0)
        return GLOBUS_SUCCESS;

    *ChecksumString = hpss_ChompXMLHeader(attr_list.Pair[1].Value, NULL);
    return GLOBUS_SUCCESS;
}

void
cksm(globus_gfs_operation_t      Operation,
     globus_gfs_command_info_t * CommandInfo,
     config_t                  * Config,
     commands_callback           Callback)
{
    globus_result_t result            = GLOBUS_SUCCESS;
    cksm_info_t   * cksm_info         = NULL;
    int             rc                = 0;
    int             file_stripe_width = 0;
    char          * checksum_string   = NULL;
    hpss_stat_t     hpss_stat_buf;

    GlobusGFSName(cksm);
    rc = hpss_Stat(CommandInfo->pathname, &hpss_stat_buf);
    if (rc)
    {
        result = GlobusGFSErrorSystemError("hpss_Stat", -rc);
        Callback(Operation, result, NULL);
        return;
    }

    cksm_info = calloc(1, sizeof(cksm_info_t));
    if (!cksm_info)
    {
        result = GlobusGFSErrorMemory("cksm_info_t");
        goto cleanup;
    }

    /* Determine the requested checksum type */
    result = algorithm_from_string(CommandInfo->cksm_alg,
            &cksm_info->HashType);

    if(result != GLOBUS_SUCCESS)
    {
        goto cleanup;
    }

    cksm_info->Operation   = Operation;
    cksm_info->CommandInfo = CommandInfo;
    cksm_info->Callback    = Callback;
    cksm_info->Config      = Config;
    cksm_info->FileFD      = -1;
    cksm_info->Pathname    = strdup(CommandInfo->pathname);
    cksm_info->RangeLength = CommandInfo->cksm_length;
    if (cksm_info->RangeLength == -1)
        cksm_info->RangeLength  = hpss_stat_buf.st_size - CommandInfo->cksm_offset;

    /*Allocate a hash context*/
    cksm_info->HashContext = hpss_HashCreate(cksm_info->HashType);
    if (cksm_info->HashContext == GLOBUS_NULL)
    {
        result = GlobusGFSErrorMemory("hpss_hash_t");
        goto cleanup;
    }

    globus_gridftp_server_get_block_size(Operation, &cksm_info->BlockSize);

    /*
     * Open the file.
     */
    result = cksm_open_for_reading(CommandInfo->pathname,
            &cksm_info->FileFD,
            &file_stripe_width);
    if (result) goto cleanup;

    result = cksm_start_markers(&cksm_info->Marker, Operation);
    if (result) goto cleanup;


    /*
     * Setup PIO
     */
    result = pio_start(HPSS_PIO_READ,
            cksm_info->FileFD,
            file_stripe_width,
            cksm_info->BlockSize,
            CommandInfo->cksm_offset,
            cksm_info->RangeLength,
            cksm_pio_callout,
            cksm_range_complete_callback,
            cksm_transfer_complete_callback,
            cksm_info);

cleanup:
    if (result)
    {
        if (cksm_info)
        {
            if (cksm_info->FileFD != -1)
                hpss_Close(cksm_info->FileFD);
            if (cksm_info->Pathname)
                free(cksm_info->Pathname);
            free(cksm_info);
        }
        Callback(Operation, result, NULL);
    }
}

globus_result_t
hpss_set_checksum(char * Pathname, int FD, const char * Algorithm, char * ChecksumString)
{
    int                     retval        = 0;
    globus_result_t         result        = GLOBUS_SUCCESS;
    hpss_file_hash_digest_t digest_info;

    char          * old_checksum_string   = NULL;


    GlobusGFSName(hpss_set_checksum);

    hpss_get_checksum(Pathname, Algorithm, &old_checksum_string);
    if (old_checksum_string != NULL){
        return GLOBUS_SUCCESS;
    }

    result = set_digest_info(Algorithm,
            ChecksumString,
            &digest_info);

    if(result != GLOBUS_SUCCESS)
        return result;

    retval = hpss_FsetFileDigest(FD, &digest_info);

    if(retval != HPSS_E_NOERROR)
    {
        result = GlobusGFSErrorSystemError("hpss_FsetFileDigest", -(retval));
        return result;
    }
    return GLOBUS_SUCCESS;


}

/*
 * /hpss/user/cksum/algorithm                                  md5
 * /hpss/user/cksum/checksum               93b885adfe0da089cdf634904fd59f71
 * /hpss/user/cksum/lastupdate                          1376424299
 * /hpss/user/cksum/errors                                       0
 * /hpss/user/cksum/state                                    Valid
 * /hpss/user/cksum/app                                    hpsssum
 * /hpss/user/cksum/filesize                                     1
 */

globus_result_t
uda_set_checksum(char * Pathname, const char * algorithm, char * Checksum)
{
    int                  retval = 0;
    char                 filesize_buf[32];
    char                 lastupdate_buf[32];
    globus_result_t      result   = GLOBUS_SUCCESS;
    hpss_userattr_t      user_attrs[7];
    hpss_userattr_list_t attr_list;
    globus_gfs_stat_t    gfs_stat;

    char *               old_checksum = NULL;

    GlobusGFSName(uda_set_checksum);

    result = stat_object(Pathname, &gfs_stat);
    if (result != GLOBUS_SUCCESS)
        return result;

    uda_get_checksum(Pathname, algorithm, &old_checksum);
    if (old_checksum != NULL)
        return GLOBUS_SUCCESS;

    snprintf(filesize_buf, sizeof(filesize_buf), "%lu", gfs_stat.size);
    snprintf(lastupdate_buf, sizeof(lastupdate_buf), "%lu", time(NULL));
    stat_destroy(&gfs_stat);

    attr_list.len  = sizeof(user_attrs)/sizeof(*user_attrs);
    attr_list.Pair = user_attrs;

    attr_list.Pair[0].Key   = "/hpss/user/cksum/algorithm";
    attr_list.Pair[0].Value = (char *) algorithm;
    attr_list.Pair[1].Key   = "/hpss/user/cksum/checksum";
    attr_list.Pair[1].Value = Checksum;
    attr_list.Pair[2].Key   = "/hpss/user/cksum/lastupdate";
    attr_list.Pair[2].Value = lastupdate_buf;
    attr_list.Pair[3].Key   = "/hpss/user/cksum/errors";
    attr_list.Pair[3].Value = "0";
    attr_list.Pair[4].Key   = "/hpss/user/cksum/state";
    attr_list.Pair[4].Value = "Valid";
    attr_list.Pair[5].Key   = "/hpss/user/cksum/app";
    attr_list.Pair[5].Value = "GridFTP";
    attr_list.Pair[6].Key   = "/hpss/user/cksum/filesize";
    attr_list.Pair[6].Value = filesize_buf;

    retval = hpss_UserAttrSetAttrs(Pathname, &attr_list, NULL);
    if (retval)
        return GlobusGFSErrorSystemError("hpss_UserAttrSetAttrs", -retval);

    return result;
}

globus_result_t
cksm_set_checksum(cksm_info_t * cksm_info, const char * Algorithm, char * Checksum)
{
    globus_result_t         result      = GLOBUS_SUCCESS;

    GlobusGFSName(cksm_set_checksum);

    if (cksm_info->Config->ChecksumSupport)
        result = hpss_set_checksum(cksm_info->Pathname, cksm_info->FileFD,
                                   Algorithm, Checksum);

    if (cksm_info->Config->UDAChecksumSupport)
        result = uda_set_checksum(cksm_info->Pathname, Algorithm, Checksum);

    return result;
}

globus_result_t
cksm_clear_checksum(char * Pathname, config_t * Config)
{
    if (Config->UDAChecksumSupport)
    {
	    int                  retval = 0;
	    hpss_userattr_list_t attr_list;

	    GlobusGFSName(checksum_clear_checksum);
		attr_list.len  = 7;
		attr_list.Pair = malloc(attr_list.len * sizeof(hpss_userattr_t));

        attr_list.Pair[0].Key   = "/hpss/user/cksum/algorithm";
        attr_list.Pair[1].Key   = "/hpss/user/cksum/checksum";
        attr_list.Pair[2].Key   = "/hpss/user/cksum/lastupdate";
        attr_list.Pair[3].Key   = "/hpss/user/cksum/errors";
        attr_list.Pair[4].Key   = "/hpss/user/cksum/state";
        attr_list.Pair[5].Key   = "/hpss/user/cksum/app";
        attr_list.Pair[6].Key   = "/hpss/user/cksum/filesize";

		retval = hpss_UserAttrDeleteAttrs(Pathname, &attr_list, NULL);
        free(attr_list.Pair);
		if (retval && retval != -ENOENT)
			return GlobusGFSErrorSystemError("hpss_UserAttrDeleteAttrs", -retval);
	}
    return GLOBUS_SUCCESS;
}

void
cksm_transfer_complete_callback(globus_result_t Result, void * UserArg)
{
    globus_result_t result    = Result;
    cksm_info_t   * cksm_info = UserArg;
    int             rc        = 0;

    char          * checksum_string = NULL;

    GlobusGFSName(cksm_transfer_complete_callback);

    /* Give our error priority. */
    if (cksm_info->Result)
        result = cksm_info->Result;


    if (!result)
    {
        checksum_string = hpss_HashFinishHex(cksm_info->HashContext);
    }

    cksm_stop_markers(cksm_info->Marker);

    if (!result && cksm_info->CommandInfo->cksm_offset == 0 && cksm_info->CommandInfo->cksm_length == -1)
        cksm_set_checksum(cksm_info, (const char *)cksm_info->CommandInfo->cksm_alg, checksum_string);

    if (cksm_info->FileFD != -1)
    {
        rc = hpss_Close(cksm_info->FileFD);
        if (rc && !result)
            result = GlobusGFSErrorSystemError("hpss_Close", -rc);
    }

    cksm_info->Callback(cksm_info->Operation, result, result ? NULL : checksum_string);

    hpss_HashDelete(cksm_info->HashContext);
    free(cksm_info->Pathname);
    free(cksm_info);
    free(checksum_string);
}


