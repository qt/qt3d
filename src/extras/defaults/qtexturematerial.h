// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTEXTUREMATERIAL_H
#define QT3DEXTRAS_QTEXTUREMATERIAL_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qmaterial.h>
#include <QtGui/QVector2D>
#include <QtGui/QMatrix3x3>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractTexture;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QTextureMaterialPrivate;

class Q_3DEXTRASSHARED_EXPORT QTextureMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAbstractTexture *texture READ texture WRITE setTexture NOTIFY textureChanged)
    Q_PROPERTY(QVector2D textureOffset READ textureOffset WRITE setTextureOffset NOTIFY textureOffsetChanged)
    Q_PROPERTY(QMatrix3x3 textureTransform READ textureTransform WRITE setTextureTransform NOTIFY textureTransformChanged REVISION 10)
    Q_PROPERTY(bool alphaBlending READ isAlphaBlendingEnabled WRITE setAlphaBlendingEnabled NOTIFY alphaBlendingEnabledChanged REVISION 11)

public:
    explicit QTextureMaterial(Qt3DCore::QNode *parent = nullptr);
    ~QTextureMaterial();

    Qt3DRender::QAbstractTexture *texture() const;
    QVector2D textureOffset() const;
    QMatrix3x3 textureTransform() const;
    bool isAlphaBlendingEnabled() const;

public Q_SLOTS:
    void setTexture(Qt3DRender::QAbstractTexture *texture);
    void setTextureOffset(QVector2D textureOffset);
    void setTextureTransform(const QMatrix3x3 &matrix);
    void setAlphaBlendingEnabled(bool enabled);

Q_SIGNALS:
    void textureChanged(Qt3DRender::QAbstractTexture *texture);
    void textureOffsetChanged(QVector2D textureOffset);
    void textureTransformChanged(const QMatrix3x3 &textureTransform);
    void alphaBlendingEnabledChanged(bool enabled);

private:
    Q_DECLARE_PRIVATE(QTextureMaterial)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTEXTUREMATERIAL_H
