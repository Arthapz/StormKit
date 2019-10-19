# - Try to find libjpeg-turbo
# Once done, this will define
#
#  TURBOJPEG_FOUND - system has libjpeg-turbo
#  TURBOJPEG_INCLUDE_DIRS - the libjpeg-turbo include directories
#  TURBOJPEG_LIBRARIES - link these to use libjpeg-turbo
#
# and the following imported targets
#
#     TURBOJPEG::TURBOJPEG
#
# Author: Arthur Laurent - <arthur.laurent4@gmail.com>

include (FindPackageHandleStandardArgs)

find_path(TURBOJPEG_INCLUDE_DIR 
            turbojpeg.h
          PATHS 
            ${TURBOJPEG_ROOT}/include
            /usr/include
            /usr/local/include
            /usr/local/opt/jpeg-turbo/include
            /opt/jpeg-turbo/include
)
				
set(JPEG_NAMES ${JPEG_NAMES} jpeg libjpeg turbojpeg libturbojpeg libjpeg.so)

find_library(TURBOJPEG_LIBRARY NAMES 
                  ${JPEG_NAMES} 
             PATHS 
                  ${TURBOJPEG_INCLUDE_DIR}/../lib
                  ${TURBOJPEG_ROOT}/lib64
                  ${TURBOJPEG_ROOT}/lib
                  /usr/lib
		 /usr/lib/x86_64-linux-gnu/
                  /usr/lib64
                  /usr/local/lib
                  /usr/local/lib64
                  /usr/local/opt/jpeg-turbo/lib
                  /opt/jpeg-turbo/lib
                  NO_DEFAULT_PATH
)

mark_as_advanced(TURBOJPEG_FOUND TURBOJPEG_INCLUDE_DIR TURBOJPEG_LIBRARY)

find_package_handle_standard_args(TURBOJPEG
      REQUIRED_VARS 
            TURBOJPEG_INCLUDE_DIR
            TURBOJPEG_LIBRARY
)

if(TURBOJPEG_FOUND)
      set(TURBOJPEG_LIBRARIES    ${TURBOJPEG_LIBRARY})
      set(TURBOJPEG_INCLUDE_DIRS ${TURBOJPEG_INCLUDE_DIR})
endif()

if(TURBOJPEG_FOUND AND NOT TARGET TURBOJPEG::TURBOJPEG)
	  add_library(TURBOJPEG::TURBOJPEG UNKNOWN IMPORTED)
      set_target_properties(TURBOJPEG::TURBOJPEG 
            PROPERTIES
				  INTERFACE_INCLUDE_DIRECTORIES "${TURBOJPEG_INCLUDE_DIRS}"
				  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
				  IMPORTED_LOCATION ${TURBOJPEG_LIBRARIES}
	)
endif()
