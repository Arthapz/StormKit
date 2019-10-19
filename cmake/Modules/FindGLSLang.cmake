set(_GLSLang_Include_Path "")
set(_GLSLang_Library_Path "")

set(_GLSLang_Components_Found "")

function(_GLSLang_Add_Component COMPONENT)
	list(APPEND _GLSLang_Components_Found ${COMPONENT})
endfunction()

if(WIN32)
	set(_GLSLang_Include_Path "$ENV{VULKAN_SDK}/Include")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(_GLSLang_Library_Path "$ENV{VULKAN_SDK}/Lib" "$ENV{VULKAN_SDK}/Bin")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(_GLSLang_Library_Path "$ENV{VULKAN_SDK}/Lib32" "$ENV{VULKAN_SDK}/Bin32")
  endif()
else()
	set(_GLSLang_Include_Path "$ENV{VULKAN_SDK}/include")
	set(_GLSLang_Library_Path "$ENV{VULKAN_SDK}/lib")
endif()

function(_GLSLang_find_component COMPONENT HEADERS LIBRARIES EXTRADEPS)
	find_path(GLSLang_${COMPONENT}_INCLUDE_DIR
		NAMES "${HEADERS}"
		PATHS "${_GLSLang_Include_Path}"
	)
	find_library(GLSLang_${COMPONENT}_LIBRARY
		NAMES "${LIBRARIES}"
		PATHS "${_GLSLang_Library_Path}"
	)
	if(TARGET GLSLang::${COMPONENT})
		_GLSLang_Add_Component(${COMPONENT})
		return()
	endif()
	set(EXTRALIBS "")
	foreach(DEP ${EXTRADEPS})
		if (NOT TARGET ${DEP})
			find_library(GLSLang_${DEP}_LIBRARY
				NAMES "${DEP}"
				PATHS "${_GLSLang_Library_Path}"
			)
		else()
			set(GLSLang_${DEP}_LIBRARY ${DEP})
		endif()
		if(NOT GLSLang_${DEP}_LIBRARY AND GLSLang_FIND_REQUIRED)
			message(SEND_ERROR "Can't find library ${DEP} required by ${COMPONENT} in GLSLang. Check that it's installed correctly and try again.")
			return()
		endif()
		list(APPEND EXTRALIBS ${GLSLang_${DEP}_LIBRARY})
	endforeach()
	if(GLSLang_${COMPONENT}_INCLUDE_DIR AND GLSLang_${COMPONENT}_LIBRARY)
		_GLSLang_Add_Component(${COMPONENT})
		add_library(GLSLang::${COMPONENT} UNKNOWN IMPORTED)
		set_target_properties(GLSLang::${COMPONENT} PROPERTIES
			IMPORTED_LOCATION "${GLSLang_${COMPONENT}_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${GLSLang_${COMPONENT}_INCLUDE_DIR}"
			IMPORTED_LINK_INTERFACE_LIBRARIES "${EXTRALIBS}"
		)
	else()
		if(GLSLang_FIND_REQUIRED)
			message(SEND_ERROR "Can't find ${COMPONENT} in GLSLang. Check that it's installed correctly and try again.")
		endif()
	endif()
endfunction()

function(_GLSLang_Find_Component_Wrapper COMPONENT)
	if(COMPONENT STREQUAL "GLSLang")
		set(GLSLang_QUIET "")
		if (GLSLang_FIND_QUIETLY)
			set(GLSLang_QUIET QUIET)
		endif()
		find_package(Threads ${GLSLang_QUIET})
		_GLSLang_find_component(${COMPONENT} "glslang/Public/ShaderLang.h" "glslang" "HLSL;OGLCompiler;OSDependent;Threads::Threads")
	elseif(COMPONENT STREQUAL "SPIRV")
		_GLSLang_Find_Component_Wrapper(GLSLang)
		_GLSLang_find_component(${COMPONENT} "SPIRV/GlslangToSpv.h" "SPIRV" "GLSLang::GLSLang;SPVRemapper")
	else()
		message(SEND_ERROR "Unknown component ${COMPONENT} in GLSLang. Only GLSLang and SPIRV are available.")
	endif()
endfunction()

foreach(COMPONENT ${GLSLang_FIND_COMPONENTS})
	_GLSLang_Find_Component_Wrapper(${COMPONENT})
endforeach()
