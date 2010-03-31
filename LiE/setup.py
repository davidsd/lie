from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

sourcefiles = ['pylie.pyx', 'bigint.c']
module1 = Extension('pylie',
                    include_dirs = ['.', 'box'],
                    sources = sourcefiles
                    )

setup(
    cmdclass = { 'build_ext': build_ext },
    ext_modules = [module1]
)

