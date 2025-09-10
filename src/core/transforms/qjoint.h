// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QJOINT_H
#define QT3DCORE_QJOINT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>

#include <QtGui/qmatrix4x4.h>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QJointPrivate;

class Q_3DCORESHARED_EXPORT QJoint : public QNode
{
    Q_OBJECT
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation NOTIFY translationChanged)
    Q_PROPERTY(QMatrix4x4 inverseBindMatrix READ inverseBindMatrix WRITE setInverseBindMatrix NOTIFY inverseBindMatrixChanged)
    Q_PROPERTY(float rotationX READ rotationX WRITE setRotationX NOTIFY rotationXChanged)
    Q_PROPERTY(float rotationY READ rotationY WRITE setRotationY NOTIFY rotationYChanged)
    Q_PROPERTY(float rotationZ READ rotationZ WRITE setRotationZ NOTIFY rotationZChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit QJoint(Qt3DCore::QNode *parent = nullptr);
    ~QJoint();

    QVector3D scale() const;
    QQuaternion rotation() const;
    QVector3D translation() const;
    QMatrix4x4 inverseBindMatrix() const;
    float rotationX() const;
    float rotationY() const;
    float rotationZ() const;
    QString name() const;

    void addChildJoint(QJoint *joint);
    void removeChildJoint(QJoint *joint);
    QList<QJoint *> childJoints() const;

public Q_SLOTS:
    void setScale(const QVector3D &scale);
    void setRotation(const QQuaternion &rotation);
    void setTranslation(const QVector3D &translation);
    void setInverseBindMatrix(const QMatrix4x4 &inverseBindMatrix);
    void setRotationX(float rotationX);
    void setRotationY(float rotationY);
    void setRotationZ(float rotationZ);
    void setName(const QString &name);
    void setToIdentity();

Q_SIGNALS:
    void scaleChanged(const QVector3D &scale);
    void rotationChanged(const QQuaternion &rotation);
    void translationChanged(const QVector3D &translation);
    void inverseBindMatrixChanged(const QMatrix4x4 &inverseBindMatrix);
    void rotationXChanged(float rotationX);
    void rotationYChanged(float rotationY);
    void rotationZChanged(float rotationZ);
    void nameChanged(const QString &name);

private:
    Q_DECLARE_PRIVATE(QJoint)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QJOINT_H
