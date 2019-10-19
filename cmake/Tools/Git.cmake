find_dependency(Git QUIET REQUIRED)

function(detect_git_version)
    execute_process(
      COMMAND git log -1 --format=%h
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_COMMIT_HASH_PRIVATE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(GIT_COMMIT_HASH "${GIT_COMMIT_HASH_PRIVATE}" PARENT_SCOPE)
endfunction()

function(detect_git_branch)
    execute_process(
      COMMAND git rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_COMMIT_BRANCH_PRIVATE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(GIT_COMMIT_BRANCH "${GIT_COMMIT_BRANCH_PRIVATE}" PARENT_SCOPE)
endfunction()

function(clone_git_submodule PATH)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init ${PATH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT)

    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init ${PATH} failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endfunction()
