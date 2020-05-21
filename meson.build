project(
    'StormKit',
    ['c', 'cpp'],
    version: '0.1.0',
    license: 'MIT',
    default_options : ['c_std=c17', 'default_library=static']
)

system_name = host_machine.system()

is_windows = system_name == 'windows'
is_linux = system_name == 'linux'
is_macos = system_name == 'darwin'
is_ios = system_name == 'ios'
is_android = system_name == 'android'

examples_data_dir = get_option('examples_data_dir')

lib_type = get_option('library_type')
project_arguments = []
examples_arguments = []

lib_extension   = ''

if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
    project_arguments += ['/DSTORM_BUILD', '/std:c++latest', '/permissive-', '/DFMT_HEADER_ONLY', '/DNOMINMAX']
    examples_arguments += ['/std:c++latest', '/permissive-', '/NOMINMAX', '/DEXAMPLES_DATA_DIR="' + examples_data_dir + '"']
    lib_extension = '.lib'

    message(meson.get_compiler('cpp').get_id())
    if get_option('buildtype') == 'debug'
        project_arguments += ['/DSTORM_BUILD_DEBUG']
    endif
    if lib_type == 'static'
        project_arguments += ['/DSTORM_STATIC /Od']
    endif

    if meson.get_compiler('cpp').get_id() == 'clang-cl'
        project_arguments += ['-fcolor-diagnostics', '-fdiagnostics-color', '-Wno-missing-braces', '-Wno-unused-command-line-argument', '-fansi-escape-codes']
    endif
else
    project_arguments += ['-DSTORM_BUILD', '-std=c++2a', '-Wno-missing-braces', '-Wno-unknown-pragmas', '-DFMT_HEADER_ONLY']
    examples_arguments += ['-std=c++2a', '-Wno-missing-braces', '-Wno-unknown-pragmas', '-DEXAMPLES_DATA_DIR="' + examples_data_dir + '"']
    if get_option('buildtype') == 'debug'
        project_arguments += ['-DSTORM_BUILD_DEBUG -O0']
    endif

    if is_windows
        lib_extension = '.a'
        examples_arguments += ['-DNOMINMAX']
    elif is_macos
        lib_extension = '.dylib'
    else
        lib_extension = '.so'
    endif

    if lib_type == 'static'
        project_arguments += ['-DSTORM_STATIC']
        lib_extension = '.a'
    endif
endif
add_project_arguments(project_arguments, language: ['c', 'cpp', 'objc', 'objcpp'])

enable_entities   = get_option('enable_entities')
enable_log        = get_option('enable_log')
enable_module     = get_option('enable_module')
enable_window     = get_option('enable_window')
enable_image      = get_option('enable_image')
enable_render     = get_option('enable_render')
enable_engine     = get_option('enable_engine')

major_version   = 0
minor_version   = 1
patch_version   = 0
milestone       = 'alpha'
build_type      = get_option('buildtype')
git_commit_hash = 'unknown'
git_branch      = 'unknown'
suffix          = ''

if build_type == 'debug' or build_type == 'debugoptimized'
    suffix += '-debug'
endif

git_command = find_program('git', required: false)
if not git_command.found()
    message('Failed to extract git commit hash: \ngit not found')
else
    git_commit_hash_run = run_command('git', ['log', '-1', '--pretty=%h'])
    git_branch_run = run_command('git', ['rev-parse', '--abbrev-ref', 'HEAD'])

    if git_commit_hash_run.returncode() == 0
        git_commit_hash = git_commit_hash_run.stdout().strip()
    else
        message('Failed to extract git commit hash: \n' + git_run.stderr())
    endif

    if git_branch_run.returncode() == 0
        git_branch = git_branch_run.stdout().strip()
    else
        message('Failed to extract git branch: \n' + git_run.stderr())
    endif
endif

if enable_render
    #assert(enable_image and enable_window and enable_log)
endif 

if enable_engine
    #assert(enable_render)
endif 

subdir('core/')
stormkit_deps = [core_dep]

if enable_entities
    subdir('entities/')
    stormkit_deps += [entities_dep]
endif

if enable_log
    subdir('log/')
    stormkit_deps += [log_dep]
endif

if enable_module
    subdir('module/')
    stormkit_deps += [module_dep]
endif

if enable_window
    subdir('main/')
    subdir('window/')
    stormkit_deps += [main_dep, window_dep]
endif

if enable_image
    subdir('image/')
    stormkit_deps += [image_dep]
endif

if enable_render
    subdir('render/')
    stormkit_deps += [render_dep]
endif

if enable_engine
    subdir('engine/')
    stormkit_deps += [engine_dep]
endif

stormkit_dep = declare_dependency(
    dependencies: stormkit_deps
)

cmake = import('cmake')
cmake.write_basic_package_version_file(
    name: 'StormKit',
    version: meson.project_version(),
    compatibility: 'SameMajorVersion'
)

conf = configuration_data()
conf.set('x', major_version.to_string())
conf.set('y', minor_version.to_string())
conf.set('z', patch_version.to_string())

cmake.configure_package_config_file(
    name: 'StormKit',
    input: 'StormKitConfig.cmake.in',
    configuration: conf
)
