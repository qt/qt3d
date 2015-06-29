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

#ifndef SCENEMODIFIER_H
#define SCENEMODIFIER_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscaletransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/QTranslateTransform>

#include <Qt3DRenderer/QTorusMesh>
#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QCuboidMesh>
#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QPhongMaterial>

class SceneModifier : public QObject
{
    Q_OBJECT

public:
    explicit SceneModifier(Qt3D::QEntity *rootEntity);
    ~SceneModifier();

public slots:
    void enableTorus(bool enabled);
    void enableCylinder(bool enabled);
    void enableCuboid(bool enabled);
    void enableSphere(bool enabled);

private:
    Qt3D::QEntity *m_rootEntity;
    Qt3D::QTorusMesh *m_torus;
    Qt3D::QEntity *m_cylinderEntity;
    Qt3D::QEntity *m_torusEntity;
    Qt3D::QEntity *m_cuboidEntity;
    Qt3D::QEntity *m_sphereEntity;
};

#endif // SCENEMODIFIER_H

