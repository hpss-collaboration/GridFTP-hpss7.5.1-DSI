# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/sdk_system_tests.py $: $id:$ $Rev: 17 $
import logging, sys, time, unittest, hashlib, socket, os
import hpssclapi

import test_config

from globus_sdk_dir.globus_sdk_tokens import globus_sdk_tokens
from globus_sdk_dir.globus_sdk_transfer import globus_sdk_transfer
from globus_sdk_dir.globus_sdk_operations import globus_sdk_operations
from globus_sdk_dir.globus_sdk_endpoints import globus_sdk_endpoints

from dsi_configuration import dsi_configuration
from checksum import checksum
from hpss_file import hpss_file
from globus_url_copy_transfer import globus_url_copy_transfer


class sdk_system_tests(unittest.TestCase):
    def delete_file(self, ep_id, parent_dir, file_name):
        if (operation_manager.file_exists(ep_id, parent_dir, file_name)):
            logging.info('Deleting file ' + parent_dir + '/' +file_name)
            hpssclapi.Unlink(parent_dir + '/' + file_name)

    def delete_dir(self, ep_id, parent_dir, dir_name):
        if (operation_manager.dir_exists(ep_id, parent_dir, dir_name)):
            logging.info('Deleting dir ' + parent_dir + '/' + dir_name)
            hpssclapi.Rmdir(parent_dir + '/' + dir_name)

    def verify_transfer_to_globus(self, parent_dir, file_path):
        file_info = operation_manager.get_file_info(
                test_config.globus_test_ep_id, parent_dir, file_path)

        file_msg = ' for file ' + parent_dir + '/' + file_path +\
            ' on globus test endpoint' 
        
        self.assertTrue(file_info['user'] == test_config.globus_user, 
                'File owner incorrect: ' + file_info['user'] + ' != ' +
                test_config.globus_user  + file_msg)

        self.assertTrue(file_info['permissions'] == '0644', 'File ' +
                'permissions incorrect: ' +  file_info['permissions'] +
                ' != 0644' + file_msg)

        self.assertTrue(file_info['group'] == 'tutorial',
                'File owner not in correct group:' + file_info['group'] +
                '!=tutorial '+ file_msg)


    def verify_transfer_to_hpss(self, parent_dir, file_path):
        self.assertTrue(operation_manager.file_exists(test_config.hpss_ep_id,
            parent_dir, file_path), 'File did not transfer')

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                parent_dir, file_path)

        file_msg = ' for file ' + parent_dir + '/' + file_path +\
            ' on hpss endpoint' 
        
        self.assertTrue(file_info['user'] == test_config.grid_user, 
                'File owner incorrect: ' + file_info['user'] + ' != ' +
                test_config.grid_user  + file_msg)

        self.assertTrue(file_info['permissions'] == '0644', 'File ' +
                'permissions incorrect: ' +  file_info['permissions'] +
                ' != 0644' + file_msg)

        h_file = hpss_file()
        hpss_stat = h_file.fstat(parent_dir + '/' + file_path)
        group = hpssclapi.SECGetGroupNameById(hpss_stat.st_gid) 
        
        self.assertTrue(file_info['group'] == group, 'File owner not in ' +
                'correct group:' + file_info['group']+'!='+group + file_msg)

    def verify_task_completion(self, task_id, timeout=600,
            polling_interval=30):
        operation_manager.complete_task(task_id, timeout, polling_interval)
        self.assertTrue(operation_manager.task_successful(task_id),
            'Task unsuccessful')
   
    def verify_no_checksum_stored(self, file_path):
        stored_cksm = self.cksm.get_stored_checksum_info(file_path)
        if stored_cksm:
            self.fail('Checksum stored on file ' + file_path + ' when it ' +
                    'should not have been')

        stored_cksm = self.cksm.get_stored_uda_checksum_info(file_path)
        if stored_cksm:
            self.fail('Checksum stored on file ' + file_path + ' when it ' +
                    'should not have been')
    
    def verify_checksum_stored(self, file_path, hasher=hashlib.md5()):
        stored_cksm = self.cksm.get_stored_checksum_info(file_path)
        calc_cksm = self.cksm.calc_hpss_checksum(file_path, hasher)
        self.assertEqual(stored_cksm['digest'], calc_cksm.upper(),
                'stored checksum and calculated checksum are not equal:' +
                 stored_cksm['digest'] + '!=' + calc_cksm.upper() + ' on' +
                 ' file ' + file_path) 

        stored_cksm = self.cksm.get_stored_uda_checksum_info(file_path)
        if stored_cksm:
            self.fail('Checksum stored on file ' + file_path + ' when it ' +
                    'should not have been')

    def verify_uda_checksum_stored(self, file_path):
        stored_cksm = self.cksm.get_stored_checksum_info(file_path)
        if stored_cksm:
            self.fail('Checksum stored on file ' + file_path + ' when it ' +
                    ' should not have been')

        stored_cksm = self.cksm.get_stored_uda_checksum_info(file_path)
        calc_cksm = self.cksm.calc_hpss_checksum(self.test_dir + '/' +
                self.file_name, hashlib.md5())
        self.assertEqual(stored_cksm['checksum'], calc_cksm,
                'stored checksum and calculated checksum are not equal:' +
                 stored_cksm['checksum'] + '!=' + calc_cksm + ' on file ' +
                 file_path)


