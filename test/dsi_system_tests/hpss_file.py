import hpssclapi

class hpss_file(object):

    def fstat(self, hpss_file_path):
        oflags = hpssclapi.O_RDONLY
        fd = hpssclapi.Open(hpss_file_path, oflags)[0]
        stat = hpssclapi.Fstat(fd)
        hpssclapi.Close(fd)
        return stat

    def file_exists(self, hpss_path):
        oflags = hpssclapi.O_RDONLY
        try: 
            fd = hpssclapi.Open(hpss_path, oflags)[0]
            hpssclapi.Close(fd)
        except IOError as e:
            if e.errno == 2: 
                return False
            else:
                raise 
        return True 

    def delete_file(self, path):
        if self.file_exists(path):
            hpssclapi.Unlink(path)
