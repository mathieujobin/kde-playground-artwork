import os
import sipconfig
# import pyqtconfig

try:
    import PyQt4.pyqtconfig as pyqt4
except:
    pyqt4 = None

if pyqt4 is not None:
    config = pyqt4.Configuration()
    qt_data_dir = config.qt_data_dir
else:
    sipconfig.error("Unable to find PyQt v4.")



# The name of the SIP build file generated by SIP and used by the build
# system.
build_file = "cokoon.sbf"


## try to get the KDE4 include directory, only used for kdelibs_export.h...
#kde4_include_dir = os.popen("kde4-config --prefix").read().replace('\n','') + "/include"

# Get the extra SIP flags needed by the imported qt module.  Note that
# this normally only includes those flags (-x and -t) that relate to SIP's
# versioning system.
qt_sip_flags = config.pyqt_sip_flags

# Run SIP to generate the code.  Note that we tell SIP where to find the qt
# module's specification files using the -I flag.
os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, "-I", config.pyqt_sip_dir, qt_sip_flags, "cokoon.sip"]))

# We are going to install the SIP specification file for this module and
# its configuration module.
installs = []

installs.append(["cokoon.sip", os.path.join(config.default_sip_dir, "cokoon")])

installs.append(["cokoonconfig.py", config.default_mod_dir])

# Create the Makefile.  The QtModuleMakefile class provided by the
# pyqtconfig module takes care of all the extra preprocessor, compiler and
# linker flags needed by the Qt library.
makefile = pyqt4.QtGuiModuleMakefile(
    configuration=config,
    build_file=build_file,
    installs=installs
)

# Add the library we are wrapping.  The name doesn't include any platform
# specific prefixes or extensions (e.g. the "lib" prefix on UNIX, or the
# ".dll" extension on Windows).
makefile.extra_libs = ["cokoon"]

# Generate the Makefile itself.
makefile.generate()

# Now we create the configuration module.  This is done by merging a Python
# dictionary (whose values are normally determined dynamically) with a
# (static) template.
content = {
    # Publish where the SIP specifications for this module will be
    # installed.
    "cokoon_sip_dir":    config.default_sip_dir,

    # Publish the set of SIP flags needed by this module.  As these are the
    # same flags needed by the qt module we could leave it out, but this
    # allows us to change the flags at a later date without breaking
    # scripts that import the configuration module.
    "cokoon_sip_flags":  qt_sip_flags
}

# This creates the helloconfig.py module from the helloconfig.py.in
# template and the dictionary.
sipconfig.create_config_module("cokoonconfig.py", "cokoonconfig.py.in", content)

