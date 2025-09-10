// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QDIRECTIONALLIGHT_H
#define QT3DRENDER_QDIRECTIONALLIGHT_H

#include <Qt3DRender/qabstractlight.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDirectionalLightPrivate;

class Q_3DRENDERSHARED_EXPORT QDirectionalLight : public QAbstractLight
{
    Q_OBJECT
    Q_PROPERTY(QVector3D worldDirection READ worldDirection WRITE setWorldDirection NOTIFY worldDirectionChanged)

public:
    explicit QDirectionalLight(Qt3DCore::QNode *parent = nullptr);
    ~QDirectionalLight();

    QVector3D worldDirection() const;

public Q_SLOTS:
    void setWorldDirection(const QVector3D &worldDirection);

Q_SIGNALS:
    void worldDirectionChanged(const QVector3D &worldDirection);

protected:
    explicit QDirectionalLight(QDirectionalLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QDirectionalLight)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QDIRECTIONALLIGHT_H
