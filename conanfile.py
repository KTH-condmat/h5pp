from conan import ConanFile
from conan.tools.layout import basic_layout
from conan.tools.files import copy
from conan.tools.microsoft import is_msvc
from conan.tools.build import check_min_cppstd
from conan.tools.scm import Version
from conan.errors import ConanInvalidConfiguration
import os

required_conan_version = ">=1.45.0"

class H5ppConan(ConanFile):
    name = "h5pp"
    version = "1.11.3"
    description = "A C++17 wrapper for HDF5 with focus on simplicity"
    homepage = "https://github.com/DavidAce/h5pp"
    author = "DavidAce <aceituno@kth.se>"
    topics = ("hdf5", "binary", "storage", "header-only", "cpp17")
    url = "https://github.com/DavidAce/h5pp"
    license = "MIT"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    no_copy_source = True
    short_paths = True
    exports_sources = "include/*"
    options = {
        "with_eigen": [True, False],
        "with_spdlog": [True, False],
        "with_zlib" : [True, False],
        "with_quadmath": [True, False],
        "with_float128": [True, False]
    }
    default_options = {
        "with_eigen": True,
        "with_spdlog": True,
        "with_zlib" : True,
        "with_quadmath": False,
        "with_float128": False,
    }

    @property
    def _compilers_minimum_version(self):
        return {
            "gcc": "7.4",
            "Visual Studio": "15.7",
            "clang": "6",
            "apple-clang": "10",
        }

    def config_options(self):
        self.options["hdf5"].with_zlib = self.options.with_zlib

    def requirements(self):
        self.requires("hdf5/[>=1.10.0 <1.15]", transitive_headers=True, transitive_libs=True)
        if self.options.get_safe('with_eigen'):
            self.requires("eigen/[>=3.3.7 <=3.4.0]", transitive_headers=True)
        if self.options.get_safe('with_spdlog'):
            self.requires("spdlog/[>=1.6.0 <1.16]", transitive_headers=True, transitive_libs=True)
        if self.options.get_safe('with_zlib'):
            self.requires("zlib/[>=1.2.11 <2]", transitive_headers=True, transitive_libs=True)
    def layout(self):
        basic_layout(self)

    def package_id(self):
        self.info.clear()

    def validate(self):
        if self.settings.compiler != "gcc":
            self.options.rm_safe('with_quadmath')
        if self.settings.compiler.get_safe("cppstd"):
            if self.options.get_safe('with_float128'):
                check_min_cppstd(self, 23)
            else:
                check_min_cppstd(self, 17)

        minimum_version = self._compilers_minimum_version.get(str(self.settings.compiler), False)
        if minimum_version:
            if Version(self.settings.compiler.version) < minimum_version:
                raise ConanInvalidConfiguration("h5pp requires C++17, which your compiler does not support.")
        else:
            self.output.warning("h5pp requires C++17. Your compiler is unknown. Assuming it supports C++17.")

        if self.options.get_safe('with_float128') and  self.options.get_safe('with_quadmath'):
            raise ConanInvalidConfiguration("These are mutually exclusive options: h5pp:with_float128 and h5pp:with_quadmath")



    def package(self):
        includedir = os.path.join(self.source_folder, "include")
        copy(self, pattern="*", src=includedir, dst=os.path.join(self.package_folder, "include"))
        copy(self, pattern="LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))


    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "h5pp")
        self.cpp_info.set_property("cmake_target_name", "h5pp::h5pp")
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.components["h5pp_headers"].set_property("cmake_target_name", "h5pp::headers")
        self.cpp_info.components["h5pp_headers"].bindirs = []
        self.cpp_info.components["h5pp_headers"].libdirs = []
        self.cpp_info.components["h5pp_deps"].set_property("cmake_target_name", "h5pp::deps")
        self.cpp_info.components["h5pp_deps"].bindirs = []
        self.cpp_info.components["h5pp_deps"].libdirs = []
        self.cpp_info.components["h5pp_deps"].requires = ["hdf5::hdf5"]
        self.cpp_info.components["h5pp_flags"].set_property("cmake_target_name", "h5pp::flags")
        self.cpp_info.components["h5pp_flags"].bindirs = []
        self.cpp_info.components["h5pp_flags"].libdirs = []

        if self.options.get_safe('with_eigen'):
            self.cpp_info.components["h5pp_deps"].requires.append("eigen::eigen")
            self.cpp_info.components["h5pp_flags"].defines.append("H5PP_USE_EIGEN3")
        if self.options.get_safe('with_spdlog'):
            self.cpp_info.components["h5pp_deps"].requires.append("spdlog::spdlog")
            self.cpp_info.components["h5pp_flags"].defines.append("H5PP_USE_SPDLOG")
            self.cpp_info.components["h5pp_flags"].defines.append("H5PP_USE_FMT")
        if self.options.get_safe('with_zlib'):
            self.cpp_info.components["h5pp_deps"].requires.append("zlib::zlib")
        if self.options.get_safe('with_float128'):
            self.cpp_info.components["h5pp_flags"].defines.append("H5PP_USE_FLOAT128")
        elif self.options.get_safe('with_quadmath'):
            self.cpp_info.components["h5pp_flags"].defines.append("H5PP_USE_QUADMATH")
            self.cpp_info.system_libs.append('quadmath')

