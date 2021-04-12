from conans import ConanFile, CMake

class CFF(ConanFile):
    requires = "cpp-httplib/0.8.6"
    generators = "cmake"