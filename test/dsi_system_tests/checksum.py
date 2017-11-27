import hpssclapi
from subprocess import Popen, PIPE
import re


import test_config
from hpss_file import hpss_file

class checksum(object):

    def get_hpss_file_size(self, file_path):
        h_file = hpss_file()
        hpss_stat = h_file.fstat(file_path)
        return hpss_stat.st_size

    def calc_local_checksum(self, file_path, hasher, blocksize=65536):
        with open(file_path,'rb') as fout:
            buf = fout.read(blocksize)
            while len(buf) > 0:
                hasher.update(buf)
                buf = fout.read(blocksize)
        return hasher.hexdigest()
    
    def calc_hpss_checksum(self, file_path, hasher):
        fd = hpssclapi.Open(file_path, hpssclapi.O_RDONLY)[0]
        file_size = self.get_hpss_file_size(file_path)
        buf = hpssclapi.Read(fd, file_size)
        hasher.update(buf)
        hpssclapi.Close(fd)
        return hasher.hexdigest()

    def get_stored_uda_checksum_info(self, hpss_file_path):
        output = self.call_hpsssum_list(hpss_file_path)
        if output:
            return self.parse_hpsssum_list_output(output)

    def call_hpsssum_list(self, hpss_file_path):
        cmd_list = [test_config.hpss_bin + '/hpsssum', '-f', hpss_file_path,
                '-U', 'list'] 
        process = Popen(cmd_list, stdin=PIPE, stdout=PIPE, stderr=PIPE)
        output, err = process.communicate()

        if err != '':
            if err == 'hpss_UserAttrGetAttrs: No such file or directory (-2)\n':
                h_file = hpss_file()
                if self.h_file.file_exists(hpss_file_path):
                    return None
                else:
                    raise IOError('File not found') 
            else:
                raise IOError(err)
        return output

    def parse_hpsssum_list_output(self, output):
        output = output.split('\n')
        checksum_info = {}
        
        app = output[1].replace('/hpss/user/cksum/app', '')
        checksum_info['app'] = app.strip()
        
        state = output[2].replace('/hpss/user/cksum/state','')
        checksum_info['state'] = state.strip()

        algorithm = output[3].replace('/hpss/user/cksum/algorithm','')
        checksum_info['algorithm'] = algorithm.strip()

        checksum = output[4].replace('/hpss/user/cksum/checksum','')
        checksum_info['checksum'] = checksum.strip()

        file_size = output[5].replace('/hpss/user/cksum/filesize','')
        checksum_info['file_size'] = file_size.strip()

        last_update = output[6].replace('/hpss/user/cksum/lastupdate','')
        checksum_info['last_update'] = last_update.strip()

        errors = output[7].replace('/hpss/user/cksum/errors', '')
        checksum_info['errors'] = errors.strip()
       
        return checksum_info 

    def get_stored_checksum_info(self, hpss_file_path):
        output = self.call_hashcheck(hpss_file_path)
        return self.parse_hashcheck_output(output)

    def call_hashcheck(self, hpss_file_path):
        cmd_list = [test_config.hpss_bin + '/hashcheck', '-i',
                test_config.subsys_id, hpss_file_path]
        process = Popen(cmd_list, stdin=PIPE, stdout=PIPE, stderr=PIPE)
        output, err = process.communicate()
        return output
    
    def parse_hashcheck_output(self, output):
        m = re.search('No hashing information found for.*', output)
        if m:
            return

        output = output.split('\n')
        checksum_info = {}
        
        m = re.search("'(\S*)'", output[0])
        checksum_info['bitfile_id'] = m.group(1)

        fileset_name = output[1].replace('Fileset Name', '')
        checksum_info['fileset_name'] = fileset_name.strip()

        file_name = output[2].replace('File Name', '')
        checksum_info['file_name'] = file_name.strip()

        hash_record = output[3].replace('Hash Record:', '')
        checksum_info['hash_record'] = hash_record.strip()

        algorithm = output[4].replace('Algorithm', '')
        checksum_info['algorithm'] = algorithm.strip()

        flags = output[5].replace('Flags', '')
        checksum_info['flags'] = flags.strip()

        creator = output[6].replace('Creator', '')
        checksum_info['creator'] = creator.strip()

        timestamp = output[7].replace('Timestamp', '')
        checksum_info['timestamp'] = timestamp.strip()
        
        m = re.search("'(\S*)'", output[8])
        checksum_info['digest'] = m.group(1)

        return checksum_info
