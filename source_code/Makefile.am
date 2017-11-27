# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/source_code/Makefile.am $ $Id: Makefile.am 56 2017-03-20 18:48:53Z toennies $ $Rev: 56 $ 
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
# Copyright © 2014, NCSA.  All rights reserved.
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

# SUBDIRS is an automake macro which contains
# the list of subdirectories that have Makefile.am's
SUBDIRS=source
ACLOCAL_AMFLAGS=-I m4

BUILD_TOP_ROOT = $(abs_srcdir)
RPM_BUILD_ROOT = $(BUILD_TOP_ROOT)

RPM_PKG_NAME      = dsi
RPM_BUILD_DIR     = _rpmbuild_
RPM_TOPDIR_FILE   = $(abs_srcdir)/$(RPM_BUILD_DIR)/.rpmbuild_topdir
RPM_URL_FILE      = $(abs_srcdir)/$(RPM_BUILD_DIR)/.rpmbuild_url
RPM_TMP_DIR_FILE  = $(abs_srcdir)/$(RPM_BUILD_DIR)/.rpmbuild_tempdir
RPM_VERSION_FILE  = $(abs_srcdir)/$(RPM_BUILD_DIR)/.rpmbuild_version
RPM_RELEASE_FILE  = $(abs_srcdir)/$(RPM_BUILD_DIR)/.rpmbuild_release
RPM_BUILD_BASE    = `basename $(RPM_BUILD_ROOT)`
RPM_SOURCE_DIR    = `cat $(RPM_TOPDIR_FILE)`/SOURCES
RPM_SOURCE_TAR    = $(RPM_SOURCE_DIR)/$(RPM_PKG_NAME).tar
RPM_TOPDIR        = `cat $(RPM_TOPDIR_FILE)`
RPM_TMP_DIR       = `cat $(RPM_TMP_DIR_FILE)`
RPM_TMP_PKG_DIR   = $(RPM_TMP_DIR)/$(RPM_PKG_NAME)
RPM_BUILD_IT      = rpmbuild -ba --define 'sourcedir $(RPM_PKG_NAME)' --define '_topdir '$(RPM_TOPDIR) --define '_build_url '`cat $(RPM_URL_FILE)` --define 'new_build_root $(BUILD_TOP_ROOT)/$(RPM_BUILD_DIR)/BUILD/$(RPM_PKG_NAME)' --define '_rpm_version '`cat $(RPM_VERSION_FILE)` --define '_rpm_release '`cat $(RPM_RELEASE_FILE)`

