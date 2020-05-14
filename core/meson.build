fmt_dep = dependency('fmt')
glm_dep = dependency('glm')
gsl_dep = dependency('Microsoft.GSL', method: 'cmake', modules: ['Microsoft.GSL::GSL'])
range_v3_dep = dependency('range-v3')

dependencies = [fmt_dep, glm_dep, gsl_dep, range_v3_dep]

#ide:editable-filelist
base_headers = files([
      'include/storm/core/Fwd.hpp',
      'include/storm/core/DryComparisons.hpp',
      'include/storm/core/DeferredStackAllocation.hpp',
      'include/storm/core/App.hpp',
      'include/storm/core/Assert.hpp',
      'include/storm/core/CRTP.hpp',
      'include/storm/core/Flags.hpp',
      'include/storm/core/Hash.hpp',
      'include/storm/core/Memory.hpp',
      'include/storm/core/NamedType.hpp',
      'include/storm/core/NonCopyable.hpp',
      'include/storm/core/NonDefaultInstanciable.hpp',
      'include/storm/core/NonMovable.hpp',
      'include/storm/core/Math.hpp',
      'include/storm/core/Numerics.hpp',
      'include/storm/core/Platform.hpp',
      'include/storm/core/Singleton.hpp',
      'include/storm/core/Strings.hpp',
      'include/storm/core/Pimpl.hpp',
      'include/storm/core/Timer.hpp',
      'include/storm/core/Types.hpp',
      'include/storm/core/TypeTraits.hpp',
      'include/storm/core/Reference.hpp',
      'include/storm/core/Observer.hpp',
      'include/storm/core/Ranges.hpp',
      'include/storm/core/ResourcesPool.hpp',
      'include/storm/core/State.hpp',
      'include/storm/core/StateManager.hpp',
      'include/storm/core/Subject.hpp',
      'include/storm/core/Tree.hpp',
      'include/storm/core/TreeNode.hpp',
      'include/storm/core/RingBuffer.hpp',
      'include/storm/core/Span.hpp',
      'include/storm/core/Color.hpp',
])

private_headers = files([
   'include/storm/core/private/Flags.hpp',
   'include/storm/core/private/incbin.h',
])

#ide:editable-filelist
inls = files([
    'include/storm/core/Numerics.inl',
    'include/storm/core/Pimpl.inl',
    'include/storm/core/Timer.inl',
    'include/storm/core/Reference.inl',
    'include/storm/core/Subject.inl',
    'include/storm/core/Tree.inl',
    'include/storm/core/TreeNode.inl',
    'include/storm/core/RingBuffer.inl',
])

#ide:editable-filelist
sources = files([
    'src/State.cpp',
    'src/StateManager.cpp',
    'src/TreeNode.cpp',
    'src/Math.cpp',
])

conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('major_version', major_version)
conf_data.set('minor_version', minor_version)
conf_data.set('patch_version', patch_version)
conf_data.set('milestone', milestone)
conf_data.set('build_type', build_type)
conf_data.set('git_commit_hash', git_commit_hash)
conf_data.set('git_branch', git_branch)

configure_file(input : 'include/storm/core/Configure.hpp.in',
             output : 'Configure.hpp',
             configuration : conf_data)

core_inc = include_directories(['include/', '.'])

base_headers += files(meson.current_build_dir() / 'Configure.hpp')
headers = base_headers + private_headers

if lib_type == 'shared'
    core_lib_name = 'StormKit-core' + suffix
    core_lib = shared_library(
        core_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: core_inc,
        install: true
    )
else
    core_lib_name = 'StormKit-core-static' + suffix
    core_lib = static_library(
        core_lib_name,
        sources,
        headers + private_headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: core_inc,
        install: true
    )
endif

core_dep = declare_dependency(
    link_with: core_lib, 
    include_directories: core_inc,
    dependencies: dependencies
)

install_headers(base_headers + inls, subdir : 'storm/core')
install_headers(private_headers, subdir : 'storm/core/private')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Core')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'fmt::fmt Microsoft.GSL::GSL glm range-v3::range-v3')
conf.set_quoted('PACKAGE_DEPENDENCIES', 'fmt;Microsoft.GSL;glm;range-v3')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitCoreTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitCoreTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
