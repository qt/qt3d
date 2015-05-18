TEMPLATE = app

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml

RESOURCES += \
    multiviewport.qrc \
    ../exampleresources/test_scene.qrc
