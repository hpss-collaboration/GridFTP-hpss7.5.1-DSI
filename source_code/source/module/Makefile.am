#   begin_generated_IBM_prolog
#
#   This is an automatically generated prolog
#   ------------------------------------------------------------------
#   Product(s): HPSS Data Storage Interface
#
#   Licensed Materials
#
#   Copyright (C) 2016, 2017
#   International Business Machines Corporation, The Regents of the
#   University of California, Los Alamos National Security, LLC,
#   Lawrence Livermore National Security, LLC, Sandia Corporation,
#   and UT-Battelle.
#
#   All rights reserved.
#
#   Portions of this work were produced by Lawrence Livermore National
#   Security, LLC, Lawrence Livermore National Laboratory (LLNL)
#   under Contract No. DE-AC52-07NA27344 with the U.S. Department
#   of Energy (DOE); by the University of California, Lawrence
#   Berkeley National Laboratory (LBNL) under Contract
#   No. DE-AC02-05CH11231 with DOE; by Los Alamos National
#   Security, LLC, Los Alamos National Laboratory (LANL) under
#   Contract No. DE-AC52-06NA25396 with DOE; by Sandia Corporation,
#   Sandia National Laboratories (SNL) under Contract
#   No. DE-AC04-94AL85000 with DOE; and by UT-Battelle, Oak Ridge
#   National Laboratory (ORNL) under Contract No. DE-AC05-00OR22725
#   with DOE.  The U.S. Government has certain reserved rights under
#   its prime contracts with the Laboratories.
#
#                             DISCLAIMER
#   Portions of this software were sponsored by an agency of the
#   United States Government.  Neither the United States, DOE, The
#   Regents of the University of California, Los Alamos National
#   Security, LLC, Lawrence Livermore National Security, LLC, Sandia
#   Corporation, UT-Battelle, nor any of their employees, makes any
#   warranty, express or implied, or assumes any liability or
#   responsibility for the accuracy, completeness, or usefulness of
#   any information, apparatus, product, or process disclosed, or
#   represents that its use would not infringe privately owned rights.
#
#   High Performance Storage System is a trademark of International
#   Business Machines Corporation.
#   ------------------------------------------------------------------
#
#   end_generated_IBM_prolog
##########################################################################
# University of Illinois/NCSA Open Source License
#
# Copyright © 2015 NCSA.  All rights reserved.
#
# Developed by:
#
# Storage Enabling Technologies (SET)
#
# Nation Center for Supercomputing Applications (NCSA)
#
# http://www.ncsa.illinois.edu
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the .Software.),
# to deal with the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
#    + Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimers.
#
#    + Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimers in the
#      documentation and/or other materials provided with the distribution.
#
#    + Neither the names of SET, NCSA
#      nor the names of its contributors may be used to endorse or promote
#      products derived from this Software without specific prior written
#      permission.
#
# THE SOFTWARE IS PROVIDED .AS IS., WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS WITH THE SOFTWARE.
############################################################################

HPSS_ROOT=$(HPSS_LOCATION)

#
# Include the HPSS Makefile to get build information. The HPSS Makefile
# expects LOCAL_ROOT to be set.
#
LOCAL_ROOT=$(HPSS_ROOT)
include $(HPSS_LOCATION)/Makefile.macros

# Build targets of the form <dir>_<primary> where dir is where to install it:
#   Ex. bin, man, lib, pkglib, include
# and primary is what type of target it is:
#   Ex. PROGRAMS, MANS, LTLIBRARIES, SCRIPTS, DATA, LIBRARIES, HEADERS
# Note: LTLIBRARIES is a libtool library
# Note: The names of these libraries are important, the gridftp server appends
# our dsi name (hpss_control or hpss_data) onto libglobus_gridftp_server_ when
# loading our DSI.
lib_LTLIBRARIES = libglobus_gridftp_server_hpss_real.la

SOURCES = dsi.c \
	      config.c  \
	      authenticate.c \
	      commands.c \
	      stor.c \
	      retr.c \
	      cksm.c \
	      pio.c \
	      dl.c \
	      markers.c \
	      stage.c \
	      stat.c

libglobus_gridftp_server_hpss_real_la_SOURCES=$(SOURCES)

AM_CPPFLAGS= -Wall -DLINUX $(GLOBUS_CPPFLAGS) -I$(HPSS_ROOT)/include
AM_LDFLAGS= -L$(HPSS_ROOT)/lib -Wl,-rpath,$(HPSS_ROOT)/lib $(GLOBUS_LDFLAGS)

#
# IU and NERSC report that using krb fails because libhpssgss.so doesn't link
# against libhpsskrbauth.so. NERSC reported that libhpssunixauth.so was
# necessary in their environment. It is necessary to link these here; the
# XDR/libtirpc issue causes the runtime dynamic linking to fail without this.
#

if WITH_HPSS_PKG_CFG
CC+= $(HPSS_PKG_CFG_CFLAGS)
else
#
# Include the HPSS Makefile to get build information. The HPSS Makefile
# expects LOCAL_ROOT to be set.
#
include $(HPSS_LOCATION)/Makefile.macros
endif

libglobus_gridftp_server_hpss_real_la_LIBADD=-lglobus_gridftp_server -lhpsskrb5auth -lhpssunixauth -lhpss
