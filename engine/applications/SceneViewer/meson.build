
qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Qml', 'Quick', 'QuickControls2'])

dependencies = [core_dep, engine_dep, render_dep, qt6_dep]

if is_windows
    dependencies += meson.get_compiler('cpp').find_library('Dwmapi')
endif

directory = 'stormkit/SceneViewer/'

arguments = []
if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
    arguments += ['/DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"', '/DSTORMKIT_ENABLE_VALIDATION_LAYERS']
else
    arguments += ['-DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"', '-DSTORMKIT_ENABLE_VALIDATION_LAYERS']
endif
arguments += examples_arguments

moc_headers = files([
    'src/StormKitView.hpp',
    'src/StormKitTextureNode.hpp',
    'src/EntityModel.hpp',
    'src/StormKitScene.hpp',
    'src/ComponentModel.hpp',
    'src/ComponentReflector.hpp',
])

headers = files([
])

qml = files([
    'qml/main.qml',
    'qml/LeftPanel.qml',
    'qml/RightPanel.qml',
    'qml/MainPanel.qml',
    'qml/NameComponentEditor.qml',
    'qml/TransformComponentEditor.qml',
    'qml/MaterialComponentEditor.qml',
    'qml/MeshComponentEditor.qml',
])
qrc = files(['data.qrc'])

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/StormKitView.cpp',
    'src/StormKitTextureNode.cpp',
    'src/StormKitScene.cpp',
    'src/EntityModel.cpp',
    'src/ComponentModel.cpp',
    'src/ComponentReflector.cpp',
])

moc_sources = qt6.preprocess(moc_headers : moc_headers,
                             qresources: qrc,
                             dependencies: qt6_dep,
                             moc_extra_arguments: ['-DUNICODE', '-D_UNICODE', '-D_ENABLE_EXTENDED_ALIGNED_STORAGE'])
#translations = qt5.compile_translations(qresource: 'Translations.qrc', build_by_default : true)

executable(
    'stormkit_SceneViewer',
    sources, moc_sources,
    dependencies: dependencies,
    cpp_args: arguments,
    extra_files: moc_headers + qml + headers,
    install: true
)

install_subdir('textures/', install_dir: get_option('datadir') / directory)
