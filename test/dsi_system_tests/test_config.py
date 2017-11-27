# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/test_config.py $ $Id: test_config.py 20 2017-01-30 18:16:26Z gregthor $ $Rev: 20 $

# Enter the values unique to your system here

# the LoginUser variable for gridftp.conf
ftp_user = 'hpssftp'

guc = '/usr/local/globus-6/bin/globus-url-copy'
hpss_bin = '/opt/hpss/bin'

dsi_config_file = '/var/hpss/etc/gridftp.conf'
# the user in the grid-map file
grid_user = 'gregthor'
# Either unix or kerberos
auth_mech = 'unix'
auth = '/var/hpss/etc/hpss.unix.keytab'

sudo =  ['sudo', '-u', grid_user] 

# The subsystem to store files on
subsys_id = '1'

#Config for globus_sdk tests
client_id = '6466f675-9cfa-4565-aa0b-87d279172ef8'
hpss_ep_id = '6e45a324-9f9d-11e6-b0dd-22000b92c261'
globus_test_ep_id = 'ddb59aef-6d04-11e5-ba46-22000b92c6ec'

# User account name used for globus webapp
globus_user = 'ibmhpss' 
