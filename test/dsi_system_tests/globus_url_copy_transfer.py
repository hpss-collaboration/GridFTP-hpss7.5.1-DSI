import os
from subprocess import call
import logging

import test_config

class globus_url_copy_transfer():

    def transfer_file(self, source, destination):
        logging.info('Transfering source %s to destination %s',
                source, destination)
        cmd_list = test_config.sudo + [test_config.guc] +\
                [source, destination]
        return call(cmd_list)

    def transfer_file_checksum_option(self, source, destination,
            cksm_algorithm=None):
        logging.info('Transfering source %s to destination %s with checksum' +
                  ' algorithm %s', source, destination, cksm_algorithm)       
        if cksm_algorithm:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-checksum', '-algo', cksm_algorithm] +\
                    [source, destination]
        else:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-checksum', source, destination]
        return call(cmd_list)

    def transfer_file_sync_option(self, source, destination, sync_level):
        logging.info('Transfering source %s to destination %s with sync ' +
                'level %s', source, destination, sync_level)

        cmd_list = test_config.sudo + [test_config.guc] +\
                   ['-sync', '-sync-level', str(sync_level)] +\
                   [source, destination]
        
        return call(cmd_list)


    def transfer_file_checksum_sync_option(self, source, destination, 
            sync_level, cksm_algorithm=None):
        logging.info('Transfering source %s to destination %s with sync' +
                ' level %s and checksum algorithm %s', source, destination,
                sync_level, cksm_algorithm)

        if cksm_algorithm:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-checksum', '-algo', cksm_algorithm] +\
                    ['-sync', '-sync-level', str(sync_level)] +\
                    [source, destination]
        else:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-sync', '-sync-level', str(sync_level)] +\
                    ['-checksum', source, destination]

        return call(cmd_list)
    
    def recursive_transfer_files(self, source, destination):
        logging.info('Recursively transfering source %s to destination %s',
                source, destination)
        cmd_list = test_config.sudo + [test_config.guc] +\
                ['-r', source, destination]
        return call(cmd_list)

    def recursive_transfer_files_checksum_option(self, source, destination,
            cksm_algorithm=None):
        logging.info('Recursively transfering source %s to destination %s' + 
                'with checksum algorithm %s',
                source, destination, cksm_algorithm)

        if cksm_algorithm:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-r', '-checksum', '-algo', cksm_algorithm] +\
                    [source, destination]
        else:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-r', '-checksum', source, destination]

        return call(cmd_list)

    def recursive_transfer_file_sync_option(self, source, destination,
            sync_level):
        logging.info('Recursively transfering source %s to destination %s' + 
                'with sync level %s',
                source, destination, sync_level)

        cmd_list = test_config.sudo + [test_config.guc] +\
                   ['-r', '-sync', '-sync-level', str(sync_level)] +\
                   [source, destination]
        
        return call(cmd_list)


    def recursive_transfer_files_checksum_sync_option(self, source,
            destination, sync_level, cksm_algorithm=None):

        logging.info('Recursively transfering source %s to destination %s' + 
                'with sync level %s and checksum algorithm %s',
                source, destination, sync_level, cksm_algorithm)

        if cksm_algorithm:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-r', '-checksum', '-algo', cksm_algorithm] +\
                    ['-sync', '-sync-level', str(sync_level)] +\
                    [source, destination]
        else:
            cmd_list = test_config.sudo + [test_config.guc] +\
                    ['-r', '-sync', '-sync-level', str(sync_level)] +\
                    ['-checksum', source, destination]

        return call(cmd_list)

    def create_directory_transfer_file(self, source, destination):
        logging.info('Transfering source %s to destination %s' + 
                ' with create directory option' ,
                source, destination)

        cmd_list = test_config.sudo + [test_config.guc, '-cd', source] +\
                [destination]
        return call(cmd_list)

