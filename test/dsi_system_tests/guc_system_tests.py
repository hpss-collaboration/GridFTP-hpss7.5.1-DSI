# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/guc_system_tests.py $ $Id: guc_system_tests.py 16 2017-01-27 23:49:10Z gregthor $ $Rev: 16 $
import logging, sys, os,  time, shutil, hashlib, socket, unittest
from pwd import getpwnam

from globus_url_copy_transfer import globus_url_copy_transfer
from dsi_configuration import dsi_configuration
from checksum import checksum

from directory import directory
from hpss_file import hpss_file
from local_file import local_file

import test_config
import hpssclapi 

import multiprocess_func

class guc_system_tests(unittest.TestCase):
    guc = globus_url_copy_transfer() 
    dir_class = directory()
    h_file = hpss_file()
    l_file = local_file()

    def compare_checksum(self, local_path, hpss_path):
        cksm = checksum()
        local_cksm = cksm.calc_local_checksum(local_path, hashlib.md5())
        hpss_cksm = cksm.calc_hpss_checksum(hpss_path, hashlib.md5())
        self.assertEqual(local_cksm, hpss_cksm, 'Checksum of local file ' +
                local_path + 'and hpss file ' + hpss_path + ' not equal')

    def verify_transfer_to_hpss(self, local_path, hpss_path):
        self.compare_checksum(local_path, hpss_path)
        hpss_stat = self.h_file.fstat(hpss_path)
        self.assertTrue(self.hpss_file_user_correct(hpss_stat),
                msg='hpss file owner incorrect')
        self.assertEqual(hpss_stat.st_mode, 33188, 'Mode not equal to ' +
                'expected value after transfer to hpss')

    def verify_transfer_from_hpss(self, local_path, hpss_path):
        self.compare_checksum(local_path, hpss_path)
        local_stat = os.stat(local_path)
        self.assertTrue(self.local_file_user_correct(local_stat),
                msg='local file owner incorrect')

        self.assertEqual(local_stat.st_mode, 33188,
                'Mode not equal to expected value after transfer from hpss')
  
    def hpss_file_user_correct(self, hpss_stat):
        user_info = hpssclapi.SECGetCredsByName(test_config.grid_user)
        return (user_info.Uid == hpss_stat.st_uid and 
                 user_info.Gid == hpss_stat.st_gid)
    
    def local_file_user_correct(self, local_stat):
        user_info = getpwnam(test_config.grid_user)
        if user_info.pw_uid != local_stat.st_uid:
            return False
        if user_info.pw_gid != local_stat.st_gid:
            return False
        return True
 
