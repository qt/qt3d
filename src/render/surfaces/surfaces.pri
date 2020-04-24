INCLUDEPATH += $$PWD

qtConfig(vulkan) {
HEADERS += \
    $$PWD/vulkaninstance_p.h \

SOURCES += \
    $$PWD/vulkaninstance.cpp
}
