/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCollision/qabstractcollider.h>
#include <Qt3DCollision/qboxcollider.h>
#include <Qt3DCollision/qcapsulecollider.h>
#include <Qt3DCollision/qgeometrycollider.h>
#include <Qt3DCollision/qspherecollider.h>
#include "qt3dquick3dcollisionplugin.h"

QT_BEGIN_NAMESPACE

void Qt3DQuick3DCollisionPlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<Qt3D::QAbstractCollider>(uri, 2, 0, "AbstractCollider", "AbstractColliders are only used for references");
    qmlRegisterType<Qt3D::QBoxCollider>(uri, 2, 0, "BoxCollider");
    qmlRegisterType<Qt3D::QCapsuleCollider>(uri, 2, 0, "CapsuleCollider");
    qmlRegisterType<Qt3D::QGeometryCollider>(uri, 2, 0, "GeometryCollider");
    qmlRegisterType<Qt3D::QSphereCollider>(uri, 2, 0, "SphereCollider");
}

QT_END_NAMESPACE