class basic_transfer(guc_system_tests):
    
    def setUp(self):
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        logging.info('Saving starting DSI configuration')
        self.start_dsi_config = self.dsi_config.get_config()
        
        self.cksm = checksum()
        logging.info('Creating local file for test')
        self.path = self.l_file.create_temp_file(10)
        os.chmod(self.path, 0777) 
        self.hpss_path = self.path.replace('/tmp/','/')
        self.source = 'file://' + self.path
        self.destination = 'gsiftp://' + socket.gethostname() +'/' 
        logging.info('Setup complete')
    
    def tearDown(self):
        logging.info('Tear down test')
        logging.info('Loading starting DSI configuration')
        self.dsi_config.load_config(self.start_dsi_config)
        logging.info('Delete local file ' + self.path)
        self.l_file.delete_file(self.path)
        logging.info('Delete hpss file ' + self.hpss_path)
        self.h_file.delete_file(self.hpss_path) 
        logging.info('Tear down complete\n')

    def test_transfer_to_hpss(self):
        logging.info('Running test: test_transfer_to_hpss')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('Transfer verified')
        logging.info('Test test_transfer_to_hpss passed')

    def test_transfer_to_hpss_uda_support(self):
        logging.info('Running test: test_transfer_to_hpss_uda_support')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('Transfer verified')
        logging.info('Test test_transfer_to_hpss_uda_support passed')
    
    def test_transfer_to_hpss_cksm_support(self):
        logging.info('Running test: test_transfer_to_hpss_cksm_support')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('Transfer verified')
        logging.info('Test test_transfer_to_hpss_cksm_support passed')
    
    def test_transfer_checksum_option(self):
        logging.info('Running test: test_transfer_checksum_option')
        self.guc.transfer_file_checksum_option(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('Transfer verified')
        logging.info('Test test_transfer_checksum_option passed')

    def test_transfer_no_write_perms(self):
        logging.info('Running test: test_transfer_no_write_perms')
        self.dir_class.create_hpss_directory('/guc_tmp/', 0555)

        self.destination += 'guc_tmp/'
        self.hpss_path = '/guc_tmp/' + self.hpss_path
        self.guc.transfer_file(self.source, self.destination)
        self.assertFalse(self.h_file.file_exists(self.hpss_path),
                msg='File exists when it should not')
        logging.info('File did not transfer as expected')

        hpssclapi.Rmdir('/guc_tmp')
        logging.info('Test test_transfer_no_write_perms passed')

class store_checksum(guc_system_tests):
    
    def setUp(self):
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        logging.info('Saving starting DSI configuration')
        self.start_dsi_config = self.dsi_config.get_config()
       
        self.cksm = checksum()
        
        logging.info('Creating local file for test')
        self.path = self.l_file.create_temp_file(10)
        os.chmod(self.path, 0777) 
        self.hpss_path = self.path.replace('/tmp/','/')
        self.source = 'file://' + self.path
        self.destination = 'gsiftp://' + socket.gethostname() +'/' 
        logging.info('Setup complete')

    def tearDown(self):
        logging.info('Tear down test')
        logging.info('Loading starting dsi configuration')
        self.dsi_config.load_config(self.start_dsi_config)
        logging.info('Delete local file ' + self.path)
        self.l_file.delete_file(self.path)
        logging.info('Delete hpss file ' + self.hpss_path)
        self.h_file.delete_file(self.hpss_path) 
        logging.info('Tear down complete\n')

    def test_store_file_cksm(self):
        logging.info('Running test: test_store_file_cksm')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        
        self.guc.transfer_file_checksum_option(self.source, self.destination)
        
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.md5())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm passed')

    def test_store_file_cksm_md5(self):
        logging.info('Running test: test_store_file_cksm_md5')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "md5")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.md5())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm_md5 passed')

    def test_store_file_cksm_sha1(self):
        logging.info('Running test: test_store_file_cksm_sha1')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha1")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha1())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm_sha1 passed')

    def test_store_file_cksm_sha224(self):
        logging.info('Running test: test_store_file_cksm_sha224')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha224")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)   
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha224())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'], 
                'Destination checksum does not equal stored checksum') 

        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm_sha224 passed')

    def test_store_file_cksm_sha256(self):
        logging.info('Running test: test_store_file_cksm_sha256')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha256")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)   
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha256())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm_sha256 passed')

    def test_store_file_cksm_sha384(self):
        logging.info('Running test: test_store_file_cksm_sha384')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha384")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha384())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm_sha384 passed')

    def test_store_file_cksm_sha512(self):
        logging.info('Running test: test_store_file_cksm_sha512')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha512")         
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored on ' +
                'file')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha512())
        stored_cksm = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm.upper(), stored_cksm['digest'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_file_cksm_sha512 passed')
    
    def test_store_uda_cksm(self):
        logging.info('Running test: test_store_uda_cksm')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.md5())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm['checksum'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_uda_cksm passed')
    
    def test_store_uda_cksm_md5(self):
        logging.info('Running test: test_store_uda_cksm_md5')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "md5")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.md5())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm['checksum'], 
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_uda_cksm_md5 passed')

    def test_store_uda_cksm_sha1(self):
        logging.info('Running test: test_store_uda_sha1')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha1")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha1())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm['checksum'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_uda_cksm_sha1 passed')

    def test_store_uda_cksm_sha224(self):
        logging.info('Running test: test_store_uda_cksm_sha224')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha224")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha224())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm['checksum'],
                'Destination checksum does not equal stored checksum')
        logging.info('Checksums match')
        logging.info('Test test_store_uda_cksm_sha224 passed')

    def test_store_uda_cksm_sha256(self):
        logging.info('Running test: test_store_uda_cksm_sha256')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha256")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha256())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm['checksum'],
                'Destination checksum does not equal stored checksum')
        logging.info('Checksums match')
        logging.info('Test test_store_uda_cksm_sha256 passed')

    def test_store_uda_cksm_sha384(self):
        logging.info('Running test: test_store_uda_cksm_sha384')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha384")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha384())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        
        self.assertEqual(dest_cksm, stored_cksm['checksum'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_store_uda_cksm_sha384 passed')

    def test_store_uda_cksm_sha512(self):
        logging.info('Running test: test_store_uda_cksm_sha512')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file_checksum_option(self.source, self.destination,
                "sha512")
        self.verify_transfer_to_hpss(self.path, self.hpss_path)    
        logging.info('File transfer verified')
        logging.info('Comparing destination checksum to checksum stored in ' +
                'uda')
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path,
                hashlib.sha512())
        stored_cksm = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(dest_cksm, stored_cksm['checksum'],
                'Destination checksum does not equal stored checksum') 
        logging.info('Checksums match')
        logging.info('Test test_uda_cksm_sha512 passed')

class sync_level_three_tests(guc_system_tests):

    def setUp(self):
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        logging.info('Saving starting DSI configuration')
        self.start_dsi_config = self.dsi_config.get_config()
        
        self.cksm = checksum()
        logging.info('Creating local file for test')
        self.path = self.l_file.create_temp_file(10)
        os.chmod(self.path, 0777) 
        self.hpss_path = self.path.replace('/tmp/','/')
        self.source = 'file://' + self.path
        self.destination = 'gsiftp://' + socket.gethostname() +'/' 
        logging.info('Setup complete')

    def tearDown(self): 
        logging.info('Tear down test')
        logging.info('Loading starting dsi configuration')
        self.dsi_config.load_config(self.start_dsi_config)
        logging.info('Delete local file ' + self.path)
        self.l_file.delete_file(self.path)
        logging.info('Delete hpss file ' + self.hpss_path)
        self.h_file.delete_file(self.hpss_path) 
        logging.info('Tear down complete\n')
    
    def test_sync_level_three_no_transfer(self):
        logging.info('Running test: test_sync_level_three_no_transfer')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_sync_option(self.source, self.destination, 3)

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        logging.info('Compare create time retireved by fstat before and ' +
                'after sync transfer')
        self.assertEqual(hpss_stat.hpss_st_ctime, hpss_stat_two.hpss_st_ctime,
                'Creation time before and after sync level 3 transfer are ' +
                'not equal')
            
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
        logging.info('Test test_sync_level_three_no_transfer passed')

    def test_sync_level_three_store_checksum_md5(self):
        logging.info('Running test: test_sync_level_three_store_checksum_md5')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'md5')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime, hpss_stat_two.hpss_st_ctime,
                'Creation times before and after sync level 3 transfer are ' +
                'not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.md5())
        self.assertEqual(dest_cksm.upper(), hpss_cksm_info['digest'],
                'Destination checksum does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_store_checksum_md5 passed')

    def test_sync_level_three_store_checksum_sha1(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_checksum_sha1')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)

        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha1')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time before and after ' +
             'sync level 3 transfer are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha1())
        self.assertEqual(dest_cksm.upper(),
                hpss_cksm_info['digest'], 'Destination checksum ' +
                'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_store_checksum_sha1 passed')

    def test_sync_level_three_store_checksum_sha224(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_checksum_sha224')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)        
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha224')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time before and after ' +
             'sync level 3 transfer are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha224())
        self.assertEqual(dest_cksm.upper(),
                hpss_cksm_info['digest'], 'Destination checksum ' +
                'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_store_checksum_sha224 passed')


    def test_sync_level_three_store_checksum_sha256(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_checksum_sha256')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path) 
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha256')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time before and after ' +
             'sync level 3 transfer are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha256())
        self.assertEqual(dest_cksm.upper(),
                hpss_cksm_info['digest'], 'Destination checksum ' +
                'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_store_checksum_sha256 passed')


    def test_sync_level_three_store_checksum_sha384(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_checksum_sha384')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha384')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime, hpss_stat_two.hpss_st_ctime,
            'Creation times before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha384())
        self.assertEqual(dest_cksm.upper(), hpss_cksm_info['digest'],
                'Destination checksum does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_store_checksum_sha384 passed')

    def test_sync_level_three_store_checksum_sha512(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_checksum_sha512')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha512')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation times ' +
			'before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha512())
        self.assertEqual(dest_cksm.upper(),
                hpss_cksm_info['digest'], 'Destination checksum ' +
                'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_store_checksum_sha512 passed')

    def test_sync_level_three_store_uda_checksum_md5(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_uda_checksum_md5')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'md5')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.md5())
        self.assertEqual(dest_cksm,
                hpss_cksm_info['checksum'], 'Destination checksum ' +
	            'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_uda_store_checksum_md5 ' +
                'passed')

    def test_sync_level_three_store_uda_checksum_sha1(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_uda_checksum_sha1')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha1')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
       
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha1())
        self.assertEqual(dest_cksm,
                hpss_cksm_info['checksum'], 'Destination checksum ' +
	            'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_uda_store_checksum_sha1 ' +
                'passed')

    def test_sync_level_three_store_uda_checksum_sha224(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_uda_checksum_sha224')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha224')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha224())
        self.assertEqual(dest_cksm,
                hpss_cksm_info['checksum'], 'Destination checksum ' +
	            'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_uda_store_checksum_sha224 ' +
                'passed')

    def test_sync_level_three_store_uda_checksum_sha256(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_uda_checksum_sha256')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)

        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)             
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha256')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha256())
        self.assertEqual(dest_cksm,
                hpss_cksm_info['checksum'], 'Destination checksum ' +
	            'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_uda_store_checksum_sha256 ' +
                'passed')

    def test_sync_level_three_store_uda_checksum_sha384(self):
        logging.info('Running test: ' +
                'test_sync_level_three_store_uda_checksum_sha384')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)   
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)        
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha384')

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer ' +
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha384())
        self.assertEqual(dest_cksm,
                hpss_cksm_info['checksum'], 'Destination checksum ' +
	            'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_uda_store_checksum_sha384 ' +
                'passed')

    def test_sync_level_three_store_uda_checksum_sha512(self):
        logging.info('Running test: ' + 
                'test_sync_level_three_store_uda_checksum_sha512')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path) 
        hpss_stat = self.h_file.fstat(self.hpss_path)
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha512')
         
        logging.info('fstat hpss file ' + self.hpss_path)        
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer '+
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
       
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.sha512())
        self.assertEqual(dest_cksm,
                 hpss_cksm_info['checksum'], 'Destination checksum ' +
	             'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_sync_level_three_uda_store_checksum_sha512 ' +
                'passed')

    def test_do_not_overwrite_uda_checksum(self):
        logging.info('Running test: test_do_not_overwrite_uda_checksum')
        logging.info('Loading uda checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.uda_support_config)
        
        self.guc.transfer_file_checksum_option(self.source,
                self.destination, 'md5')
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)  
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha512')
        
        logging.info('fstat hpss file ' + self.hpss_path)        
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_uda_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer '+
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
       
        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.md5())
        self.assertEqual(dest_cksm,
                hpss_cksm_info['checksum'], 'Destination checksum ' +
	            'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_do_not_overwrite_uda_checksum passed')

    def test_do_not_overwrite_checksum(self):
        logging.info('Running test: test_do_not_overwrite_checksum')
        logging.info('Loading checksum support DSI configuration')
        self.dsi_config.load_config(self.dsi_config.cksm_support_config)
        
        self.guc.transfer_file_checksum_option(self.source,
                self.destination, 'md5')
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path) 
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_checksum_sync_option(self.source,
                self.destination, 3, 'sha512')

        logging.info('fstat hpss file ' + self.hpss_path)        
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        hpss_cksm_info = self.cksm.get_stored_checksum_info(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer '+
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')

        dest_cksm = self.cksm.calc_hpss_checksum(self.hpss_path, hashlib.md5())
        self.assertEqual(dest_cksm.upper(),
                hpss_cksm_info['digest'], 'Destination checksum ' +
                'does not equal stored checksum')
        logging.info('Destination checksum equals stored checksum')
        logging.info('Test test_do_not_overwrite_checksum passed')

class sync_tests(guc_system_tests):

    def setUp(self):
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        logging.info('Saving starting DSI configuration')
        self.start_dsi_config = self.dsi_config.get_config()
        
        self.cksm = checksum()
        logging.info('Creating local file for test')
        self.path = self.l_file.create_temp_file(10)
        os.chmod(self.path, 0777) 
        self.hpss_path = self.path.replace('/tmp/','/')
        self.source = 'file://' + self.path
        self.destination = 'gsiftp://' + socket.gethostname() +'/' 
        logging.info('Setup complete')
 
    def tearDown(self): 
        logging.info('Tear down test')
        logging.info('Loading starting dsi configuration')
        self.dsi_config.load_config(self.start_dsi_config)
        logging.info('Delete local file ' + self.path)
        self.l_file.delete_file(self.path)
        logging.info('Delete hpss file ' + self.hpss_path)
        self.h_file.delete_file(self.hpss_path) 
        logging.info('Tear down complete\n')
 
    def test_transfer_sync_level_zero(self): 
        logging.info('Running test: test_transfer_sync_level_zero')
        self.guc.transfer_file_sync_option(self.source,
                self.destination, 0)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        logging.info('Test test_transfer_sync_level_zero passed')
           
    def test_no_transfer_sync_level_zero(self):
        logging.info('Running test: test_no_transfer_sync_level_zero')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_sync_option(self.source,
                self.destination, 0)
        
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer '+
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
        logging.info('Test test_no_transfer_sync_level_zero passed')

    def test_transfer_sync_level_one(self):
        logging.info('Running test: test_transfer_sync_level_one')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
    
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        logging.info('Append to local file ' + self.path)
        with open(self.path, 'a') as f:
            f.write('append')
        
        logging.info('Wait 2 seconds')
        time.sleep(2)
        self.guc.transfer_file_sync_option(self.source,
                self.destination, 1)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        
        logging.info('fstat hpss file ' + self.hpss_path) 
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        self.assertLess(hpss_stat.hpss_st_ctime,
            hpss_stat_two.hpss_st_ctime, 'Creation time taken after second ' +
            'transfer not less than time taken after first transfer')

        logging.info('File transfer occured')
        logging.info('Test test_transfer_sync_level_one passed')

    def test_no_transfer_sync_level_one(self):       
        logging.info('Running test: test_no_transfer_sync_level_one')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
    
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        self.guc.transfer_file_sync_option(self.source,
                self.destination, 1)

        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer '+
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
        logging.info('Test test_no_transfer_sync_level_one passed')

    def test_transfer_sync_level_two(self):
        logging.info('Running test: test_transfer_sync_level_two')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
    
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        with open(self.path, 'a') as f:
            f.write('append')
        
        logging.info('Wait 2 seconds')
        time.sleep(2)       
        
        self.guc.transfer_file_sync_option(self.source,
                self.destination, 2)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
    
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        self.assertLess(hpss_stat.hpss_st_ctime,
            hpss_stat_two.hpss_st_ctime, 'Creation time taken after second ' +
            'transfer not less than time taken after first transfer')

        logging.info('File transfer occured')
        logging.info('Test test_transfer_sync_level_two passed')
   
    def test_no_transfer_sync_level_two(self):
        logging.info('Running test: test_no_transfer_sync_level_two')
        self.guc.transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
    
        logging.info('fstat hpss file ' + self.hpss_path)
        hpss_stat = self.h_file.fstat(self.hpss_path)
        
        self.guc.transfer_file_sync_option(self.source,
                self.destination, 2)
        
        hpss_stat_two = self.h_file.fstat(self.hpss_path)
        self.assertEqual(hpss_stat.hpss_st_ctime,
			 hpss_stat_two.hpss_st_ctime, 'Creation time ' +
			'before and after sync level 3 transfer '+
			'are not equal')
        logging.info('Create time before and after sync transfer equal')
        logging.info('No file transfer has occured')
        logging.info('Test test_no_transfer_sync_level_two passed')
       
class create_directory(guc_system_tests):
    
    def setUp(self):
        logging.info('Setup test')
        self.dsi_config = dsi_configuration()
        logging.info('Saving starting DSI configuration')
        self.start_dsi_config = self.dsi_config.get_config()
        
        self.cksm = checksum()
        logging.info('Creating local file for test')
        self.path = self.l_file.create_temp_file(10)
        os.chmod(self.path, 0777) 
        self.hpss_path = self.path.replace('/tmp/','/guc_tmp/')
        self.source = 'file://' + self.path
        self.destination = 'gsiftp://' + socket.gethostname() +'/guc_tmp/' 
        logging.info('Setup complete')
 
    def tearDown(self):
        logging.info('Tear down test')
        logging.info('Loading starting dsi configuration')
        self.dsi_config.load_config(self.start_dsi_config)
        logging.info('Delete local file ' + self.path)
        self.l_file.delete_file(self.path)
        logging.info('Delete hpss file ' + self.hpss_path)
        self.h_file.delete_file(self.hpss_path)
        logging.info('Delete hpss directory /guc_tmp')
        hpssclapi.Rmdir('/guc_tmp')
        logging.info('Tear down complete\n')
     
    def test_directory_exists(self):
        logging.info('Running test: test_directory_exists')
        self.dir_class.create_hpss_directory('/guc_tmp', 16895)
        
        self.guc.create_directory_transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
    
        logging.info('stat hpss dir /guc_tmp')
        hpss_stat = hpssclapi.Stat('/guc_tmp')    
        self.assertEqual(hpss_stat.st_mode, 16895, "Directory mode incorrect")
        logging.info('Test test_directory_exists passed')

    def test_directory_does_not_exist(self):
        logging.info('Running test: test_directory_does_not_exist') 
        self.guc.create_directory_transfer_file(self.source, self.destination)
        self.verify_transfer_to_hpss(self.path, self.hpss_path)
        logging.info('File transfer verified')
        logging.info('Test test_directory_does_not_exist passed')
    
class small_recursive_transfer_to_hpss(guc_system_tests):
    
    def setUp(self):
        logging.info('Setup test')

        self.base_dir = '/tmp/guc_tmp/'
        self.source = 'file://' + self.base_dir
        self.destination = 'gsiftp://' + socket.gethostname() +'/guc_tmp/' 

        self.dir_class.create_local_directory(self.base_dir)
        self.dir_class.create_hpss_directory('/guc_tmp', 16895)
               
        self.dir_class.create_local_directory(self.base_dir + 'a')
        self.dir_class.create_local_directory(self.base_dir + 'b')
        self.dir_class.create_local_directory(self.base_dir + 'c')
        
        logging.info('Chmod created local direcotories to mode 777')
        os.chmod(self.base_dir + 'a', 0777)
        os.chmod(self.base_dir + 'b', 0777)
        os.chmod(self.base_dir + 'c', 0777)
        
        self.fileA = self.base_dir + 'a/a.txt'
        self.fileB = self.base_dir + 'b/b.txt'
        self.fileC = self.base_dir + 'c/c.txt'
        
        logging.info('Create files ' + self.fileA + ', ' + self.fileB + ', ' +
                self.fileC)

        with open(self.fileA, 'w') as f:
            f.write('test file A')
        with open(self.fileB, 'w') as f:
            f.write('test file B')
        with open(self.fileC, 'w') as f:
            f.write('test file C')
        
        logging.info('Chmod created files to mode 777')
        os.chmod(self.fileA, 0777)
        os.chmod(self.fileB, 0777)
        os.chmod(self.fileC, 0777)
        logging.info('Setup complete')
        
    def tearDown(self):
        logging.info('Tear down test')
        logging.info('Delete file /guc_tmp/a/a.txt')
        self.h_file.delete_file('/guc_tmp/a/a.txt')
        logging.info('Delete file /guc_tmp/b/b.txt')
        self.h_file.delete_file('/guc_tmp/b/b.txt')
        logging.info('Delete file /guc_tmp/c/c.txt')
        self.h_file.delete_file('/guc_tmp/c/c.txt')
        
        logging.info('Delete directory /guc_tmp/a')
        hpssclapi.Rmdir('/guc_tmp/a')
        logging.info('Delete directory /guc_tmp/b')
        hpssclapi.Rmdir('/guc_tmp/b')
        logging.info('Delete directory /guc_tmp/c')
        hpssclapi.Rmdir('/guc_tmp/c')
        logging.info('Delete directory /guc_tmp')
        hpssclapi.Rmdir('/guc_tmp')
        
        logging.info('Recursively delete ' + self.base_dir)
        shutil.rmtree(self.base_dir)
        logging.info('Tear down complete\n')
    
    def test_recursive_transfer_to_hpss(self):
        logging.info('Running test: test_recursive_transfer_to_hpss')
        self.guc.recursive_transfer_files(self.source, self.destination)
        logging.info('Verify ' + self.fileA + ' transfered to hpss')
        self.verify_transfer_to_hpss(self.fileA, '/guc_tmp/a/a.txt')
        logging.info('Verify ' + self.fileB + ' transfered to hpss')
        self.verify_transfer_to_hpss(self.fileB, '/guc_tmp/b/b.txt')
        logging.info('Verify ' + self.fileC + ' transfered to hpss')
        self.verify_transfer_to_hpss(self.fileC, '/guc_tmp/c/c.txt')
        logging.info('Transfers verified')
        logging.info('Test test_recursive_transfer_to_hpss passed') 

# Uncomment out below if you wish to skip this test

#@unittest.skip('Class large_recursive_transfer_to_hpss will take multiple' +
#    ' hours skipping test')
class large_recursive_transfer_to_hpss(guc_system_tests):

    def setUp(self):
        logging.info('Setup test') 
        self.base_dir = '/tmp/guc_tmp/'
        self.source = 'file://' + self.base_dir
        self.destination = 'gsiftp://' + socket.gethostname() +'/guc_tmp/' 

        self.dir_class.create_local_directory(self.base_dir)
        self.dir_class.create_hpss_directory('/guc_tmp', 16895)
               
        self.dir_class.create_local_directory(self.base_dir + 'a')
        self.dir_class.create_local_directory(self.base_dir + 'b')
        self.dir_class.create_local_directory(self.base_dir + 'c')
        
        logging.info('Chmod created local directories to 777')
        os.chmod(self.base_dir + 'a', 0777)
        os.chmod(self.base_dir + 'b', 0777)
        os.chmod(self.base_dir + 'c', 0777)
        
        self.files_in_dir = 35000
        logging.info('Create ' +  str(self.files_in_dir) + ' files in ' +
                self.base_dir + 'a, ' + self.base_dir + 'b and ' +
                self.base_dir + 'c')
        multiprocess_func.create_local_files(self.base_dir,
                self.files_in_dir)

    def tearDown(self):
        logging.info('Tear down test')
        logging.info('Recursivly delete ' + self.base_dir)
        shutil.rmtree(self.base_dir)
        sub_dirs = ['a','b','c']
        # TODO: GT look into multi-processing/ threading to speed up test
        logging.info('Deleting all files transfered to hpss') 
        for x in range(0, 3):
            dir_fd = hpssclapi.Opendir('/guc_tmp/' + sub_dirs[x])
            for i in range(0, self.files_in_dir + 2):
                entry = hpssclapi.Readdir(dir_fd)
                if entry.d_name not in ['.', '..']:
                    hpssclapi.Unlink('/guc_tmp/' + sub_dirs[x] + '/' +
                            entry.d_name)
            hpssclapi.Closedir(dir_fd)
            logging.info('Delete hpss directory /guc_tmp/' +sub_dirs[x])
            hpssclapi.Rmdir('/guc_tmp/' + sub_dirs[x])
        logging.info('Delete hpss directory /guc_tmp')
        hpssclapi.Rmdir('/guc_tmp')
        logging.info('All files transfered to hpss deleted')
        logging.info('Tear down complete\n')
    
    def test_large_recursive_transfer_to_hpss(self):
        logging.info('Running test: test_large_recursive_transfer_to_hpss')
        self.guc.recursive_transfer_files(self.source, self.destination)
        logging.info('Verify that all files have transfered successfully')
        logging.info('This will take a a considerable amount of time, up to 48 hours')
        self.verify_recursive_transfer_to_hpss() 
        logging.info('Transfers verified')
        logging.info('Test test_large_recursive_transfer_to_hpss passed')

    def verify_recursive_transfer_to_hpss(self):
        sub_dirs = ['a','b','c']
        # TODO: GT look into multi-processing/ threading to speed up test
        for x in range(0, 3):
            dir_fd = hpssclapi.Opendir('/guc_tmp/' + sub_dirs[x])
            for i in range(0, self.files_in_dir + 2):
                entry = hpssclapi.Readdir(dir_fd)
                if entry.d_name not in ['.', '..']:
                    source = self.base_dir + sub_dirs[x] + '/' + entry.d_name
                    dest =  '/guc_tmp/' + sub_dirs[x] + '/' + entry.d_name
                    self.verify_transfer_to_hpss(source, dest)
            hpssclapi.Closedir(dir_fd)
class transfer_from_hpss(guc_system_tests):

    def setUp(self):
        logging.info('Setup test')
        self.cksm = checksum()
        
        self.base_dir = '/tmp/guc_tmp'
        
        old_mask = os.umask(0)
        self.dir_class.create_local_directory(self.base_dir, 0777) 
        os.umask(old_mask)
        
        hints = hpssclapi.cos_hints()
        priorities = hpssclapi.cos_priorities()
        self.hpss_path = '/guc_tmp_file'
        
        logging.info('Create hpss file ' + self.hpss_path)
        fd = hpssclapi.Creat(self.hpss_path, 0777, hints, priorities)[0]
        
        file_size = 10
        random_data = bytearray(os.urandom(file_size))
        logging.info('write ' + str(file_size) + ' bytes of data to ' +  self.hpss_path)
        hpssclapi.Write(fd, random_data, file_size)
        hpssclapi.Close(fd)

        self.source = 'gsiftp://' + socket.gethostname() + self.hpss_path 
        
        self.path = self.base_dir + '/guc_tmp_file'
        self.destination = 'file://' + self.path

        logging.info('Setup complete')
    
    def tearDown(self): 
        logging.info('Tear down test')
        self.h_file.delete_file(self.hpss_path) 
        self.l_file.delete_file(self.path)
        logging.info('Delete local directory ' + self.base_dir)
        shutil.rmtree(self.base_dir)
        logging.info('Tear down complete\n')
    
    def test_transfer_from_hpss(self):
        logging.info('Running test: test_transfer_from_hpss')
        self.guc.transfer_file(self.source, self.destination)
        logging.info('Verify transfer')
        self.verify_transfer_from_hpss(self.path, self.hpss_path)
        logging.info('Transfer verified')
        logging.info('Test test_transfer_from_hpss passed')
    
    def test_no_write_perms_transfer_from_hpss(self):
        logging.info('Running test: test_no_write_perms_transfer_from_hpss')
        logging.info('Chmod local dir ' + self.base_dir + ' to 555')
        os.chmod(self.base_dir, 0555)
        self.guc.transfer_file(self.source, self.destination)
        logging.info('Assert that local file ' + self.path + ' was not created')
        self.assertFalse(os.path.exists(self.path))
        logging.info('Local file ' + self.path + ' was not created')
        logging.info('Test test_no_write_perms_transfer_from_hpss passed')

class recursive_transfer_from_hpss(guc_system_tests):

    def setUp(self):
        logging.info('Setup test')
        self.base_dir = '/tmp/guc_tmp/'
        self.source = 'gsiftp://' + socket.gethostname() +'/guc_tmp/' 
        self.destination = 'file://' + self.base_dir
        
        self.dir_class.create_local_directory(self.base_dir)
        os.chmod(self.base_dir, 0777)

        self.dir_class.create_hpss_directory('/guc_tmp/', 16895)
        self.dir_class.create_hpss_directory('/guc_tmp/a', 16895)
        self.dir_class.create_hpss_directory('/guc_tmp/b', 16895)
        self.dir_class.create_hpss_directory('/guc_tmp/c', 16895)
    
        self.files_in_dir = 1

        self.create_random_hpss_file('/guc_tmp/a/a.txt', 10)
        self.create_random_hpss_file('/guc_tmp/b/b.txt', 10)
        self.create_random_hpss_file('/guc_tmp/c/c.txt', 10)

        logging.info('Setup complete')

    def tearDown(self):
        logging.info('Tear down test')
        logging.info('Recursively delete local directory ' + self.base_dir)
        shutil.rmtree(self.base_dir)
        sub_dirs = ['a','b','c']
        
        logging.info('Delete hpss files')
        for x in range(0, self.files_in_dir +2):
            dir_fd = hpssclapi.Opendir('/guc_tmp/' + sub_dirs[x])
            for i in range(0, 3):
                entry = hpssclapi.Readdir(dir_fd)
                if entry.d_name not in ['.', '..']:
                    hpssclapi.Unlink('/guc_tmp/' + sub_dirs[x] + '/' +
                            entry.d_name)
            hpssclapi.Closedir(dir_fd)
            logging.info('Delete hpss directory ' + sub_dirs[x])
            hpssclapi.Rmdir('/guc_tmp/' + sub_dirs[x])
        logging.info('Delete hpss directory /guc_tmp')
        hpssclapi.Rmdir('/guc_tmp')
        logging.info('Tear down complete\n')
    
    def create_random_hpss_file(self, path, size):
        logging.info('Create hpss file ' + path + ' of size ' + str(size))
        hints = hpssclapi.cos_hints()
        priorities = hpssclapi.cos_priorities()
        fd = hpssclapi.Creat(path, 0777, hints, priorities)[0]
        random_data = bytearray(os.urandom(size))
        hpssclapi.Write(fd, random_data, size)
        hpssclapi.Close(fd)   
    # TODO: remove comment once bug fixed
    # This test will fail due to a bug that I am currently working on  
    def test_recursive_transfer_from_hpss(self):
        logging.info('Running test: test_recursive_transfer_from_hpss')
        self.guc.recursive_transfer_files(self.source, self.destination)
        logging.info('Verify recursive transfer')
        self.verify_recursive_transfer_from_hpss()
        logging.info('Recursive transfer verified')
        logging.info('Test test_recursive_transfer_from_hpss passed')

    def verify_recursive_transfer_from_hpss(self):
        sub_dirs = ['a', 'b', 'c']
        for x in range(0, self.files_in_dir + 2):
            dir_fd = hpssclapi.Opendir('/guc_tmp/' + sub_dirs[x])
            for i in range(0, 3):
                entry = hpssclapi.Readdir(dir_fd)
                if entry.d_name not in ['.', '..']:
                    source = self.base_dir + sub_dirs[x] + '/' + entry.d_name
                    dest =  '/guc_tmp/' + sub_dirs[x] + '/' + entry.d_name
                    self.verify_transfer_from_hpss(source, dest)
            hpssclapi.Closedir(dir_fd)

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
    
    print 'Testing started results logged in ' + log_file
    with open(log_file, 'a') as f:
        main(f)
    print 'Testing complete!'
    print 'Results logged in ' + log_file
