# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/directory.py $: $id:$ $Rev: 17 $
import logging, os
import hpssclapi

class directory(object):

    def create_hpss_directory(self, path, mode):
        logging.info('Create hpss dir %s with mode %s', path, mode)
        try:
            old_mask = hpssclapi.Umask(0)
            hpssclapi.Mkdir(path, mode)
            hpssclapi.Umask(old_mask)
        except IOError as e:
            if e.errno == 17:
                logging.info(path + ' should not exist')
                pass
            else:
                raise

    def create_local_directory(self, path, mode=None):
        try:
            if mode:
                logging.info('Create local dir %s with mode %s', path, mode)
                os.mkdir(path, mode)
            else:
                logging.info('Create local dir %s', path)
                os.mkdir(path)
        except OSError as e:
            if e.errno == 17:
                pass
            else:
                raise


