TEMPLATE = lib
CONFIG += static

SOURCES += exampleresources.cpp

HEADERS += exampleresources.h

DEFINES += ASSETS=$$shadowed($$PWD)/

#
# Generate the RCC (binary resource) file for all of our assets.
# We can't use RESOURCES here, as the resources are not in a subdir,
# of each example
#
load(resources)
RCC_BINARY_SOURCES += example-assets.qrc
asset_builder.commands = $$QMAKE_RCC -binary ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
asset_builder.depend_command = $$QMAKE_RCC_DEP -list $$QMAKE_RESOURCE_FLAGS ${QMAKE_FILE_IN}
asset_builder.input = RCC_BINARY_SOURCES
asset_builder.output = ${QMAKE_FILE_IN_BASE}.qrb
asset_builder.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += asset_builder

OTHER_FILES += \
    example-assets.qrc \
    assets/gltf/duck/* \
    assets/gltf/rambler/* \
    assets/gltf/SuperMurdoch/* \
    assets/gltf/wine/* \
    assets/obj/* \
    assets/textures/pattern_09/* \
    assets/chest/* \
    assets/houseplants/* \
    assets/metalbarrel/* \
    assets/cubemaps/miramar/* \
    assets/cubemaps/night/*
