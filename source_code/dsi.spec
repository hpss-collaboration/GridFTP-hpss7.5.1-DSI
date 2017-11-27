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
%if !%(test "%{?dsibuilddir:1}" == 1 && echo 1 || echo 0)
%define dsibuilddir %{_builddir}/%{sourcedir}
%endif

%define dsi_dir /usr/local/gridftp-hpss-%{version}
%define dsi_src_dir /hpss_src/dsi-%{version}-%{release}

%global dsi_files_list %(mktemp /tmp/dsi.XXXXXXXX)
%global dsi_clean_src_files_list %(mktemp /tmp/dsi_src_srcdir.XXXXXXXX)
%global dsi_time_reference_file %(mktemp /tmp/dsi_time_reference_file.XXXXXXXX)

%global __os_install_post /usr/lib/rpm/brp-compress %{nil}

Name: hpss-gridftp-dsi
# The values for version and release are parsed automatically from the
# config.h file and are specified as defined values by the RPM_BUILD_IT
# command in the Makefile.
Version: %{_rpm_version}
Release: %{_rpm_release}
Summary: HPSS GridFTP Data Storage Interface binaries
Group: Applications/Storage
License: IBM proprietary and NCSA Open Source License
URL: http://hpss-collaboration.org
Source0: %{sourcedir}.tar
Requires: hpss-lib >= 7.5.1.0-0
BuildRequires: hpss-clnt-devel >= 7.5.1.0-0
AutoReqProv: no
Distribution: %{_build_url}

%description
%{summary}

%package src
Summary: HPSS GridFTP Data Storage Interface source code
Group: Applications/Storage
Requires: hpss-clnt-devel >= 7.5.1.0-0
AutoReqProv: no
%description src
%{summary}

# %setup is a standard RPM macro used to unpack the source files in preparation
# for the build.
%prep
%setup -q -n %{sourcedir}

%build
if [ -e $RPM_BUILD_ROOT ]; then
   rm -rf $RPM_BUILD_ROOT
fi
mkdir -p $RPM_BUILD_ROOT

cd %{new_build_root}
# TODO: This will still leave some files created by autoreconf or configure
# in the list of files for the src RPM. Should I exclude these also?
find . -type f | grep -Ev "\.deps|autom4te.cache|m4" > %{dsi_clean_src_files_list}

echo "Doing build for DSI RPM %{name}"
make clean
make

# The user doing the build should have access to uninstall and install the
# product under the libdir specified with the configure command.
make uninstall
make install

# The directory $RPM_BUILD_ROOT serves as a staging directory and files copied
# to it will be made part of the RPM package.
%install
export DONT_STRIP=1
mkdir -p $RPM_BUILD_ROOT%{dsi_dir}
mkdir -p $RPM_BUILD_ROOT%{dsi_src_dir}

#------------------------------------------------------------------------------
# Copy files for dsi RPM
#------------------------------------------------------------------------------
# Copy installed files that are newer than the reference file.
pushd %{dsi_dir}
find -L . -type f -newer %{dsi_time_reference_file} | rsync -al --files-from - . $RPM_BUILD_ROOT%{dsi_dir}
popd

echo "%defattr(-, root, hpss, 755)" > %{dsi_files_list}
# Get a list of files that will go into the %{name} RPM. The -L argument is
# used to include links.
find -L $RPM_BUILD_ROOT%{dsi_dir} -type f | \
   sed "s?^$RPM_BUILD_ROOT??" >> %{dsi_files_list}

#------------------------------------------------------------------------------
# Copy files for the dsi-src RPM from the list of files created earlier before
# doing any makes.
#------------------------------------------------------------------------------
echo "Copying files needed for package %{name}-src."

rsync -arp --files-from=%{dsi_clean_src_files_list} %{dsibuilddir} $RPM_BUILD_ROOT%{dsi_src_dir}

# Create some empty directories that are required to run the autoconf or
# autoreconf tools.
mkdir $RPM_BUILD_ROOT%{dsi_src_dir}/m4
mkdir $RPM_BUILD_ROOT%{dsi_src_dir}/autom4te.cache

%post
echo "Files for package %{name} installed under %{dsi_dir}"

%post src
echo "Files for package %{name}-src installed under %{dsi_src_dir}"

%clean
rm -rf %{buildroot}
rm -f %{dsi_files_list} %{dsi_time_reference_file} %{dsi_clean_src_files_list}

# Only these files will be packaged. If the files in $RPM_BUILD_ROOT don't
# match the files listed here, rpmbuild will fail. Also, each file copied
# above must appear in the files list.
%files -f %{dsi_files_list}

%files src
%defattr(-, hpss, hpss, 755)
%{dsi_src_dir}

%changelog
