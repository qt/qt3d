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
    "quick" => "#include <QtQuick/QQuick>\n",
    "network" => "#include <QtNetwork/QtNetwork>\n"
);
%modulepris = (
    "Qt3D" => "$basedir/modules/qt_qt3d.pri",
    "Qt3DQuick" => "$basedir/modules/qt_qt3dquick.pri",
);
# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
    "qtbase" => "refs/heads/master",
    "qtsvg" => "refs/heads/master",
    "qtxmlpatterns" => "refs/heads/master",
    "qtdeclarative" => "refs/heads/master",
    "qtjsbackend" => "refs/heads/master",
);
