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

#ifndef PLANEENTITY_H
#define PLANEENTITY_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QPlaneMesh>

class PlaneEntity : public Qt3D::QEntity
{
public:
    PlaneEntity(Qt3D::QNode *parent = 0);
    ~PlaneEntity();

    Qt3D::QScaleTransform *scaleTransform() const;
    Qt3D::QTranslateTransform *translateTransform() const;
    Qt3D::QPlaneMesh *mesh() const;

private:
    Qt3D::QPlaneMesh *m_mesh;
    Qt3D::QTransform *m_transform;
    Qt3D::QScaleTransform *m_scaleTransform;
    Qt3D::QTranslateTransform *m_translateTransform;
};

#endif // PLANEENTITY_H
