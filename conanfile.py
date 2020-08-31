from conans import ConanFile, CMake, tools
import os


class QtProtobufConan(ConanFile):
    name = "qtprotobuf"
    version = "0.5.0"
    license = "MIT"
    url = "https://github.com/semlanik/qtprotobuf"
    description = ("gRPC and Protobuf generator and bindings for Qt framework")
    topics = ("conan", "qt", "protobuf")
    settings = "os", "compiler", "build_type", "arch"
    homepage = "https://github.com/semlanik/qtprotobuf"
    options = {"shared": [True, False]}
    default_options = {
        "shared": False,
        "qt:qtdeclarative": True,
    }
    generators = "cmake"
    requires = [
        "protobuf/3.9.1",
        "protoc_installer/3.9.1@bincrafters/stable",
        "qt/5.14.2@bincrafters/stable",
        "microjson/0.1.0@semlanik/stable",
    ]
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
    }
    exports_sources = "*"

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["QT_PROTOBUF_MAKE_EXAMPLES"] = "OFF"
        cmake.definitions["QT_PROTOBUF_MAKE_TESTS"] = "OFF"
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        cmake.install()

    def package(self):
        cmake = self._configure_cmake()
        cmake.configure()
        cmake.install()

        self.copy("LICENSE", dst="licenses")

