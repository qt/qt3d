/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtQml>
#include "qt3dquick3dcoreplugin.h"
#include <Qt3DQuick/quick3dentity.h>
#include <Qt3DQuick/quick3dtransform.h>
#include <Qt3DCore/cameralens.h>
#include <Qt3DCore/camera.h>
#include <Qt3DCore/scaletransform.h>
#include <Qt3DCore/lookattransform.h>
#include <Qt3DCore/matrixtransform.h>
#include <Qt3DCore/translatetransform.h>
#include <Qt3DCore/rotatetransform.h>
#include <Qt3DCore/qabstracteffect.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DCorePlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<Qt3D::Node>(uri, 2, 0, "Node_priv", "");
    qmlRegisterInterface<Qt3D::QAbstractEffect>("QAbstractEffect");
    qmlRegisterType<Qt3D::Component>(uri, 2, 0, "Component");
    qmlRegisterType<Qt3D::Camera>(uri, 2, 0, "Camera");
    qmlRegisterType<Qt3D::CameraLens>(uri, 2, 0, "CameraLens");
    qmlRegisterType<Qt3D::MatrixTransform>(uri, 2, 0, "MatrixTransform");
    qmlRegisterType<Qt3D::TranslateTransform>(uri, 2, 0, "Translate");
    qmlRegisterType<Qt3D::RotateTransform>(uri, 2, 0, "Rotate");
    qmlRegisterType<Qt3D::LookAtTransform>(uri, 2, 0, "LookAt");
    qmlRegisterType<Qt3D::ScaleTransform>(uri, 2, 0, "Scale");
    qmlRegisterType<Qt3D::Quick::Quick3DEntity>(uri, 2, 0, "Entity");
    qmlRegisterType<Qt3D::Quick::Quick3DTransform>(uri, 2, 0, "Transform");
    qmlRegisterType<Qt3D::Quick::Quick3DNode>(uri, 2, 0, "Node");
}

QT_END_NAMESPACE
