// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QABSTRACTLIGHT_H
#define QT3DRENDER_QABSTRACTLIGHT_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qcomponent.h>

#include <QtGui/QVector3D>
#include <QtGui/QColor>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractLightPrivate;

class Q_3DRENDERSHARED_EXPORT QAbstractLight : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)

public:
    ~QAbstractLight();

    enum Type {
        PointLight = 0,
        DirectionalLight,
        SpotLight
    };
    Q_ENUM(Type) // LCOV_EXCL_LINE

    Type type() const;
    QColor color() const;
    float intensity() const;

public Q_SLOTS:
    void setColor(const QColor &color);
    void setIntensity(float intensity);

protected:
    explicit QAbstractLight(QAbstractLightPrivate &dd, Qt3DCore::QNode *parent = nullptr);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void intensityChanged(float intensity);

private:
    Q_DECLARE_PRIVATE(QAbstractLight)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTLIGHT_H
