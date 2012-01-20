/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLRES_H
#define QMLRES_H

#include <QtCore/qdir.h>

#include <QtGui/QGuiApplication>
#include <QtQuick/qquickview.h>

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
    QDir pkgdir(QLatin1String("/usr/share/qt5/quick3d/examples"));
#endif
#else
    QDir pkgdir(QLatin1String("/usr/share/qt4/quick3d/examples"));
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
                app = QDir::toNativeSeparators(app);
                //For windows platforms the "app" filepath should have the .exe extension removed.
                const QString winExtension = ".exe";
                if (app.right(winExtension.length()) == winExtension) {
                    app = app.left(app.length() - winExtension.length());
                }

                //Grab just the app name itself.
                app = app.section(QDir::separator(), -1);

                if (dir.cd(QLatin1String("examples")) && dir.cd(app) && dir.exists())
                {
                    qml = dir.filePath(qml);
                }
                else
                {
                    QString msg = QLatin1String("examples");
                    msg += QDir::separator();
                    msg += app;
                    qWarning("Expected %s directry with qml resources!", qPrintable(msg));
                }
            }
        }
    }
    return qml;
}

#define QUICK3D_EXAMPLE_MAIN(file)                                                      \
int main(int argc, char *argv[])                                                        \
{                                                                                       \
    QGuiApplication app(argc, argv);                                                    \
    QSurfaceFormat f;                                                                   \
    f.setSamples(16);                                                                   \
    QQuickView view;                                                                       \
    view.setFormat(f);                                                                  \
    QString qml = q_get_qmldir(QLatin1String("qml/desktop.qml"));                       \
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
