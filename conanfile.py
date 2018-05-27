from conans import ConanFile
from conans.tools import os_info

class OpenCvExamples(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'cmake'

    def configure(self):
        self.options['OpenCV'].shared = True
        self.options['OpenCV'].opencl = True
        if os_info.is_windows:
            self.options['OpenCV'].gui = "WIN"

    def requirements(self):
        self.requires('OpenCV/3.4.1@piponazo/testing')

    def imports(self):
        #self.copy('*.dll', dst='bin', keep_path=False)
        self.copy('*.dll', src='x64/vc%s/bin/' % self.settings.compiler.version, dst='bin')