class basic_transfer(sdk_system_tests):

    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()
        self.test_dir = '/globus_api_test'
        self.file_name = 'file2.txt'
        self.hpss_path  = self.test_dir + '/' + self.file_name
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
        
        self.cksm = checksum()

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
        
        if (operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name)):
            logging.info('Deleting file ' + self.test_dir + '/' + self.file_name)
            hpssclapi.Unlink(self.test_dir + '/' + self.file_name)
        
        logging.info('Deleting dir ' + self.test_dir) 
        hpssclapi.Rmdir(self.test_dir)
        logging.info('Tear down complete') 

    def test_basic_transfer(self):
        logging.info('Running test: test_basic_transfer')         
        
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)

        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']
        
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.test_dir + '/' + self.file_name)

        logging.info('Test test_basic_transfer passed')

    def test_basic_transfer_dsi_cksm(self):
        logging.info('Running test: test_basic_transfer_dsi_cksm')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support on')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.test_dir + '/' + self.file_name)

        logging.info('Test test_basic_transfer_dsi_cksm passed')

    def test_basic_transfer_dsi_uda_cksm(self):
        logging.info('Running test: test_basic_transfer_dsi_uda_cksm')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')

        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.test_dir + '/' + self.file_name)
        
        logging.info('Test test_basic_transfer_dsi_uda_cksm passed')

    def test_preserve_timestamp_transfer(self):
        logging.info('Running test: test_preserve_timestamp_transfer')
        
        file_info = operation_manager.get_file_info(
                test_config.globus_test_ep_id, '/share/godata/',
                self.file_name)
        last_mod_one = file_info['last_modified']

        transfer_result = transfer_manager.transfer_file_preserve_timestamp(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/'+self.file_name, self.hpss_path)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        
        logging.info('Verifing that the preserve timestamp flag is set')
        task = operation_manager.get_task(task_id)
        self.assertTrue(task['preserve_timestamp'],
            'Perserve timestamp flag not set')

        file_info = operation_manager.get_file_info(
                test_config.hpss_ep_id, self.test_dir, self.file_name)
        last_mod_two = file_info['last_modified']
        
        logging.info('Verifing that the timestamp is preserved') 
        self.assertEqual(last_mod_one, last_mod_two, last_mod_one + '!=' +
                last_mod_two + ' Timestamp was not preserved')
  
        logging.info('Test test_preserve_timestamp_transfer passed')
    
    def test_transfer_no_write_permission(self):
        logging.info('Running test: test_transfer_no_write_permission')
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
        logging.info('Chmod hpss dir ' + self.test_dir + ' to 000')
        hpssclapi.Chmod(self.test_dir, 000)
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']
        self.assertFalse(operation_manager.complete_task(task_id), 
            'Task completed when it should have failed')
        self.assertFalse(operation_manager.task_successful(task_id),
            'Task successful when it should have failed') 
        logging.info('Chmod hpss dir ' + self.test_dir + ' to 0777')
        hpssclapi.Chmod(self.test_dir, 0777)
        
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists when it should not')
        
        logging.info('Test test_transfer_no_write_permission passed')



class verify_transfer(sdk_system_tests):

    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()
        self.test_dir = '/globus_api_test'
        self.file_name = 'file2.txt'
        self.hpss_path  = self.test_dir + '/' + self.file_name
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
        
        self.cksm = checksum()

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
        
        if (operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name)):
            logging.info('Deleting file ' + self.test_dir + '/' + self.file_name)
            hpssclapi.Unlink(self.test_dir + '/' + self.file_name)
        
        logging.info('Deleting dir ' + self.test_dir) 
        hpssclapi.Rmdir(self.test_dir)
        logging.info('Tear down complete') 

    def test_verify_transfer_no_cksm_stored(self):
        logging.info('Running test: test_verify_transfer_no_cksm_stored')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')

        self.dsi_config.load_config(self.dsi_config.default_config)

        transfer_result = transfer_manager.transfer_file_checksum_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.test_dir + '/' + self.file_name)
        
        logging.info('Test test_verify_transfer_no_cksm_stored passed')


    def test_verify_transfer_dsi_uda_cksm(self):
        logging.info('Running test: test_basic_transfer_dsi_uda_cksm')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')

        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        transfer_result = transfer_manager.transfer_file_checksum_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_uda_checksum_stored(self.test_dir + '/' + self.file_name)
        
        logging.info('Test test_basic_transfer_dsi_uda_cksm passed')

    def test_verify_transfer_dsi_cksm(self):
        logging.info('Running test: test_basic_transfer_dsi_uda_cksm')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')

        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        transfer_result = transfer_manager.transfer_file_checksum_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_checksum_stored(self.test_dir + '/' + self.file_name)
 
        logging.info('Test test_basic_transfer_dsi_uda_cksm passed')
   
