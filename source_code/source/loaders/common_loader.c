//$URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/source_code/source/loaders/common_loader.c $ $Id: common_loader.c 56 2017-03-20 18:48:53Z toennies $ $Rev: 56 $
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


/**************************************************************************
 *
 * IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
 *
 * Read README.xdr before trying to make changes to the loader.
 *
**************************************************************************/

/*
 * System includes
 */
#include <syslog.h>
#include <stdarg.h>
#include <dlfcn.h>

/*
 * Globus includes
 */
#include <globus_gridftp_server.h>

/* This is used to define the debug print statements. */
GlobusDebugDefine(GLOBUS_GRIDFTP_SERVER_HPSS);

/* Our handle reference which we hold between activate / deactivate. */
static void * _real_module_handle = NULL;

static void
loader_log_to_syslog(const char * Format, ...)
{
	va_list ap;

	openlog("globus-gridftp-server", 0, LOG_FTP);

	va_start(ap, Format);
	vsyslog(LOG_ERR, Format, ap);
	va_end(ap);
}

/*
 * Only allowed an int return, must log errors internally.
 */
int
loader_activate(const char                 * DsiName,
                const char                 * DsiInterface,
                globus_module_descriptor_t * Module)
{
	int rc;
	globus_gfs_storage_iface_t * dsi_interface = NULL;

	rc = globus_module_activate(GLOBUS_COMMON_MODULE);
	if(rc != GLOBUS_SUCCESS)
		return rc;

	/* Clear any previous errors. */
	dlerror();

	/* Open our module. */
	_real_module_handle = dlopen("libglobus_gridftp_server_hpss_real.so",
	                             RTLD_NOW|RTLD_DEEPBIND);

	if (!_real_module_handle)
	{
		loader_log_to_syslog("Failed to open libglobus_gridftp_server_hpss_real.so: %s", dlerror());
		return GLOBUS_FAILURE;
	}

	dsi_interface = (globus_gfs_storage_iface_t*)dlsym(_real_module_handle, DsiInterface);
	if (!dsi_interface)
	{
		loader_log_to_syslog("Failed to find symbol %s in libglobus_gridftp_server_hpss_real.so: %s",
		                     DsiInterface,
		                     dlerror());
		return GLOBUS_FAILURE;
	}

	globus_extension_registry_add(GLOBUS_GFS_DSI_REGISTRY,
	                              DsiName,
	                              Module,
	                              dsi_interface);

	GlobusDebugInit(GLOBUS_GRIDFTP_SERVER_HPSS,
	                ERROR WARNING TRACE INTERNAL_TRACE INFO STATE INFO_VERBOSE);

	return GLOBUS_SUCCESS;
}

/*
 * Only allowed an int return, must log errors internally.
 */
int
loader_deactivate(const char * DsiName)
{
	globus_extension_registry_remove(GLOBUS_GFS_DSI_REGISTRY, DsiName);

	globus_module_deactivate(GLOBUS_COMMON_MODULE);

	if (_real_module_handle)
		dlclose(_real_module_handle);

	return GLOBUS_SUCCESS;
}

