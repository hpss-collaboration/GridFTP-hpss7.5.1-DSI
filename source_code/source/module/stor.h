/*
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
#ifndef HPSS_DSI_STOR_H
#define HPSS_DSI_STOR_H

/*
 * System includes
 */
#include <pthread.h>

/*
 * Globus includes
 */
#include <globus_gridftp_server.h>
#include <globus_list.h>

/*
 * Local includes
 */
#include "config.h"
#include "pio.h"

/*
 * Because of the sequential, ascending nature of offsets with PIO,
 * we do not need a range list.
 */
struct stor_info;

typedef struct {
	char             * Buffer;
	globus_off_t       BufferOffset;   // Moves as buffer is consumed
	globus_off_t       TransferOffset; // Moves as BufferOffset moves
	globus_off_t       BufferLength;   // Moves as BufferOffset moves
	struct stor_info * StorInfo;
} stor_buffer_t;

typedef struct stor_info {
	globus_gfs_operation_t       Operation;
	globus_gfs_transfer_info_t * TransferInfo;

	int FileFD;

	globus_result_t Result;
	globus_size_t   BlockSize;

	pthread_mutex_t Mutex;
	pthread_cond_t  Cond;

	globus_off_t    RangeLength; // Current range transfer length
	globus_bool_t   Eof;

	int OptConnCnt;
	int ConnChkCnt;
	int CurConnCnt;

	globus_list_t * AllBufferList;
	globus_list_t * ReadyBufferList;
	globus_list_t * FreeBufferList;

	struct pio_callout {
		stor_buffer_t * Buffer;
		globus_off_t    NeededOffset;
	} PioCallout;

} stor_info_t;

void
stor(globus_gfs_operation_t       Operation,
     globus_gfs_transfer_info_t * TransferInfo,
     config_t                   * Config);

#endif /* HPSS_DSI_STOR_H */