class sync_transfer_level_three(sdk_system_tests):

    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()
        self.test_dir = '/globus_api_test'
        self.file_name = 'file2.txt'
        self.hpss_path  = self.test_dir + '/' + self.file_name
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
        
        self.cksm = checksum()

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
        
        if (operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name)):
            logging.info('Deleting file ' + self.test_dir + '/' + self.file_name)
            hpssclapi.Unlink(self.test_dir + '/' + self.file_name)
        
        logging.info('Deleting dir ' + self.test_dir) 
        hpssclapi.Rmdir(self.test_dir)
        logging.info('Tear down complete') 


    def test_sync_transfer_lvl_three(self):
        logging.info('Running test: test_sync_transfer_lvl_three')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)

        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.test_dir + '/' + self.file_name)
 
        logging.info('Test test_sync_transfer_lvl_three passed')
 
    def test_checksum_sync_transfer_lvl_three(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.test_dir + '/' + self.file_name)
 
        logging.info('Test test_checksum_sync_transfer_lvl_three passed')
   
    def test_checksum_sync_transfer_lvl_three_cksm(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three_' +
                'cksm')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_checksum_stored(self.test_dir + '/' + self.file_name)
 
        logging.info('Test test_checksum_sync_transfer_lvl_three_cksm passed')

    def test_checksum_sync_transfer_lvl_three_uda(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three_' +
                'uda')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_uda_checksum_stored(self.test_dir + '/' + self.file_name)
 
        logging.info('Test test_checksum_sync_transfer_lvl_three_uda passed')

    def test_checksum_sync_transfer_lvl_three_uda_file_exists(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three_' +
                'uda_file_exists')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')
        
        logging.info('Transfering file into hpss')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']
 
        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        
        logging.info('Transfering file 2nd time with sync level 3')
        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was ' +
        'transfered when it should not have been')

        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_uda_checksum_stored(self.test_dir + '/' + self.file_name)
        
        logging.info('Test test_checksum_sync_transfer_lvl_three_uda_' +
                'file_exists passed')

    def test_checksum_sync_transfer_lvl_three_cksm_file_exists(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three_' +
                'cksm_file_exists')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')
        
        logging.info('Transfering file into hpss')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']
 
        logging.info('DSI config set uda checksum support off and ' +
                'checksum support on')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        
        logging.info('Transfering file 2nd time with sync level 3')
        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was ' +
        'transfered when it should not have been')

        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_checksum_stored(self.test_dir + '/' + self.file_name)
        
        logging.info('Test test_checksum_sync_transfer_lvl_three_cksm_' +
                'file_exists passed')
        
    def test_checksum_sync_transfer_lvl_three_uda_cksm_exists(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three_' +
                'uda_cksnexists')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')
        
        logging.info('Transfering file into hpss')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        
        logging.info('Setting checksum UDA on file ' + self.test_dir + '/' +
                self.file_name)
        hpssclapi.UserAttrSetAttrs(self.test_dir + '/' + self.file_name,
                {'/hpss/user/cksum/checksum':'0'*40,
                '/hpss/user/cksum/algorithm': 'SHA1',
                '/hpss/user/cksum/state': 'Valid'})

        stored_cksm_info = hpssclapi.UserAttrGetAttrs(self.test_dir + '/' +
                self.file_name, ['/hpss/user/cksum/checksum',
                    '/hpss/user/cksum/algorithm'])

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']
 
        logging.info('DSI config set uda checksum support on and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        
        logging.info('Transfering file 2nd time with sync level 3')
        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was '
                + 'transfered when it should not have been')

        self.verify_transfer_to_hpss(self.test_dir, self.file_name)

        stored_cksm_info_two = hpssclapi.UserAttrGetAttrs(self.test_dir + '/' +
                self.file_name, ['/hpss/user/cksum/checksum',
                    '/hpss/user/cksum/algorithm'])
        self.assertEqual(stored_cksm_info, stored_cksm_info_two, 
                'Stored UDA checksum has changed when it should not have')

        logging.info('Test test_checksum_sync_transfer_lvl_three_uda_' +
                '_cksm_exists passed')
    
     #TODO: Build globus with modified globus-url-copy
    @unittest.skip('Need to build globus with modified globus_url_copy')
    def test_checksum_sync_transfer_lvl_three_cksm_exists(self):
        logging.info('Running test: test_checksum_sync_transfer_lvl_three_' +
                'cksm_exists')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')
        
        logging.info('Transfering file into hpss')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        
        logging.info('DSI config set uda checksum support off and ' +
                'checksum support on')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        
        logging.info('Transfering file using globus-url-copy to store sha1 '+
                'checksum')
        guc = globus_url_copy_transfer()
        guc.transfer_file_checksum_option('gsiftp://' + socket.gethostname() +
                '/' + self.hpss_path, 'gsiftp://' + socket.gethostname() +
                '/' + self.hpss_path, 'sha1')
        
        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']

        logging.info('Transfering file 2nd time with sync level 3')
        transfer_result = transfer_manager.transfer_file_checksum_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.test_dir + '/' +
                self.file_name, 3)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was ' +
        'transfered when it should not have been')

        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha1())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm, 'Stored checksum=' +
                stored_cksm + ' should be ' + dest_cksm)

        logging.info('Test test_checksum_sync_transfer_lvl_three_cksm_' +
                'file_exists passed')

