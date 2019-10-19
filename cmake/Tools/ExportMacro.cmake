include(CMakePackageConfigHelpers)

macro(export_macos_bundle TARGET RESOURCES INFO_STRING COPYRIGHT)
    foreach(RESOURCE_FILE ${RESOURCES})
        get_filename_component(RESOURCE_FILE "${RESOURCE_FILE}"
                               ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        file(RELATIVE_PATH RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${RESOURCE_FILE}")
        get_filename_component(RESOURCE_PATH "${RESOURCE_PATH}"
                           DIRECTORY)

        set_property(
            SOURCE
                ${RESOURCE_FILE}
            PROPERTY
                MACOSX_PACKAGE_LOCATION "Resources/${RESOURCE_PATH}"
        )
    endforeach()

        set_target_properties(
        ${TARGET}
        PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST  "${CMAKE_CURRENT_SOURCE_DIR}/macOS/info.plist"
            MACOSX_BUNDLE_BUNDLE_NAME "${INFO_STRING}"
            MACOSX_BUNDLE_INFO_STRING "${INFO_STRING}"
            MACOSX_BUNDLE_BUNDLE_VERSION ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
            MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT}"
            MACOSX_BUNDLE_GUI_IDENTIFIER org.arthapz.stormkit.${TARGET}
            MACOSX_BUNDLE_LONG_VERSION_STRING "${STORM_MILESTONE} ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}-${CMAKE_BUILD_TYPE}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}"
       )
endmacro()



macro(export_macos_framework FILES MODULE_NAME)
    foreach(HEADER_FILE ${FILES})
        get_filename_component(HEADER_FILE_FULL_PATH "${HEADER_FILE}"
                               ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        file(RELATIVE_PATH HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" "${HEADER_FILE_FULL_PATH}")
        get_filename_component(HEADER_PATH "${HEADER_PATH}"
                           DIRECTORY)

        set_property(
            SOURCE
                ${HEADER_FILE}
            PROPERTY
                MACOSX_PACKAGE_LOCATION "Headers/${HEADER_PATH}"
        )
    endforeach()

    set_target_properties(
        stormkit-${MODULE_NAME}
        PROPERTIES
            FRAMEWORK                             TRUE
            FRAMEWORK_VERSION                     A
            MACOSX_FRAMEWORK_IDENTIFIER           org.arthapz.stormkit.${MODULE_NAME}
            MACOSX_FRAMEWORK_BUNDLE_VERSION       ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
            MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}"
            VERSION                               ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
            SOVERSION                             ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
    )
endmacro()

function(append_prefix_and_suffix TARGET)
    set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX -d)

    if(${STORM_OUTPUT_TYPE} MATCHES SHARED)
        set_target_properties(${TARGET} PROPERTIES PREFIX "lib")
    elseif(${STORM_OUTPUT_TYPE} MATCHES MODULE)
            set_target_properties(${TARGET} PROPERTIES PREFIX "")
    elseif(${STORM_OUTPUT_TYPE} MATCHES STATIC)
        set_target_properties(${TARGET} PROPERTIES PREFIX "lib")
    endif()
endfunction()

macro(export_target TARGET NAME)
    set (extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args extra_args_count)

    set(FIND_DEPENDENCY_FILES ${${ARGV1}})

    string(TOLOWER ${NAME} NAME_LOWER)
    set_target_properties(${TARGET} PROPERTIES EXPORT_NAME ${NAME_LOWER})

    install(
        TARGETS ${TARGET}
        EXPORT ${TARGET}-targets
        RUNTIME       DESTINATION bin ${CMAKE_INSTALL_BINDIR}
        BUNDLE        DESTINATION bin ${CMAKE_INSTALL_BINDIR}
        FRAMEWORK     DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
        LIBRARY       DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE       DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION include ${CMAKE_INSTALL_INCLUDEDIR}
        PRIVATE_HEADER DESTINATION include ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        EXPORT ${TARGET}-targets
        FILE "StormKit${NAME}Targets.cmake"
        NAMESPACE StormKit::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/StormKit"
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/StormKit${NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/StormKit${NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/StormKit${NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StormKit
    )

    install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/StormKit${NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/StormKit${NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StormKit
    )

    foreach(FILE IN LISTS FIND_DEPENDENCY_FILES)
        install(FILES
            "${stormkit_SOURCE_DIR}/cmake/Modules/${FILE}"
             DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StormKit
        )

        configure_file(
            "${stormkit_SOURCE_DIR}/cmake/Modules/${FILE}"
            "${CMAKE_CURRENT_BINARY_DIR}/${FILE}"
            COPYONLY
        )
    endforeach()

    export(EXPORT ${TARGET}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/StormKit${NAME}Targets.cmake NAMESPACE StormKit::)
    export(PACKAGE StormKit)
endmacro()
