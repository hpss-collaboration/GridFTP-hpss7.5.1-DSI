# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/multiprocess_func.py $: $id:$ $Rev: 17 $
import multiprocessing 
from local_file import local_file
from functools import partial
import os
import hpssclapi

from guc_system_tests import guc_system_tests

l_file = local_file()

def create_local_file(local_dir, files_in_dir):
    path = l_file.create_temp_file(10, local_dir + '/a')
    os.chmod(path, 0777)
    path = l_file.create_temp_file(10, local_dir + '/b')
    os.chmod(path, 0777)
    path = l_file.create_temp_file(10, local_dir + '/c')
    os.chmod(path, 0777)


def create_local_files(directory, files_in_dir):
    pool = multiprocessing.Pool()
    func = partial(create_local_file, directory)
    pool.map(func, xrange(files_in_dir))
    pool.close()
    pool.join()


if __name__ == '__main__':
    create_local_files('/tmp/guc_tmp', 3)
