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

#include "qt3dquick3dcoreplugin.h"
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qtransform.h>
#include <private/quick3dentity_p.h>
#include <private/quick3dentityloader_p.h>
#include <private/quick3dconfiguration_p.h>
#include <private/quick3dnodeinstantiator_p.h>
#include <private/qquaternionanimation_p.h>
#include <private/qt3dquick_global_p.h>
#include <QtCore/qvariantanimation.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DCorePlugin::registerTypes(const char *uri)
{
    Qt3DCore::Quick::Quick3D_initialize();

    qmlRegisterUncreatableType<Qt3DCore::QComponent>(uri, 2, 0, "Component3D", QStringLiteral(""));

    qmlRegisterType<Qt3DCore::Quick::Quick3DConfiguration>(uri, 2, 0, "Configuration");
    Qt3DCore::Quick::registerExtendedType<Qt3DCore::QEntity, Qt3DCore::Quick::Quick3DEntity>("QEntity", "Qt3D.Core/Entity", uri, 2, 0, "Entity");
    qmlRegisterType<Qt3DCore::Quick::Quick3DEntityLoader>(uri, 2, 0, "EntityLoader");
    qmlRegisterType<Qt3DCore::Quick::Quick3DNodeInstantiator>(uri, 2, 0, "NodeInstantiator");
    qmlRegisterType<Qt3DCore::QTransform>(uri, 2, 0, "Transform");

    qmlRegisterType<Qt3DCore::Quick::QQuaternionAnimation>(uri, 2, 0, "QuaternionAnimation");
    qRegisterAnimationInterpolator<QQuaternion>(Qt3DCore::Quick::q_quaternionInterpolator);

    // Ideally we want to make Node an uncreatable type
    // We would need qmlRegisterUncreatableExtendedType for that
    qmlRegisterExtendedUncreatableType<Qt3DCore::QNode, Qt3DCore::Quick::Quick3DNode>(uri, 2, 0, "Node", QStringLiteral("Node is a base class"));
    Qt3DCore::Quick::registerExtendedType<Qt3DCore::QCamera, Qt3DCore::Quick::Quick3DNode>("QCamera", "Qt3D.Core/Camera", uri, 2, 0, "Camera");
    qmlRegisterType<Qt3DCore::QCameraLens>(uri, 2, 0, "CameraLens");
}

QT_END_NAMESPACE
