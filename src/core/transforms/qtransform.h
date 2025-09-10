// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QTRANSFORM_H
#define QT3DCORE_QTRANSFORM_H

#include <Qt3DCore/qcomponent.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QTransformPrivate;
class Q_3DCORESHARED_EXPORT QTransform : public QComponent
{
    Q_OBJECT
    Q_PROPERTY(QMatrix4x4 matrix READ matrix WRITE setMatrix NOTIFY matrixChanged)
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector3D scale3D READ scale3D WRITE setScale3D NOTIFY scale3DChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation NOTIFY translationChanged)
    Q_PROPERTY(float rotationX READ rotationX WRITE setRotationX NOTIFY rotationXChanged)
    Q_PROPERTY(float rotationY READ rotationY WRITE setRotationY NOTIFY rotationYChanged)
    Q_PROPERTY(float rotationZ READ rotationZ WRITE setRotationZ NOTIFY rotationZChanged)
    Q_PROPERTY(QMatrix4x4 worldMatrix READ worldMatrix NOTIFY worldMatrixChanged QT3D_PROPERTY_REVISION(2, 14))

public:
    explicit QTransform(QNode *parent = nullptr);
    ~QTransform();

    float scale() const;
    QVector3D scale3D() const;
    QQuaternion rotation() const;
    QVector3D translation() const;

    Q_INVOKABLE static QQuaternion fromAxisAndAngle(const QVector3D &axis, float angle);
    Q_INVOKABLE static QQuaternion fromAxisAndAngle(float x, float y, float z, float angle);

    Q_INVOKABLE static QQuaternion fromAxesAndAngles(const QVector3D &axis1, float angle1,
                                                     const QVector3D &axis2, float angle2);
    Q_INVOKABLE static QQuaternion fromAxesAndAngles(const QVector3D &axis1, float angle1,
                                                     const QVector3D &axis2, float angle2,
                                                     const QVector3D &axis3, float angle3);
    Q_INVOKABLE static QQuaternion fromAxes(const QVector3D &xAxis, const QVector3D &yAxis, const QVector3D &zAxis);

    Q_INVOKABLE static QQuaternion fromEulerAngles(const QVector3D &eulerAngles);
    Q_INVOKABLE static QQuaternion fromEulerAngles(float pitch, float yaw, float roll);

    Q_INVOKABLE static QMatrix4x4 rotateAround(const QVector3D &point, float angle, const QVector3D &axis);
    Q_INVOKABLE static QMatrix4x4 rotateFromAxes(const QVector3D &xAxis, const QVector3D &yAxis, const QVector3D &zAxis);

    QMatrix4x4 matrix() const;
    QMatrix4x4 worldMatrix() const;

    float rotationX() const;
    float rotationY() const;
    float rotationZ() const;

public Q_SLOTS:
    void setScale(float scale);
    void setScale3D(const QVector3D &scale);
    void setRotation(const QQuaternion &rotation);
    void setTranslation(const QVector3D &translation);
    void setMatrix(const QMatrix4x4 &matrix);

    void setRotationX(float rotationX);
    void setRotationY(float rotationY);
    void setRotationZ(float rotationZ);

Q_SIGNALS:
    void scaleChanged(float scale);
    void scale3DChanged(const QVector3D &scale);
    void rotationChanged(const QQuaternion &rotation);
    void translationChanged(const QVector3D &translation);
    void matrixChanged();
    void rotationXChanged(float rotationX);
    void rotationYChanged(float rotationY);
    void rotationZChanged(float rotationZ);
    void worldMatrixChanged(const QMatrix4x4 &worldMatrix);

protected:
    explicit QTransform(QTransformPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QTransform)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QTRANSFORM_H