class sync_transfer(sdk_system_tests):

    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()
        self.test_dir = '/globus_api_test'
        self.file_name = 'file2.txt'
        self.hpss_path  = self.test_dir + '/' + self.file_name
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
        
        self.cksm = checksum()

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
        
        if (operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name)):
            logging.info('Deleting file ' + self.test_dir + '/' + self.file_name)
            hpssclapi.Unlink(self.test_dir + '/' + self.file_name)
        
        logging.info('Deleting dir ' + self.test_dir) 
        hpssclapi.Rmdir(self.test_dir)
        logging.info('Tear down complete') 


    def test_sync_transfer_lvl_zero(self):
        logging.info('Running test: test_sync_transfer_lvl_zero')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)

        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path, 0)

        task_id = transfer_result['task_id']

        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)
 
        logging.info('Test test_sync_transfer_lvl_zero passed')
 
    def test_sync_transfer_lvl_zero_file_exists(self):
        logging.info('Running test: test_sync_transfer_lvl_zero_file_exists')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
 
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']

        logging.info('Transfering file 2nd time with sync level 0')
        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path, 0)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was ' +
            'transfered when it should not have been')

        logging.info('Test test_sync_transfer_lvl_zero_file_exists passed')
 
    def test_sync_transfer_lvl_one_size_differs(self):
        logging.info('Running test: test_sync_transfer_lvl_one_size_differs')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
 
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/file3.txt', self.hpss_path)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']

        logging.info('Transfering file 2nd time with sync level 1')
        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path, 1)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertNotEqual(last_modified, file_info['last_modified'], 'File was ' +
            'not transfered when it should have been')

        logging.info('Test test_sync_transfer_lvl_one_size_differs passed')
 
    def test_sync_transfer_lvl_one_size_equal(self):
        logging.info('Running test: test_sync_transfer_lvl_one_size_equal')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
 
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']

        logging.info('Transfering file 2nd time with sync level 1')
        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path, 1)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was ' +
            'transfered when it should not have been')

        logging.info('Test test_sync_transfer_lvl_one_size_equal passed')
 
    def test_sync_transfer_lvl_two_dest_timestamp_older(self):
        logging.info('Running test: test_sync_transfer_lvl_two_dest'+
                '_timestamp_older')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
 
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']

        logging.info('Transfering file to globus test ep so timestamp is ' +
            'newer than on destination')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id, test_config.globus_test_ep_id,
                '/share/godata/' + self.file_name, '/~/' + self.file_name)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        logging.info('Transfering file to hpss with sync level 2')
        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/~/' + self.file_name, self.hpss_path, 2)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertNotEqual(last_modified, file_info['last_modified'], 'File was ' +
            'not transfered when it should have been')
        
        logging.info('Delete file:/~/ ' + self.file_name + 'from globus endpoint')
        del_result = operation_manager.delete_file(test_config.globus_test_ep_id,
                '/~/'+self.file_name)
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)
        
        logging.info('Test test_sync_transfer_lvl_two_dest_timestamp_older ' +
                'passed')

    def test_sync_transfer_lvl_two_dest_timestamp_newer(self):
        logging.info('Running test: test_sync_transfer_lvl_two_dest'+
                '_timestamp_newer')         
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File exists before transfer')

        logging.info('DSI config set uda checksum support off and ' +
                'checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
 
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        last_modified = file_info['last_modified']

        logging.info('Transfering file 2nd time with sync level 2')
        transfer_result = transfer_manager.transfer_file_sync_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path, 2)

        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        file_info = operation_manager.get_file_info(test_config.hpss_ep_id,
                self.test_dir, self.file_name)
        self.assertEqual(last_modified, file_info['last_modified'], 'File was ' +
            'transfered when it should not have been')

        logging.info('Test test_sync_transfer_lvl_two_dest_timestamp_newer ' +
                'passed')

class transfer_to_globus(sdk_system_tests):
    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.file_name = 'file1.txt'
        self.globus_dir = '/~/test'
        self.test_dir = '/globus_api_test'
        if not (operation_manager.dir_exists(test_config.globus_test_ep_id, 
            '/~', self.globus_dir[3:])):
            logging.info('Create globus directory ' + self.globus_dir)
            operation_manager.mkdir(test_config.globus_test_ep_id,
                self.globus_dir)
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, 
            '/', self.test_dir[1:])):
            logging.info('Create hpss directory ' + self.test_dir)
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
        
        hints = hpssclapi.cos_hints()
        priorities = hpssclapi.cos_priorities()
        logging.info('Create hpss file ' + self.test_dir + '/file1.txt')
        if test_config.auth_mech == 'unix':
            mech = hpssclapi.hpss_authn_mech_unix
        elif test_config.auth_mech == 'kerberos':
            mech = hpssclapi.hpss_authn_mech_kerberos
        hpssclapi.SetLoginCred(test_config.grid_user, mech, 
                hpssclapi.hpss_rpc_cred_client,
                hpssclapi.hpss_rpc_auth_type_keytab, test_config.auth)
        
        fd = hpssclapi.Creat(self.test_dir + '/file1.txt', 0777, hints,
                priorities)[0]
        hpssclapi.Write(fd, bytearray(os.urandom(10)), 10)
        hpssclapi.Close(fd)
        hpssclapi.PurgeLoginCred()

    def tearDown(self):
        logging.info('Tear down test')
        del_result = operation_manager.recursive_delete_dir(
                test_config.globus_test_ep_id, self.globus_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)
        
        del_result = operation_manager.recursive_delete_dir(
                test_config.hpss_ep_id, self.test_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)
        logging.info('Tear down complete')

    def test_transfer_to_globus_no_write_permission(self):
        logging.info('Running test: test_transfer_to_globus_no_write_' +
                'permission')

        transfer_result = transfer_manager.transfer_file(
                test_config.hpss_ep_id, test_config.globus_test_ep_id,
                self.test_dir + '/' + self.file_name, 
                '/share/godata/test.txt')
        task_id = transfer_result['task_id']
        self.assertFalse(operation_manager.complete_task(task_id), 
            'Task completed when it should have failed')
        self.assertFalse(operation_manager.task_successful(task_id),
            'Task successful when it should have failed') 
        self.assertFalse(operation_manager.file_exists(
            test_config.globus_test_ep_id, '/share/godata', 'test.txt'),
            'File exists when it should not')
        
        logging.info('Test test_transfer_to_globus_no_write_permission passed')

    # There seems to be a bug with transfering files from HPSS to the globus
    # endpoint
    @unittest.skip('Need to fix dsi bug that prevents HPSS -> globus ' +
            'transfers')
    def test_transfer_to_globus(self):
        logging.info('Running test: test_transfer_to_globus')

        transfer_result = transfer_manager.transfer_file(
                test_config.hpss_ep_id, test_config.globus_test_ep_id,
                self.test_dir + '/' + self.file_name, 
                self.globus_dir + '/' + self.file_name)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        print operation_manager.get_task(task_id)

        self.verify_transfer_to_globus(self.globus_dir, self.file_name)

        logging.info('Test test_transfer_to_globus passed')


