/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
