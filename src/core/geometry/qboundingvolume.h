/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QBOUNDINGVOLUME_H
#define QT3DCORE_QBOUNDINGVOLUME_H

#include <QtGui/qvector3d.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QBoundingVolumePrivate;

class Q_3DCORESHARED_EXPORT QBoundingVolume : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QGeometryView* view READ view WRITE setView NOTIFY viewChanged)
    Q_PROPERTY(QVector3D implicitMinPoint READ implicitMinPoint NOTIFY implicitMinPointChanged)
    Q_PROPERTY(QVector3D implicitMaxPoint READ implicitMaxPoint NOTIFY implicitMaxPointChanged)
    Q_PROPERTY(bool implicitPointsValid READ areImplicitPointsValid NOTIFY implicitPointsValidChanged)
    Q_PROPERTY(QVector3D minPoint READ minPoint WRITE setMinPoint NOTIFY minPointChanged)
    Q_PROPERTY(QVector3D maxPoint READ maxPoint WRITE setMaxPoint NOTIFY maxPointChanged)
public:
    explicit QBoundingVolume(Qt3DCore::QNode *parent = nullptr);
    ~QBoundingVolume();

    QGeometryView* view() const;
    QVector3D implicitMinPoint() const;
    QVector3D implicitMaxPoint() const;
    bool areImplicitPointsValid() const;
    QVector3D minPoint() const;
    QVector3D maxPoint() const;

public Q_SLOTS:
    void setView(QGeometryView *view);
    void setMinPoint(const QVector3D &minPoint);
    void setMaxPoint(const QVector3D &maxPoint);

    bool updateImplicitBounds();

Q_SIGNALS:
    void viewChanged(QGeometryView *view);
    void implicitMinPointChanged(const QVector3D &implicitMinPoint);
    void implicitMaxPointChanged(const QVector3D &implicitMaxPoint);
    void implicitPointsValidChanged(bool implicitPointsValid);
    void minPointChanged(QVector3D minPoint);
    void maxPointChanged(QVector3D maxPoint);

protected:
    QBoundingVolume(QBoundingVolumePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QBoundingVolume)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBOUNDINGVOLUME_H
