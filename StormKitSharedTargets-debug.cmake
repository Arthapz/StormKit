set(CMAKE_IMPORT_FILE_VERSION 1)

set(CORE_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-core-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(CORE_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-core-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Core APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Core PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${CORE_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${CORE_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Core)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Core ${CORE_LIB_NAME} ${CORE_BIN_NAME})

set(ENGINE_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-engine-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(ENGINE_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-engine-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Engine APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Engine PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${ENGINE_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${ENGINE_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Engine)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Engine ${ENGINE_LIB_NAME} ${ENGINE_BIN_NAME})

set(ENTITIES_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-entities-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(ENTITIES_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-entities-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Entities APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Entities PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${ENGINE_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${ENGINE_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Entities)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Entities ${ENTITIES_LIB_NAME} ${ENTITIES_BIN_NAME})

set(IMAGE_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-image-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(IMAGE_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-image-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Image APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Image PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${IMAGE_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${IMAGE_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Image)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Image ${IMAGE_LIB_NAME} ${IMAGE_BIN_NAME})

set(LOG_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-log-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(LOG_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-log-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Log APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Log PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${LOG_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${LOG_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Log)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Log ${LOG_LIB_NAME} ${LOG_BIN_NAME})

set(MAIN_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}StormKit-main-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG ${MAIN_LIB_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Main)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Main ${MAIN_LIB_NAME})

set(MODULE_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-module-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(MODULE_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-module-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Module APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Module PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${MODULE_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${MODULE_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Module)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Module ${MODULE_LIB_NAME} ${MODULE_BIN_NAME})

set(RENDER_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-render-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(RENDER_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-render-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Render APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Render PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${RENDER_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${RENDER_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Render)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Render ${RENDER_LIB_NAME} ${RENDER_BIN_NAME})

set(WINDOW_LIB_NAME "${_IMPORT_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-window-debug.${CMAKE_LINK_LIBRARY_SUFFIX}")
set(WINDOW_BIN_NAME "${_IMPORT_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}StormKit-window-debug.${CMAKE_SHARED_LIBRARY_SUFFIX}")

set_property(TARGET StormKit::Window APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(StormKit::Window PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${WINDOW_LIB_NAME}
  IMPORTED_LOCATION_DEBUG ${WINDOW_BIN_NAME}
)

list(APPEND _IMPORT_CHECK_TARGETS StormKit::Window)
list(APPEND _IMPORT_CHECK_FILES_FOR_StormKit::Window ${WINDOW_LIB_NAME} ${WINDOW_BIN_NAME})

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
