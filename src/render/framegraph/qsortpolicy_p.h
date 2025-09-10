// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSORTPOLICY_P_H
#define QT3DRENDER_QSORTPOLICY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qframegraphnode_p.h>
#include "qsortpolicy.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {


class QSortPolicyPrivate : public QFrameGraphNodePrivate
{
public:
    QSortPolicyPrivate();
    Q_DECLARE_PUBLIC(QSortPolicy)
    QList<QSortPolicy::SortType> m_sortTypes;
};

} // namespace Qt3DRender

namespace {
    template<class From, class To>
    To typeCastHelper(From type)
    {
        return static_cast<To>(type);
    }

    template<class From, class To>
    void transformVector(const QList<From> &input, QList<To> &output)
    {
        Q_ASSERT(output.isEmpty());

        output.reserve(input.size());
        std::transform(input.constBegin(), input.constEnd(),
                       std::back_inserter(output),
                       typeCastHelper<From, To>);
    }
}

QT_END_NAMESPACE

#endif // QT3DRENDER_QSORTPOLICY_P_H
