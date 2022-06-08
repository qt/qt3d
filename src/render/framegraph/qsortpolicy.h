// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSORTPOLICY_H
#define QT3DRENDER_QSORTPOLICY_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSortPolicyPrivate;

class Q_3DRENDERSHARED_EXPORT QSortPolicy : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(QList<int> sortTypes READ sortTypesInt WRITE setSortTypes NOTIFY sortTypesChanged)
public:
    explicit QSortPolicy(Qt3DCore::QNode *parent = nullptr);
    ~QSortPolicy();

    enum SortType {
        StateChangeCost = (1 << 0),
        BackToFront = (1 << 1),
        Material = (1 << 2),
        FrontToBack = (1 << 3),
        Texture = (1 << 4),
        Uniform = (1 << 5)
    };
    Q_ENUM(SortType) // LCOV_EXCL_LINE

    QList<SortType> sortTypes() const;
    QList<int> sortTypesInt() const;

public Q_SLOTS:
    void setSortTypes(const QList<SortType> &sortTypes);
    void setSortTypes(const QList<int> &sortTypesInt);

Q_SIGNALS:
    void sortTypesChanged(const QList<SortType> &sortTypes);
    void sortTypesChanged(const QList<int> &sortTypes);

protected:
    explicit QSortPolicy(QSortPolicyPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSortPolicy)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSORTPOLICY_H