class recursive_transfer_to_hpss(sdk_system_tests): 

    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()
        self.globus_dir = '/~/test'
        self.test_dir = '/globus_api_test'
        
        logging.info('Create hpss directory ' + self.test_dir)
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
       
        logging.info('Create globus directory ' + self.globus_dir) 
        if not (operation_manager.dir_exists(test_config.globus_test_ep_id,
            '/~/', self.globus_dir[3:])):
            operation_manager.mkdir(test_config.globus_test_ep_id,
                self.globus_dir)
        
        logging.info('Create globus directory ' + self.globus_dir + '/dirA') 
        operation_manager.mkdir(test_config.globus_test_ep_id,
                self.globus_dir + '/dirA')
        logging.info('Create globus directory ' + self.globus_dir + '/dirB') 
        operation_manager.mkdir(test_config.globus_test_ep_id,
                self.globus_dir+'/dirB')
        logging.info('Create globus directory ' + self.globus_dir + '/dirC')  
        operation_manager.mkdir(test_config.globus_test_ep_id,
                self.globus_dir+'/dirC')
        
        logging.info('Transfer files into globus directories')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id, test_config.globus_test_ep_id,
                '/share/godata/file1.txt', self.globus_dir + '/dirA/file1.txt')
    
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id, test_config.globus_test_ep_id,
                '/share/godata/file2.txt', self.globus_dir + '/dirB/file2.txt')

        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id, test_config.globus_test_ep_id,
                '/share/godata/file3.txt', self.globus_dir + '/dirC/file3.txt')
        
        self.cksm = checksum()
        logging.info('Setup complete')

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
            
        del_result = operation_manager.recursive_delete_dir(
                test_config.globus_test_ep_id, self.globus_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)
        
        del_result = operation_manager.recursive_delete_dir(
                test_config.hpss_ep_id, self.test_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)
        
        logging.info('Tear down complete') 

    def test_recursive_transfer(self):
        logging.info('Running test: test_recursive_transfer')         
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)

        transfer_result = transfer_manager.recursive_transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                self.globus_dir, self.test_dir)
        task_id = transfer_result['task_id']
        
        self.verify_task_completion(task_id)
        logging.info('Verifying recursive transfer')
        self.verify_transfer_to_hpss(self.test_dir + '/dirA', 'file1.txt')
        self.verify_transfer_to_hpss(self.test_dir + '/dirB', 'file2.txt')
        self.verify_transfer_to_hpss(self.test_dir + '/dirC', 'file3.txt')

        self.verify_no_checksum_stored(self.test_dir + '/dirA/file1.txt')
        self.verify_no_checksum_stored(self.test_dir + '/dirB/file2.txt')
        self.verify_no_checksum_stored(self.test_dir + '/dirC/file3.txt')
        logging.info('Recursive transfer verified') 
        logging.info('Test test_recursive_transfer passed')
    
    def test_recursive_transfer_delete_extra(self):
        logging.info('Running test: test_recursive_transfer_delete_extra')         
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
        
        logging.info('Transfer extra file')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/file2.txt', self.test_dir + '/extra.txt')
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, 'extra.txt')
        
        transfer_result = transfer_manager.recursive_transfer_file_delete_extra(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                self.globus_dir, self.test_dir)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        logging.info('Verifying recursive transfer')
        self.verify_transfer_to_hpss(self.test_dir + '/dirA', 'file1.txt')
        self.verify_transfer_to_hpss(self.test_dir + '/dirB', 'file2.txt')
        self.verify_transfer_to_hpss(self.test_dir + '/dirC', 'file3.txt')

        self.verify_no_checksum_stored(self.test_dir + '/dirA/file1.txt')
        self.verify_no_checksum_stored(self.test_dir + '/dirB/file2.txt')
        self.verify_no_checksum_stored(self.test_dir + '/dirC/file3.txt')
        
        logging.info('Recursive transfer verified') 
        logging.info('Verify extra file deleted')
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
                self.test_dir, 'extra.txt'))
        logging.info('Extra file was deleted')

        logging.info('Test test_recursive_transfer_delete_extra passed')

    def test_recursive_transfer_delete_extra_no_permission(self):
        logging.info('Running test: test_recursive_transfer_delete_extra_' +
                'no_permission')         
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
        
        operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir+'/extra')

        logging.info('Transfer extra file')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/file2.txt', self.test_dir + '/extra/extra.txt')
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir+'/extra', 'extra.txt')
        
        logging.info('Chmod ' + self.test_dir + '/extra to 000')
        hpssclapi.Chmod(self.test_dir + '/extra', 000)
        
        transfer_result = transfer_manager.recursive_transfer_file_delete_extra(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                self.globus_dir, self.test_dir)
        task_id = transfer_result['task_id']
        self.assertFalse(operation_manager.complete_task(task_id), 
           'Task completed when it should have failed')
        self.assertFalse(operation_manager.task_successful(task_id),
            'Task successful when it should have failed') 
        
        logging.info('Verifying files transfered')
        self.verify_transfer_to_hpss(self.test_dir + '/dirA', 'file1.txt')
        self.verify_transfer_to_hpss(self.test_dir + '/dirB', 'file2.txt')
        self.verify_transfer_to_hpss(self.test_dir + '/dirC', 'file3.txt')

        self.verify_no_checksum_stored(self.test_dir + '/dirA/file1.txt')
        self.verify_no_checksum_stored(self.test_dir + '/dirB/file2.txt')
        self.verify_no_checksum_stored(self.test_dir + '/dirC/file3.txt')
        
        logging.info('file transfer verified') 
 
        logging.info('Chmod ' + self.test_dir + ' to 777')
        hpssclapi.Chmod(self.test_dir + '/extra', 0777)
       
        logging.info('Verify extra file not deleted')
        self.assertTrue(operation_manager.file_exists(test_config.hpss_ep_id,
                self.test_dir+'/extra', 'extra.txt'))
        logging.info('Extra file was not deleted')
        
        logging.info('Test test_recursive_transfer_delete_extra_no ' +
                '_permission passed') 
