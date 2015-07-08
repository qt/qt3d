/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtQml>
#include <Qt3DInput/qkeyboardcontroller.h>
#include <Qt3DInput/qkeyboardinput.h>
#include <Qt3DInput/q3dkeyevent.h>
#include <Qt3DInput/qmousecontroller.h>
#include <Qt3DInput/qmouseinput.h>
#include <Qt3DInput/q3dmouseevent.h>
#include "qt3dquick3dinputplugin.h"

QT_BEGIN_NAMESPACE

void Qt3DQuick3DInputPlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<Qt3D::Q3DKeyEvent>(uri, 2, 0, "KeyEvent", QStringLiteral("Events cannot be created"));
    qmlRegisterType<Qt3D::QKeyboardController>(uri, 2, 0, "KeyboardController");
    qmlRegisterType<Qt3D::QKeyboardInput>(uri, 2, 0, "KeyboardInput");

    qmlRegisterUncreatableType<Qt3D::Q3DMouseEvent>(uri, 2, 0, "MouseEvent", QStringLiteral("Events cannot be created"));
    qmlRegisterUncreatableType<Qt3D::Q3DWheelEvent>(uri, 2, 0, "WheelEvent", QStringLiteral("Events cannot be created"));
    qmlRegisterType<Qt3D::QMouseInput>(uri, 2, 0, "MouseInput");
    qmlRegisterType<Qt3D::QMouseController>(uri, 2, 0, "MouseController");
}

QT_END_NAMESPACE
