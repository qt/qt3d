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

#ifndef TREFOILKNOT_H
#define TREFOILKNOT_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QMesh>

class TrefoilKnot : public Qt3D::QEntity
{
public:
    explicit TrefoilKnot(Qt3D::QNode *parent = 0);
    ~TrefoilKnot();

    Qt3D::QScaleTransform *scaleTransform() const;
    Qt3D::QRotateTransform *xaxisRotateTransform() const;
    Qt3D::QRotateTransform *yaxisRotateTransform() const;
    Qt3D::QTranslateTransform *translateTransform() const;

private:
    Qt3D::QMesh *m_mesh;
    Qt3D::QTransform *m_transform;
    Qt3D::QScaleTransform *m_scaleTransform;
    Qt3D::QRotateTransform *m_xaxisRotation;
    Qt3D::QRotateTransform *m_yaxisRotation;
    Qt3D::QTranslateTransform *m_translateTransform;
};

#endif // TREFOILKNOT_H
