// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSPRITEGRID_H
#define QT3DEXTRAS_QSPRITEGRID_H

#include <Qt3DExtras/qabstractspritesheet.h>
#include <Qt3DCore/qcomponent.h>
#include <QtGui/QVector2D>
#include <QtGui/QMatrix3x3>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QSpriteGridPrivate;

class Q_3DEXTRASSHARED_EXPORT QSpriteGrid : public QAbstractSpriteSheet
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
public:
    explicit QSpriteGrid(Qt3DCore::QNode *parent = nullptr);
    ~QSpriteGrid();

    int rows() const;
    int columns() const;

public Q_SLOTS:
    void setRows(int rows);
    void setColumns(int columns);

Q_SIGNALS:
    void rowsChanged(int rows);
    void columnsChanged(int columns);

private:
    Q_DECLARE_PRIVATE(QSpriteGrid)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPRITEGRID_H
