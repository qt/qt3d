// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSPRITESHEETITEM_H
#define QT3DEXTRAS_QSPRITESHEETITEM_H

#include <Qt3DExtras/qabstractspritesheet.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QSpriteSheetItemPrivate;

class Q_3DEXTRASSHARED_EXPORT QSpriteSheetItem : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
public:
    explicit QSpriteSheetItem(Qt3DCore::QNode *parent = nullptr);

    int x() const;
    int y() const;
    int width() const;
    int height() const;

public Q_SLOTS:
    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);

Q_SIGNALS:
    void xChanged(int x);
    void yChanged(int y);
    void widthChanged(int width);
    void heightChanged(int height);

private:
    Q_DECLARE_PRIVATE(QSpriteSheetItem)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPRITESHEETITEM_H
