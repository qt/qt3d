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

#ifndef TREFOILKNOT_H
#define TREFOILKNOT_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMesh>

class TrefoilKnot : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(float theta READ theta WRITE setTheta NOTIFY thetaChanged)
    Q_PROPERTY(float phi READ phi WRITE setPhi NOTIFY phiChanged)
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)

public:
    explicit TrefoilKnot(Qt3DCore::QNode *parent = 0);
    ~TrefoilKnot();

    float theta() const;
    float phi() const;
    QVector3D position() const;
    float scale() const;

public slots:
    void setTheta(float theta);
    void setPhi(float phi);
    void setPosition(QVector3D position);
    void setScale(float scale);

signals:
    void thetaChanged(float theta);
    void phiChanged(float phi);
    void positionChanged(QVector3D position);
    void scaleChanged(float scale);

protected:
    void updateTransform();

private:
    Qt3DRender::QMesh *m_mesh;
    Qt3DCore::QTransform *m_transform;
    float m_theta;
    float m_phi;
    QVector3D m_position;
    float m_scale;
};

#endif // TREFOILKNOT_H