# These tests fail there appears to be a bug when transfering recursivly
# from HPSS to globus
class recursive_transfer_to_globus(sdk_system_tests):

    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()
        self.globus_dir = '/~/test'
        self.test_dir = '/globus_api_test'
        
        logging.info('Create globus directory ' + self.globus_dir)
        if not (operation_manager.dir_exists(test_config.globus_test_ep_id,
            '/~/', self.globus_dir[3:])):
            operation.mkdir(test_config.globus_test_ep_id, self.globus_dir)

        logging.info('Create hpss directory ' + self.test_dir)
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
        
        logging.info('Create hpss directory ' + self.test_dir + '/dirA')
        operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir +
                '/dirA')
         
        logging.info('Create hpss directory ' + self.test_dir + '/dirB')
        operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir +
                '/dirB')
 
        logging.info('Create hpss directory ' + self.test_dir + '/dirC')
        operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir +
                '/dirC')
        
        hints = hpssclapi.cos_hints()
        priorities = hpssclapi.cos_priorities()
        logging.info('Create hpss file ' + self.test_dir + '/dirA/file1.txt')
        hpssclapi.Creat(self.test_dir + '/dirA/file1.txt', 0777, hints, priorities)
        logging.info('Create hpss file ' + self.test_dir + '/dirB/file1.txt')
        hpssclapi.Creat(self.test_dir + '/dirB/file1.txt', 0777, hints, priorities)
        logging.info('Create hpss file ' + self.test_dir + '/dirC/file1.txt')
        hpssclapi.Creat(self.test_dir + '/dirC/file1.txt', 0777, hints, priorities)
        logging.info('Setup complete')

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
            
        del_result = operation_manager.recursive_delete_dir(
                test_config.globus_test_ep_id, self.globus_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id, 600, 20)
        
        del_result = operation_manager.recursive_delete_dir(
                test_config.hpss_ep_id, self.test_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id, 600, 20)
    
    @unittest.skip('Need to fix dsi bug that prevents HPSS -> globus ' +
            'transfers')
    def test_recursive_transfer_to_globus(self):
        logging.info('Running test: test_recursive_transfer_to_globus')         
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)

        transfer_result = transfer_manager.recursive_transfer_file(
                test_config.hpss_ep_id, test_config.globus_test_ep_id,
                self.test_dir, self.globus_dir)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        logging.info('Verifying recursive transfer')
        self.verify_transfer_to_globus(self.globus_dir + '/dirA', 'file1.txt')
        self.verify_transfer_to_globus(self.globus_dir + '/dirB', 'file2.txt')
        self.verify_transfer_to_globus(self.globus_dir + '/dirC', 'file3.txt')
        
        logging.info('Recursive transfer verified') 
        logging.info('Test test_recursive_transfer passed')
    
    def test_recursive_transfer_to_globus_delete_extra(self):
        logging.info('Running test: test_recursive_transfer_to_globus_' +
                'delete_extra')
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
        
        logging.info('Transfer extra file')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id, test_config.globus_test_ep_id,
                '/share/godata/file2.txt', self.globus_dir + '/extra.txt')
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_globus(self.globus_dir, 'extra.txt')
        
        transfer_result = transfer_manager.recursive_transfer_file_delete_extra(
                test_config.hpss_ep_id, test_config.globus_test_ep_id,
                self.test_dir, self.globus_dir)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        logging.info('Verifying recursive transfer')
        self.verify_transfer_to_globus(self.globus_dir + '/dirA', 'file1.txt')
        self.verify_transfer_to_globus(self.globus_dir + '/dirB', 'file2.txt')
        self.verify_transfer_to_globus(self.globus_dir + '/dirC', 'file3.txt')
        logging.info('Recursive transfer verified') 
        
        logging.info('Verify extra file deleted')
        self.assertFalse(operation_manager.file_exists(test_config.globus_test_ep_id,
                self.globus_dir, 'extra.txt'))
        logging.info('Extra file was deleted')

        logging.info('Test test_recursive_transfer_to_globus_delete_extra' +
                ' passed')

