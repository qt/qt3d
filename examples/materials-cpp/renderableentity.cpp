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

#include "renderableentity.h"

RenderableEntity::RenderableEntity(Qt3D::QNode *parent)
    : Qt3D::QEntity(parent)
    , m_mesh(new Qt3D::QMesh())
    , m_transform(new Qt3D::QTransform())
    , m_rotateTransform(new Qt3D::QRotateTransform())
    , m_scaleTransform(new Qt3D::QScaleTransform())
    , m_translateTransform(new Qt3D::QTranslateTransform())
{
    m_transform->addTransform(m_rotateTransform);
    m_transform->addTransform(m_scaleTransform);
    m_transform->addTransform(m_translateTransform);
    addComponent(m_mesh);
    addComponent(m_transform);
}

RenderableEntity::~RenderableEntity()
{

}

Qt3D::QMesh *RenderableEntity::mesh() const
{
    return m_mesh;
}

Qt3D::QScaleTransform *RenderableEntity::scaleTransform() const
{
    return m_scaleTransform;
}

Qt3D::QTranslateTransform *RenderableEntity::translateTransform() const
{
    return m_translateTransform;
}

Qt3D::QRotateTransform *RenderableEntity::rotateTransform() const
{
    return m_rotateTransform;
}
