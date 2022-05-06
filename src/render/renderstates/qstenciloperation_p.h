/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#ifndef QT3DRENDER_QSTENCILOPERATION_P_H
#define QT3DRENDER_QSTENCILOPERATION_P_H

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

#include <Qt3DRender/private/qrenderstate_p.h>
#include <Qt3DRender/qstenciloperation.h>
#include <Qt3DRender/qstenciloperationarguments.h>
#include <Qt3DRender/private/qstenciloperationarguments_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

struct QStencilOperationData
{
    QStencilOperationArgumentsData front;
    QStencilOperationArgumentsData back;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT QStencilOperationPrivate : public QRenderStatePrivate
{
public:
    QStencilOperationPrivate()
        : QRenderStatePrivate(Render::StencilOpMask)
        , m_front(new QStencilOperationArguments(QStencilOperationArguments::Front, q_ptr))
        , m_back(new QStencilOperationArguments(QStencilOperationArguments::Back, q_ptr))
    {}
    ~QStencilOperationPrivate();

    QStencilOperationArguments *m_front;
    QStencilOperationArguments *m_back;

    void fillData(QStencilOperationData &data) const;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QStencilOperationData)

#endif // QT3DRENDER_QSTENCILOPERATION_P_H
