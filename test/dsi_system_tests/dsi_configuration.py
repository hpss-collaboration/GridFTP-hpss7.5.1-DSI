# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/dsi_configuration.py $ $Id: dsi_configuration.py 16 2017-01-27 23:49:10Z gregthor $ $Rev: 16 $
import os 
import test_config


class dsi_configuration(object):

    def __init__(self):
        self.default_config = {}
        self.default_config['AuthenticationMech'] = test_config.auth_mech
        self.default_config['Authenticator'] = test_config.auth
        self.default_config['LoginName'] = test_config.ftp_user
        self.default_config['ChecksumSupport'] = 'off'
        self.default_config['UDAChecksumSupport'] = 'off'

        self.cksm_support_config = self.default_config.copy()
        self.cksm_support_config['ChecksumSupport'] = 'on'

        self.uda_support_config = self.default_config.copy()
        self.uda_support_config['UDAChecksumSupport'] = 'on'

    def load_config(self, config_dict):
        os.remove(test_config.dsi_config_file)
        with open(test_config.dsi_config_file, 'w') as fout:
            for k, v in config_dict.iteritems():
                fout.write(k + ' ' + v + '\n')

    def get_config(self):
        with open(test_config.dsi_config_file, 'r') as fout:
            config = dict(line.split(' ') for line in fout)

        for k, v in config.iteritems():
            config[k] = v.replace('\n','')

        return config
