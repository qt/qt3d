/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
