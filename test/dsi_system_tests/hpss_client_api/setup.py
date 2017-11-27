from distutils.core import setup, Extension

if __name__ == '__main__':
    setup(name='hpss-systemtest-api-clapi',
          version='1.0',
          ext_modules=[Extension(
              'hpssclapi',
              ['attr.c',
               'config.c',
               'dir.c',
               'dirent.c',
               'env.c',
               'file.c',
               'hash.c',
               'hints.c',
               'hpssclapipy.c',
               'link.c',
               'mps.c',
               'net.c',
               'nsobjhandle.c',
               'pio.c',
               'priorities.c',
               'uda.c',
               'sec.c',
               'stream.c',
               'string.c',
               'uuid.c'
               ],
              runtime_library_dirs=['/opt/hpss/lib'],
          ),
          ]
          )
