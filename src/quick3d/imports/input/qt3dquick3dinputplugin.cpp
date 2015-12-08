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
#include <Qt3DInput/qkeyevent.h>
#include <Qt3DInput/qmousecontroller.h>
#include <Qt3DInput/qmouseinput.h>
#include <Qt3DInput/qmouseevent.h>

#include <Qt3DInput/qaxis.h>
#include <Qt3DInput/qaxisinput.h>
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DInput/qaction.h>
#include <Qt3DInput/qaxisactionhandler.h>
#include <Qt3DInput/qactioninput.h>
#include <Qt3DInput/qlogicaldevice.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DQuickInput/private/quick3daxis_p.h>
#include <Qt3DQuickInput/private/quick3daction_p.h>
#include <Qt3DQuickInput/private/quick3dlogicaldevice_p.h>
#include <Qt3DQuickInput/private/quick3dphysicaldevice_p.h>

#include "qt3dquick3dinputplugin.h"

QT_BEGIN_NAMESPACE

void Qt3DQuick3DInputPlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<Qt3DInput::QKeyEvent>(uri, 2, 0, "KeyEvent", QStringLiteral("Events cannot be created"));
    qmlRegisterType<Qt3DInput::QKeyboardController>(uri, 2, 0, "KeyboardController");
    qmlRegisterType<Qt3DInput::QKeyboardInput>(uri, 2, 0, "KeyboardInput");

    qmlRegisterUncreatableType<Qt3DInput::QMouseEvent>(uri, 2, 0, "MouseEvent", QStringLiteral("Events cannot be created"));
    qmlRegisterUncreatableType<Qt3DInput::QWheelEvent>(uri, 2, 0, "WheelEvent", QStringLiteral("Events cannot be created"));
    qmlRegisterType<Qt3DInput::QMouseInput>(uri, 2, 0, "MouseInput");
    qmlRegisterType<Qt3DInput::QMouseController>(uri, 2, 0, "MouseController");

    qmlRegisterExtendedType<Qt3DInput::QLogicalDevice, Qt3DInput::Input::Quick::Quick3DLogicalDevice>(uri, 2, 0, "LogicalDevice");
    qmlRegisterType<Qt3DInput::QAxisActionHandler>(uri, 2, 0, "AxisActionHandler");
    qmlRegisterType<Qt3DInput::QActionInput>(uri, 2, 0, "ActionInput");
    qmlRegisterType<Qt3DInput::QAxisInput>(uri, 2, 0, "AxisInput");
    qmlRegisterType<Qt3DInput::QAxisSetting>(uri, 2, 0, "AxisSetting");
    qmlRegisterExtendedType<Qt3DInput::QAxis, Qt3DInput::Input::Quick::Quick3DAxis>(uri, 2, 0, "Axis");
    qmlRegisterExtendedType<Qt3DInput::QAction, Qt3DInput::Input::Quick::Quick3DAction>(uri, 2, 0, "Action");
    qmlRegisterExtendedUncreatableType<Qt3DInput::QAbstractPhysicalDevice, Qt3DInput::Input::Quick::Quick3DPhysicalDevice>(uri, 2, 0, "QAbstractPhysicalDevice", QStringLiteral("QAbstractPhysicalDevice is abstract"));
}

QT_END_NAMESPACE
