# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/hpss_client_api/Makefile $ $Id: Makefile 10 2016-12-06 18:06:24Z gregthor $ $Rev: 10 $
LOCAL_ROOT   = /opt/hpss
HPSS_INCLUDE = $(LOCAL_ROOT)/include
HPSS_LIBS    = $(LOCAL_ROOT)/lib

include $(LOCAL_ROOT)/Makefile.macros

# Change the HPSS_VERSION variable below to match one of the following
# release versions under testing. Valid values are: 
# 743p2a, 743p2b, 743p3 or 751
HPSS_VERSION=751

HPSS_DEF_FLAG_743p2a = -DHPSS743P2A
HPSS_DEF_FLAG_743p2b = -DHPSS743P2B
HPSS_DEF_FLAG_743p3 = -DHPSS743P3
HPSS_DEF_FLAG_751 = -DHPSS751

AUTH_UNIX_on  = -lhpssunixauth
AUTH_UNIX_off =
AUTH_KRB5_on  = -lhpsskrb5auth
AUTH_KRB5_off =

CFLAGS_LINUX  = -Wall -Wno-strict-prototypes -Wno-strict-aliasing

PYTHON_LINUX_on  = python
PYTHON_LINUX_off = $(PYTHON_LINUX_on)
PYTHON_AIX_on    = python_64
PYTHON_AIX_off   = python
PYTHON           = $(PYTHON_$(BUILD_PLATFORM)_$(BIT64_SUPPORT))

COMPFLAGS     = -g $(CC_BIT64_FLAGS) $(MACHINE_ARCH_FLAGS) \
				$(CFLAGS_$(BUILD_PLATFORM)) \
				$(HPSS_DEF_FLAG_$(HPSS_VERSION))
								
				
INCLUDE_PATH  = -I. -I$(TIRPC_INCLUDE) -I$(HPSS_INCLUDE)
CFLAGS        = $(INCLUDE_PATH) $(COMPFLAGS) $(MACHINE_FLAGS)
LDFLAGS       = $(COMPFLAGS) $(CLNT_RUNFLAGS) -L$(HPSS_LIBS) \
		$(LINUX_SHLIBLIBS) \
		-lhpss -lhpsscs -lhpsstools \
			$(AUTH_KRB5_$(KRB5_AUTH_SUPPORT)) \
			$(AUTH_UNIX_$(UNIX_AUTH_SUPPORT))

TEST_SO       = hpssclapi.so
SRCS          = $(shell ls *.c)
TEST_OBJS     = $(SRCS:.c=.o)

TEMP_OBJ_DIR  = build/temp.linux-x86_64-2.6
TEMP_LIB_DIR  = build/lib.linux-x86_64-2.4.6


TCFLAGS       =  $(CFLAGS) -I/usr/include/python2.6  
SHARED_FLAGS = $(SHLIBFLAGS) $(SHLIBLIBS) -L/opt/hpss/lib/ -lhpss -lhpsscs -lhpssunixauth -I. -I/opt/hpss/include -L. -lpython2.6
 
export CFLAGS
export LDFLAGS

all:
	@$(PYTHON) setup.py build

install: all
	@$(PYTHON) setup.py install

test:   $(TEST_SO)
	@echo Make $@ done 


$(TEST_SO):     $(TEST_OBJS)
		@echo Make $@ 
		@/bin/rm -f $(TEMP_LIB_DIR)/$@
		$(CC) $(SHARED_FLAGS) -o $(TEMP_LIB_DIR)/$@ $(shell find . -name '*.o' -type f -print)
		cp build/lib.linux-x86_64-2.4.6/hpssclapi.so /usr/lib64/python2.6/site-packages


$(TEST_OBJS): $(SRCS)
	@echo Make $@
	@/bin/rm -f $@
	@/bin/mkdir -p $(TEMP_LIB_DIR)
	@/bin/mkdir -p $(TEMP_OBJ_DIR)
	$(CC) $(TCFLAGS) -c $*.c -o $(TEMP_OBJ_DIR)/$@


clean:
	@$(PYTHON) setup.py clean
	rm -rf build/
