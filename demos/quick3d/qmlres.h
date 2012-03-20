/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLRES_H
#define QMLRES_H

#include <QtCore/qdir.h>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>

#include <QtCore/qdebug.h>

#define internal_xstr(s) internal_str(s)
#define internal_str(s) #s

/*!
    \internal
    Returns a string with the path to qml resources, including qml sources,
    3D assets and textures.  The path depends on the platform, and (for
    some platforms) whether it was installed from a package or is being run
    in a development setting.
*/
static QString q_get_qmldir(const QString &name)
{
    QString qml = name;
    // try for a Linux package install first
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#ifdef QT3D_USE_OPT
    QDir pkgdir(QLatin1String("/opt/mt/applications/" internal_xstr(QT3D_USE_OPT)));
#else
    QDir pkgdir(QLatin1String("/usr/share/qt5/quick3d/demos"));
#endif
#else
    QDir pkgdir(QLatin1String("/usr/share/qt4/quick3d/demos"));
#endif
    QString app = QCoreApplication::applicationFilePath();
    app = app.section(QDir::separator(), -1);
    if (pkgdir.cd(app) && pkgdir.exists())
    {
        qml = pkgdir.filePath(qml);
    }
    else
    {
        // failing that try Mac (pkg & dev) next
        QDir dir(QCoreApplication::applicationDirPath());
        if (dir.path().endsWith(QLatin1String("MacOS")))
        {
            if (dir.cdUp() && dir.cd(QLatin1String("Resources"))
                    && dir.exists())
            {
                qml = dir.filePath(qml);
            }
            else
            {
                qWarning("Expected app bundle with QML resources!");
            }
        }
        else
        {
            // for Windows (pkg & dev), and for Linux dev expect to find it
            // in a "resources" directory next to the binary
            if (dir.cd(QLatin1String("resources")) && dir.exists())
            {
                QString app = QCoreApplication::applicationFilePath();
                app = QDir::toNativeSeparators(app);
                //For windows platforms the "app" filepath should have the .exe extension removed.
                const QString winExtension = ".exe";
                if (app.right(winExtension.length()) == winExtension) {
                    app = app.left(app.length() - winExtension.length());
                }

                //Grab just the app name itself.
                app = app.section(QDir::separator(), -1);

                if (dir.cd(QLatin1String("demos")) && dir.cd(app) && dir.exists())
                {
                    qml = dir.filePath(qml);
                }
                else
                {
                    QString msg = QLatin1String("demos");
                    msg += QDir::separator();
                    msg += app;
                    qWarning("Expected %s directry with qml resources!", qPrintable(msg));
                }
            }
        }
    }
    return qml;
}

#define QUICK3D_MAIN(file)                                                      \
int main(int argc, char *argv[])                                                        \
{                                                                                       \
    QGuiApplication app(argc, argv);                                                    \
    QSurfaceFormat f;                                                                   \
    f.setSamples(16);                                                                   \
    QQuickView view;                                                                    \
    view.setFormat(f);                                                                  \
    QString qml = q_get_qmldir(QLatin1String(file));                                    \
    view.setSource(QUrl::fromLocalFile(qml));                                           \
    if (QGuiApplication::arguments().contains(QLatin1String("-maximize")))              \
        view.showMaximized();                                                           \
    else if (QGuiApplication::arguments().contains(QLatin1String("-fullscreen")))       \
        view.showFullScreen();                                                          \
    else                                                                                \
        view.show();                                                                    \
    return app.exec();                                                                  \
}                                                                                       \


#endif // QMLRES_H
