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

#ifndef QT3DEXTRAS_QSPRITESHEET_H
#define QT3DEXTRAS_QSPRITESHEET_H

#include <Qt3DExtras/qabstractspritesheet.h>
#include <Qt3DExtras/qspritesheetitem.h>
#include <QtCore/QList>
#include <QtCore/QRect>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QSpriteSheetPrivate;

class Q_3DEXTRASSHARED_EXPORT QSpriteSheet : public QAbstractSpriteSheet
{
    Q_OBJECT
    Q_PROPERTY(QList<QSpriteSheetItem *> sprites READ sprites WRITE setSprites NOTIFY spritesChanged)
public:
    explicit QSpriteSheet(Qt3DCore::QNode *parent = nullptr);
    ~QSpriteSheet();

    QList<QSpriteSheetItem *> sprites() const;
    QSpriteSheetItem *addSprite(int x, int y, int width, int height);
    void addSprite(QSpriteSheetItem *sprite);
    void removeSprite(QSpriteSheetItem *sprite);

public Q_SLOTS:
    void setSprites(QList<QSpriteSheetItem *> sprites);

Q_SIGNALS:
    void spritesChanged(QList<QSpriteSheetItem *> sprites);

private:
    Q_DECLARE_PRIVATE(QSpriteSheet)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPRITESHEET_H
