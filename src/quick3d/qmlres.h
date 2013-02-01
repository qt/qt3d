/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLRES_H
#define QMLRES_H

//
//  W A R N I N G
//  -------------
//
// THIS FILE CONTAINS AN UNSUPPORTED INTERNAL API FOR AD-HOC CROSS-PLATFORM
// AND CROSS-VERSION QML FILE DEPLOYMENT SPECIFIC TO QT3D EXAMPLES, DEMOS &
// TUTORIALS ONLY.
//
// ONCE QML APP DEPLOYMENT FOR QT3D IS STABLE & DEFINED THIS FILE WILL BE
// REMOVED.  DO NOT USE IT IN YOUR APPLICATIONS.

#include <Qt3DQuick/qt3dquickglobal.h>

QT_BEGIN_NAMESPACE

QString Q_QT3D_QUICK_EXPORT q_get_qmldir(const QString &name, const QString &category);

QT_END_NAMESPACE

int Q_QT3D_QUICK_EXPORT q_quick3d_main(const char *entryPointQmlFile, const char *category, int argc, char *argv[]);

#endif // QMLRES_H
