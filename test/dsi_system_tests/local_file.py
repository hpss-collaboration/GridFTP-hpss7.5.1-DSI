import os
from tempfile import mkstemp

class local_file(object):

    def create_temp_file(self, file_size=0, directory=None):
        fd, path = mkstemp(dir=directory)
        os.close(fd)
        with open(path, 'wb') as fout:
            fout.write(os.urandom(file_size))
        return path

    def delete_file(self, path):
        if os.path.isfile(path):
            os.remove(path)
 
