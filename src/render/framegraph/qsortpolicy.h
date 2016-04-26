/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_QSORTPOLICY_H
#define QT3DRENDER_QSORTPOLICY_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSortPolicyPrivate;

class QT3DRENDERSHARED_EXPORT QSortPolicy : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(QVariantList sortTypes READ sortTypeList WRITE setSortTypes NOTIFY sortTypesChanged)
public:
    explicit QSortPolicy(Qt3DCore::QNode *parent = 0);

    enum SortType {
        StateChangeCost = (1 << 0),
        BackToFront = (1 << 1),
        Material = (1 << 2)
    };
    Q_ENUM(SortType)

    void addSortType(SortType sortType);
    void removeSortType(SortType sortType);
    QVector<SortType> sortTypes() const;
    QVariantList sortTypeList() const;

public Q_SLOTS:
    void setSortTypes(QVector<QSortPolicy::SortType> sortTypes);
    void setSortTypes(QVariantList sortTypes);

Q_SIGNALS:
    void sortTypesChanged(QVector<SortType> sortTypes);
    void sortTypesChanged(QVariantList sortTypes);

protected:
    QSortPolicy(QSortPolicyPrivate &dd, Qt3DCore::QNode *parent = 0);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QSortPolicy)
    QT3D_CLONEABLE(QSortPolicy)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSORTPOLICY_H