rpm-setup:;	@if [ ! -d $(RPM_BUILD_ROOT) ]; then \
					echo "Error: the directory $(RPM_BUILD_ROOT) should exist."; \
					exit 1; \
				fi
				echo "Creating RPM build directory $(RPM_BUILD_ROOT)/$(RPM_BUILD_DIR)"; \
				mkdir -p $(RPM_BUILD_ROOT)/$(RPM_BUILD_DIR); \
				echo "Setting up file $(RPM_VERSION_FILE)"; \
				rpm_version=`grep -w VERSION $(RPM_BUILD_ROOT)/config.h | awk '{print $$3}' | sed 's/"//g'`; \
				echo "$$rpm_version" > $(RPM_VERSION_FILE); \
				echo "Contents of $(RPM_VERSION_FILE):"; \
				cat $(RPM_VERSION_FILE); \
				echo "Setting up file $(RPM_RELEASE_FILE)"; \
				rpm_release=`grep -w RELEASE $(RPM_BUILD_ROOT)/config.h | awk '{print $$3}' | sed 's/"//g'`; \
				echo "$$rpm_release%{?dist}" > $(RPM_RELEASE_FILE); \
				echo "Contents of $(RPM_RELEASE_FILE):"; \
				cat $(RPM_RELEASE_FILE); \
				echo "Setting up file $(RPM_TOPDIR_FILE)"; \
				echo "$(RPM_BUILD_ROOT)/$(RPM_BUILD_DIR)" > $(RPM_TOPDIR_FILE); \
				echo "Contents of $(RPM_TOPDIR_FILE):"; \
				cat $(RPM_TOPDIR_FILE); \
				echo "Setting up file $(RPM_TMP_DIR_FILE)"; \
				echo "`mktemp -u -d /tmp/hpss.XXXXXX`" > $(RPM_TMP_DIR_FILE); \
				echo "Contents of $(RPM_TMP_DIR_FILE):"; \
				cat $(RPM_TMP_DIR_FILE); \
				echo "Setting up file $(RPM_URL_FILE)"; \
				svn info $(RPM_BUILD_ROOT) 2>/dev/null | awk 'BEGIN { FOUND=0 } /^URL: / { printf $$0; FOUND=1 } /^Last Changed Rev: / { printf $$0; FOUND=1 } END { if (FOUND==1) {print}}' | sed -n 's/URL: \(.*\)Last Changed Rev: \(.*\)/\1@\2/p' > $(RPM_URL_FILE); \
				if [[ ! -s $(RPM_URL_FILE) ]]; then \
				   echo "Error trying to determine build URL from extracting svn info from" ;\
					echo "directory $(RPM_BUILD_ROOT). Setting build URL to basename" ;\
					echo "of this directory."; \
				   echo $(RPM_BUILD_BASE) > $(RPM_URL_FILE); \
				fi; \
				echo "Contents of $(RPM_URL_FILE):"; \
				cat $(RPM_URL_FILE)

rpm-build-prep:;
				@if [ -z "$(RPM_TMP_DIR)" ]; then \
					echo "Error: the value for the RPM_TMP_DIR is not set."; \
					exit 1; \
				fi
				@if [ "$(RPM_TMP_DIR)" == "/" ]; then \
				   echo "Invalid value '/' for RPM temporary directory."; \
				   exit 1; \
				fi
				@if [ "$(RPM_SOURCE_DIR)" == "/" ]; then \
				   echo "Invalid value '/' for RPM source directory."; \
				   exit 1; \
				fi
				rm -rf $(RPM_TMP_DIR)
				mkdir -p $(RPM_TOPDIR)/BUILD
				mkdir -p $(RPM_TOPDIR)/BUILDROOT
				mkdir -p $(RPM_TOPDIR)/RPMS
				mkdir -p $(RPM_TOPDIR)/SRPMS
				mkdir -p $(RPM_TOPDIR)/SOURCES
				mkdir -p $(RPM_TOPDIR)/COMMON
				mkdir -p $(RPM_TMP_PKG_DIR)
				@echo "Copying source tree from `pwd` to $(RPM_TMP_PKG_DIR)"
				@echo "Copying files to destination directory $(RPM_TMP_DIR)"
				tar -cf - --exclude=$(RPM_BUILD_DIR) --exclude-vcs . | (cd $(RPM_TMP_PKG_DIR) && tar xf -)
				@if [ -d $(RPM_SOURCE_DIR) ]; then \
				   echo "Cleaning up old RPM source directory $(RPM_SOURCE_DIR)"; \
				   rm -rf $(RPM_SOURCE_DIR); \
				fi
				mkdir -p $(RPM_SOURCE_DIR)
				@echo "Creating $(RPM_SOURCE_TAR) of files in $(RPM_TMP_DIR)"
				@echo "Running command 'tar -cf $(RPM_SOURCE_TAR) .' while inside directory $(RPM_TMP_DIR)"
				(cd $(RPM_TMP_DIR) && tar -cf $(RPM_SOURCE_TAR) .)
				@echo "rm -rf $(RPM_TMP_DIR)"
				@rm -rf $(RPM_TMP_DIR)

rpm-prep: clean rpm-setup rpm-build-prep

rpm: rpm-prep
		$(RPM_BUILD_IT) $(abs_srcdir)/dsi.spec
