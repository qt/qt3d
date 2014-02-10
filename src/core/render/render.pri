include(backend/render-backend.pri)
include(frontend/render-frontend.pri)
include(io/render-io.pri)

RESOURCES += $$PWD/render.qrc

OTHER_FILES += \
    $$PWD/shaders/*
