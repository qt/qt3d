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
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qscaletransform.h>
#include <Qt3DCore/qlookattransform.h>
#include <Qt3DCore/qmatrixtransform.h>
#include <Qt3DCore/qtranslatetransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qabstracteffect.h>
#include <Qt3DCore/qabstractmaterial.h>
#include <Qt3DCore/qabstracttechnique.h>
#include <Qt3DCore/qabstractmesh.h>
#include <Qt3DCore/qabstractrenderpass.h>
#include <Qt3DCore/qabstractshader.h>
#include <Qt3DQuick/quick3dentity.h>
#include <Qt3DQuick/quick3dtransform.h>
#include <Qt3DQuick/quick3dconfiguration.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DCorePlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<Qt3D::QComponent>(uri, 2, 0, "Component", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractMesh>(uri, 2, 0, "QAbstractMesh", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractMaterial>(uri, 2, 0, "QAbstractMaterial", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractTechnique>(uri, 2, 0, "QAbstractTechnique", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractRenderPass>(uri, 2, 0, "QAbstractRenderPass", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractEffect>(uri, 2, 0, "QAbstractEffect", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractShader>(uri, 2, 0, "QAbstractShader", QStringLiteral(""));
    qmlRegisterUncreatableType<Qt3D::QAbstractTransform>(uri, 2, 0, "QAbstractTransform", QStringLiteral("QAbstractTransform is abstract"));

    qmlRegisterType<Qt3D::Quick::Quick3DConfiguration>(uri, 2, 0, "Configuration");
    qmlRegisterExtendedType<Qt3D::QEntity, Qt3D::Quick::Quick3DEntity>(uri, 2, 0, "Entity");
    qmlRegisterExtendedType<Qt3D::QTransform, Qt3D::Quick::Quick3DTransform>(uri, 2, 0, "Transform");
    // Ideally we want to make Node an uncreatable type
    // We would need qmlRegisterUncreatableExtendedType for that
    qmlRegisterExtendedUncreatableType<Qt3D::QNode, Qt3D::Quick::Quick3DNode>(uri, 2, 0, "Node", QStringLiteral("Node is a base class"));
    qmlRegisterExtendedType<Qt3D::QCamera, Qt3D::Quick::Quick3DNode>(uri, 2, 0, "Camera");
    qmlRegisterType<Qt3D::QCameraLens>(uri, 2, 0, "CameraLens");

    qmlRegisterType<Qt3D::QMatrixTransform>(uri, 2, 0, "MatrixTransform");
    qmlRegisterType<Qt3D::QTranslateTransform>(uri, 2, 0, "Translate");
    qmlRegisterType<Qt3D::QRotateTransform>(uri, 2, 0, "Rotate");
    qmlRegisterType<Qt3D::QLookAtTransform>(uri, 2, 0, "LookAt");
    qmlRegisterType<Qt3D::QScaleTransform>(uri, 2, 0, "Scale");
}

QT_END_NAMESPACE