class large_recursive_transfer_to_hpss(sdk_system_tests):
    
    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        self.start_dsi_config = self.dsi_config.get_config()

        self.globus_dir = '/~/large_transfer'
        self.test_dir = '/globus_api_test'

        logging.info('Create hpss directory ' + self.test_dir)
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)

        logging.info('Createing globus directories')
        if not (operation_manager.dir_exists(test_config.globus_test_ep_id, '/',
            self.globus_dir[3:])):
            operation_manager.mkdir(test_config.globus_test_ep_id, self.globus_dir)

        operation_manager.mkdir(test_config.globus_test_ep_id, self.globus_dir + '/dirA')
        operation_manager.mkdir(test_config.globus_test_ep_id, self.globus_dir + '/dirB')
        operation_manager.mkdir(test_config.globus_test_ep_id, self.globus_dir + '/dirC')
        

        for directory in ['dirA', 'dirB', 'dirC']:
            for i in range(0, 100):
                transfer_result = transfer_manager.transfer_file(
                    test_config.globus_test_ep_id,
                    test_config.globus_test_ep_id,
                    '/share/godata/file1.txt', '/~/large_transfer/' +
                    directory + '/file' + str(i))
                task_id = transfer_result['task_id']
                self.verify_task_completion(task_id, 300, 1)

    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
            
        del_result = operation_manager.recursive_delete_dir(
                test_config.globus_test_ep_id, self.globus_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id, 1800, 60)

        del_result = operation_manager.recursive_delete_dir(
                test_config.hpss_ep_id, self.test_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id, 1800, 60)

    def test_large_recursive_transfer(self):
        logging.info('Running test: test_large_recursive_transfer')
        
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_config.default_config)
            
        transfer_result = transfer_manager.recursive_transfer_file_checksum_option(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                self.globus_dir, self.test_dir)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id, 1800, 60)

class large_recursive_transfer_to_globus(sdk_system_tests):
    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.source_dir = '/large_transfer'
        self.dest_dir = '/~/large_transfer_dest'

        operation_manger.mkdir(test_config.globus_test_ep_id, self.dest_dir)
        
        operation_manager.mkdir(test_config.hpss_ep_id, self.source_dir)
        operation_manager.mkdir(test_config.hpss_ep_id, self.source_dir + '/dirA')
        operation_manager.mkdir(test_config.hpss_ep_id, self.source_dir + '/dirB')
        operation_manager.mkdir(test_config.hpss_ep_id, self.source_dir + '/dirC')
        
        hints = hpssclapi.cos_hints()
        priorities = hpssclapi.cos_priorities()
        logging.info('Create hpss file ' + self.test_dir + '/file1.txt')
        if test_config.auth_mech == 'unix':
            mech = hpssclapi.hpss_authn_mech_unix
        elif test_config.auth_mech == 'kerberos':
            mech = hpssclapi.hpss_authn_mech_kerberos
        hpssclapi.SetLoginCred(test_config.grid_user, mech, 
                hpssclapi.hpss_rpc_cred_client,
                hpssclapi.hpss_rpc_auth_type_keytab, test_config.auth)
       
        for directory in ['dirA', 'dirB', 'dirC']:
            for i in range(0, 100):
                fd = hpssclapi.Creat(self.source_dir + '/' +directory +
                        '/file' + i, 0777, hints, priorities)[0]
                hpssclapi.Write(fd, bytearray(os.urandom(4)), 4)
                hpssclapi.Close(fd)
        hpssclapi.PurgeLoginCred()
    
    def tearDown(self):
        logging.info('Tear down test')
        self.dsi_config.load_config(self.start_dsi_config)
            
        del_result = operation_manager.recursive_delete_dir(
                test_config.globus_test_ep_id, self.dest_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id, 1800, 60)
        
        del_result = operation_manager.recursive_delete_dir(
                test_config.hpss_ep_id, self.source_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id, 1800, 60)
    
    @unittest.skip('There is a bug concerning recursive transfers to globus')
    def test_large_recursive_transfer_to_globus(self):
        logging.info('Running test: test_large_recursive_transfer_to_globus')
        
        logging.info('DSI config set uda checksum and checksum support off')
        self.dsi_config.load_config(self.dsi_conifg.default_dsi_config)
            
        transfer_result = transfer_manager.recursive_transfer_file_checksum_option(
                test_config.hpss_ep_id,test_config.globus_test_ep_id,
                self.globus_dir, self.test_dir)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id, 1800, 60)
        logging.info('Test test_recursive_transfer_to_globus passed')
    
