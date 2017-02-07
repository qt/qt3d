INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/IDConfig.hpp \
    $$PWD/IDConfigEigen.hpp \
    $$PWD/IDMath.hpp \
    $$PWD/IDConfigBuiltin.hpp \
    $$PWD/IDErrorMessages.hpp \
    $$PWD/MultiBodyTree.hpp \
    $$PWD/details/IDEigenInterface.hpp \
    $$PWD/details/IDMatVec.hpp \
    $$PWD/details/IDLinearMathInterface.hpp \
    $$PWD/details/MultiBodyTreeImpl.hpp \
    $$PWD/details/MultiBodyTreeInitCache.hpp

SOURCES += \
    $$PWD/IDMath.cpp \
    $$PWD/MultiBodyTree.cpp \
    $$PWD/details/MultiBodyTreeInitCache.cpp \
    $$PWD/details/MultiBodyTreeImpl.cpp
