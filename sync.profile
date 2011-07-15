%modules = ( # path to module name map
    "Qt3D" => "$basedir/src/threed",
    "Qt3DQuick" => "$basedir/src/quick3d",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
%classnames = (
    "qtquick3dversion.h" => "QtQuick3DVersion",
);
%mastercontent = (
    "core" => "#include <QtCore/QtCore>\n",
    "gui" => "#include <QtGui/QtGui>\n",
    "opengl" => "#include <QtOpenGL/QtOpenGL>\n",
    "declarative" => "#include <QtDeclarative/QDeclarative>\n",
    "network" => "#include <QtNetwork/QtNetwork>\n"
);
%modulepris = (
    "Qt3D" => "$basedir/modules/qt_qt3d.pri",
    "Qt3DQuick" => "$basedir/modules/qt_qt3dquick.pri",
);
# Modules and programs, and their dependencies.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - "LATEST_REVISION", to always test against the latest revision.
#   - "LATEST_RELEASE", to always test against the latest public release.
#   - "THIS_REPOSITORY", to indicate that the module is in this repository.
%dependencies = (
    "Qt3D" => {
        "QtCore" => "0c637cb07ba3c9b353e7e483a209537485cc4e2a",
        "QtDeclarative" => "0c637cb07ba3c9b353e7e483a209537485cc4e2a",
    },
);
