// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QABSTRACTSPRITESHEET_H
#define QT3DEXTRAS_QABSTRACTSPRITESHEET_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/QNode>
#include <Qt3DRender/qabstracttexture.h>
#include <QtGui/QVector2D>
#include <QtGui/QMatrix3x3>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QAbstractSpriteSheetPrivate;

class Q_3DEXTRASSHARED_EXPORT QAbstractSpriteSheet : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAbstractTexture *texture READ texture WRITE setTexture NOTIFY textureChanged)
    Q_PROPERTY(QMatrix3x3 textureTransform READ textureTransform NOTIFY textureTransformChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    ~QAbstractSpriteSheet();

    Qt3DRender::QAbstractTexture *texture() const;
    QMatrix3x3 textureTransform() const;
    int currentIndex() const;

public Q_SLOTS:
    void setTexture(Qt3DRender::QAbstractTexture *texture);
    void setCurrentIndex(int currentIndex);

Q_SIGNALS:
    void textureChanged(Qt3DRender::QAbstractTexture *texture);
    void textureTransformChanged(const QMatrix3x3 &textureTransform);
    void currentIndexChanged(int currentIndex);

protected:
    explicit QAbstractSpriteSheet(QAbstractSpriteSheetPrivate &d, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QAbstractSpriteSheet)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QABSTRACTSPRITESHEET_H
