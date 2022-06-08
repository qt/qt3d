// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSTENCILOPERATIONARGUMENTS_P_H
#define QT3DRENDER_QSTENCILOPERATIONARGUMENTS_P_H

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

#include <QtCore/private/qobject_p.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QStencilOperationArgumentsPrivate : public QObjectPrivate
{
public:
    QStencilOperationArgumentsPrivate(QStencilOperationArguments::FaceMode mode)
        : QObjectPrivate()
        , m_face(mode)
        , m_stencilTestFailureOperation(QStencilOperationArguments::Keep)
        , m_depthTestFailureOperation(QStencilOperationArguments::Keep)
        , m_allTestsPassOperation(QStencilOperationArguments::Keep)
    {

    }
    ~QStencilOperationArgumentsPrivate();

    QStencilOperationArguments::FaceMode m_face;
    QStencilOperationArguments::Operation m_stencilTestFailureOperation;
    QStencilOperationArguments::Operation m_depthTestFailureOperation;
    QStencilOperationArguments::Operation m_allTestsPassOperation;
};

struct QStencilOperationArgumentsData
{
    QStencilOperationArguments::FaceMode face;
    QStencilOperationArguments::Operation stencilTestFailureOperation;
    QStencilOperationArguments::Operation depthTestFailureOperation;
    QStencilOperationArguments::Operation allTestsPassOperation;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSTENCILOPERATIONARGUMENTS_P_H
