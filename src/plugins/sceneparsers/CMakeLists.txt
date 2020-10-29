# Generated from sceneparsers.pro.

qt_feature_module_begin(
    NO_MODULE
    PUBLIC_FILE "qtsceneparsers-config.h"
    PRIVATE_FILE "qtsceneparsers-config_p.h"
)
include(configure.cmake)
qt_feature_module_end(NO_MODULE)

if (TARGET Qt::3DExtras)
    add_subdirectory(gltf)
endif()
if((GCC AND QT_COMPILER_VERSION_MAJOR STRGREATER 4) OR (QT_FEATURE_qt3d_assimp AND NOT IOS AND NOT TVOS AND NOT qcc AND (CLANG OR QT_FEATURE_qt3d_system_assimp OR android-clang OR win32-msvc)))
    add_subdirectory(assimp)
endif()
if(QT_FEATURE_regularexpression AND QT_FEATURE_temporaryfile AND TARGET Qt::3DExtras)
    add_subdirectory(gltfexport)
endif()
