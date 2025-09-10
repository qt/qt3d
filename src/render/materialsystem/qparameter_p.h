// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPARAMETER_P_H
#define QT3DRENDER_QPARAMETER_P_H

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

#include <private/qnode_p.h>
#include <private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameter;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QParameterPrivate : public Qt3DCore::QNodePrivate
{
public:
    QParameterPrivate();

    Q_DECLARE_PUBLIC(QParameter)

    virtual void setValue(const QVariant &v);

    QString m_name;
    QVariant m_value;
    QVariant m_backendValue;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPARAMETER_P_H
