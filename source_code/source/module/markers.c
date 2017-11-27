//$URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/source_code/source/module/markers.c $ $Id: markers.c 56 2017-03-20 18:48:53Z toennies $ $Rev: 56 $
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
 * Copyright © 2014-2015 NCSA.  All rights reserved.
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
 * Wrappers for perf and restart markers which may or may not
 * be available depending upon what is installed on the system.
 */

/*
 * System includes.
 */
#include <pthread.h>

/*
 * Local includes.
 */
#include "markers.h"
#include "dl.h"

typedef void (*globus_perf_markers_func_t) (globus_gfs_operation_t Operation,
                                            globus_off_t           Length);

typedef void (*globus_restart_markers_func_t) (globus_gfs_operation_t Operation,
                                               globus_off_t           Offset,
                                               globus_off_t           Length);

static globus_restart_markers_func_t _globus_restart_markers = NULL;
static globus_perf_markers_func_t    _globus_perf_markers = NULL;

static void
_load_symbols()
{
		_globus_restart_markers = dl_find_symbol("globus_gridftp_server_update_range_recvd");
		_globus_perf_markers = dl_find_symbol("globus_gridftp_server_update_bytes_recvd");
}

static void
_init_symbols()
{
	static pthread_once_t once_control = PTHREAD_ONCE_INIT;

	pthread_once(&once_control, _load_symbols);
}

void
markers_update_perf_markers(globus_gfs_operation_t Operation,
                            globus_off_t           Offset,
                            globus_off_t           Length)
{
	_init_symbols();

	if (_globus_perf_markers)
		_globus_perf_markers(Operation, Length);
	else
		globus_gridftp_server_update_bytes_written(Operation, Offset, Length);
}

void
markers_update_restart_markers(globus_gfs_operation_t Operation,
                               globus_off_t           Offset,
                               globus_off_t           Length)
{
	_init_symbols();

	if (_globus_restart_markers)
		_globus_restart_markers(Operation, Offset, Length);
}

int
markers_restart_supported()
{
	_init_symbols();
	return (_globus_restart_markers != NULL);
}
