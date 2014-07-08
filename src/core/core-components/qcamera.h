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

#ifndef QT3D_CAMERA_H
#define QT3D_CAMERA_H

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qt3dcore_global.h>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QCameraPrivate;
class QCameraLens;
class QTransform;

class QT3DCORESHARED_EXPORT QCamera : public QEntity
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QCameraLens *lens READ lens WRITE setLens NOTIFY lensChanged)
    Q_PROPERTY(Qt3D::QTransform *transform READ transform WRITE setTransform NOTIFY transformChanged)

public:
    explicit QCamera(QNode *parent = 0);


    enum CameraTranslationOption {
        TranslateViewCenter,
        DontTranslateViewCenter
    };

    QCameraLens* lens() const;
    void setLens(QCameraLens *lens);

    QTransform *transform() const;
    void setTransform(QTransform* lookAt);

    QQuaternion tiltRotation(float angle) const;
    QQuaternion panRotation(float angle) const;
    QQuaternion rollRotation(float angle) const;

public slots:

    // Translate relative to camera orientation axes
    void translate( const QVector3D& vLocal, CameraTranslationOption option = TranslateViewCenter );

    // Translate relative to world axes
    void translateWorld( const QVector3D& vWorld, CameraTranslationOption option = TranslateViewCenter );

    void tilt( const float& angle );
    void pan( const float& angle );
    void roll( const float& angle );

    void tiltAboutViewCenter( const float& angle );
    void panAboutViewCenter( const float& angle );
    void rollAboutViewCenter( const float& angle );

    void rotate( const QQuaternion& q );
    void rotateAboutViewCenter( const QQuaternion& q );

Q_SIGNALS:
    void lensChanged();
    void transformChanged();

protected:
    Q_DECLARE_PRIVATE(QCamera)

private:
    QCameraPrivate* d_ptr;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_CAMERA_H
