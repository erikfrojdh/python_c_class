

import setuptools
import numpy as np
c_ext = setuptools.Extension("creader",
                    sources = ["src/creader_module.c", "src/cluster_reader.c"],
                    include_dirs=[
                            np.get_include(),
                            ],
                    extra_compile_args=['-std=gnu99', '-Wall', '-Wextra'] )
                    

c_ext.language = 'c'
setuptools.setup(
    name= 'creader',
    version = '0.1',
    description = 'Reading files',
    ext_modules=[c_ext],
)