class delete_tests(sdk_system_tests):
    
    def setUp(self):
        logging.info('\n')
        logging.info('Setup test')
        self.test_dir = '/globus_api_test'
        self.file_name = 'file1.txt'
        self.hpss_file = self.test_dir + '/' + self.file_name

        self.cksm = checksum()

        logging.info('create hpss directory ' + self.test_dir)
        if not (operation_manager.dir_exists(test_config.hpss_ep_id, '/',
            self.test_dir[1:])):
            operation_manager.mkdir(test_config.hpss_ep_id, self.test_dir)
 
    def tearDown(self):
        logging.info('Tear down test')

        if (operation_manager.dir_exists(test_config.hpss_ep_id,
            '/', self.test_dir[1:])):
            if (operation_manager.file_exists(test_config.hpss_ep_id,
                self.test_dir, self.file_name)):
                logging.info('Delete file ' + self.hpss_path)
                hpssclapi.Unlink(self.hpss_path)

        if (operation_manager.dir_exists(test_config.hpss_ep_id,
            '/', self.test_dir[1:])):
            logging.info('Delete dir ' + self.test_dir)
            hpssclapi.Rmdir(self.test_dir)
        logging.info('Tear down complete')

    def test_delete_file(self):
        logging.info('Running test: test_delete_file')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)

        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)

        del_result = operation_manager.delete_file(test_config.hpss_ep_id,
                self.hpss_path)
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)

        logging.info('Verify file deleted')
        self.assertFalse(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File was not deleted')
        logging.info('Test test_delete_file passed')

    def test_delete_dir(self): 
        logging.info('Running test: test_delete_dir')
        del_result = operation_manager.recursive_delete_dir(
                test_config.hpss_ep_id, self.test_dir) 
        task_id = del_result['task_id']
        self.verify_task_completion(task_id)
        logging.info('Verify dir deleted')
        self.assertFalse(operation_manager.dir_exists(test_config.hpss_ep_id,
            '/', self.test_dir), 'Directory was not deleted')

        logging.info('Test test_delete_dir passed')

    def test_delete_file_without_permission(self):
        logging.info('Running test: test_delete_file_without_permissions')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)
        
        logging.info('Chmod ' + self.test_dir + ' to 000')
        hpssclapi.Chmod(self.test_dir, 000)
        try:
            operation_manager.delete_file(test_config.hpss_ep_id,
                    self.hpss_path)
        except IOError:
            logging.info('IOError thrown as expected')
            logging.info('Chmod ' + self.test_dir + ' to 777')
            hpssclapi.Chmod(self.test_dir, 0777)
        except:
            logging.exception('Unexpected exception thrown')
            logging.info('Chmod ' + self.test_dir + ' to 777')
            hpssclapi.Chmod(self.test_dir, 0777)
            raise

        self.assertTrue(operation_manager.complete_task(task_id), 
            'Transfer task did not complete')
        self.assertTrue(operation_manager.task_successful(task_id),
            'Transfer task submission failed') 
       
        self.assertTrue(operation_manager.file_exists(test_config.hpss_ep_id,
            self.test_dir, self.file_name), 'File was deleted')
 
        logging.info('Test test_delete_file_without_permission passed')

    def test_delete_dir_without_permission(self):
        logging.info('Running test: test_delete_dir_without_permissions')
        transfer_result = transfer_manager.transfer_file(
                test_config.globus_test_ep_id,test_config.hpss_ep_id,
                '/share/godata/' + self.file_name, self.hpss_path)
        task_id = transfer_result['task_id']
        self.verify_task_completion(task_id)
        self.verify_transfer_to_hpss(self.test_dir, self.file_name)
        self.verify_no_checksum_stored(self.hpss_path)
        
        logging.info('Chmod ' + self.test_dir + ' to 000')
        hpssclapi.Chmod(self.test_dir, 000)
        try:
            operation_manager.recursive_delete_dir(test_config.hpss_ep_id,
                    self.test_dir)
        except IOError:
            logging.info('IOError thrown as expected')
            logging.info('Chmod ' + self.test_dir + ' to 000')
            hpssclapi.Chmod(self.test_dir, 0777)
        except:
            logging.exception('Unexpected exception thrown')
            logging.info('Chmod ' + self.test_dir + ' to 000')
            hpssclapi.Chmod(self.test_dir, 0777)
            raise

        self.assertTrue(operation_manager.complete_task(task_id), 
            'Transfer task did not complete')
        self.assertTrue(operation_manager.task_successful(task_id),
            'Transfer task submission failed') 
       
        self.assertTrue(operation_manager.dir_exists(test_config.hpss_ep_id,
            '/', self.test_dir[1:]), 'dir was deleted')
 
        logging.info('Test test_delete_dir_without_permission passed')

def main(out, verbosity=2):
    loader = unittest.TestLoader()
    log_arg = 0
    
    logger = logging.getLogger()

    if sys.argv[-1] == 'debug' or sys.argv[-1] == '-d':
        logger.setLevel(logging.DEBUG)
        log_arg = 1
    elif sys.argv[-1] == 'info' or sys.argv[-1] == '-i':
        logger.setLevel(logging.INFO)
        log_arg = 1
    elif sys.argv[-1] == 'warning'or sys.argv[-1] == '-w':
        logger.setLevel(logging.WARNING)
        log_arg = 1
    elif sys.argv[-1] == 'error' or sys.argv[-1] == '-e':
        logger.setLevel(logging.ERROR)
        log_arg = 1
    elif sys.argv[-1] == 'critical' or sys.argv[-1] == '-c':
        logger.setLevel(logging.CRITICAL)
        log_arg = 1
    else:
        logger.setLevel(logging.INFO)

    
    if len(sys.argv) == 1 + log_arg:
        suite = loader.loadTestsFromModule(sys.modules[__name__])
    if len(sys.argv) == 2 + log_arg:
        suite = loader.loadTestsFromName(__name__+'.'+sys.argv[1])
    unittest.TextTestRunner(stream=out, verbosity=verbosity).run(suite)

if __name__ == '__main__':
    log_file = 'logs/log-'+str(time.time())
    log_format='%(asctime)s %(levelname)s: %(message)s'
    logging.basicConfig(filename=log_file, format=log_format)
    logger = logging.getLogger()
    ch = logging.StreamHandler(sys.stdout)
    ch.setLevel(logging.INFO)
    logger.addHandler(ch)

    token_manager = globus_sdk_tokens()   
    authorizer = token_manager.authorize()
    transfer_manager = globus_sdk_transfer(authorizer)
    operation_manager = globus_sdk_operations(authorizer)
    endpoint_manager = globus_sdk_endpoints(authorizer)
    
    endpoint_manager.auto_activate(test_config.hpss_ep_id)

    print 'Testing started results logged in ' + log_file
    with open(log_file, 'a') as f:
        main(f)
    print 'Testing complete!'
    print 'Results logged in ' + log_file
