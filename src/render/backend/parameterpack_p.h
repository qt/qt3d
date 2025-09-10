// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_RENDER_PARAMETERPACK_H
#define QT3DRENDER_RENDER_PARAMETERPACK_H

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

#include <Qt3DCore/qnodeid.h>
#include <QList>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class ParameterPack
{
public:
    ParameterPack();
    ~ParameterPack();
    void clear();
    void appendParameter(Qt3DCore::QNodeId parameterId);
    void removeParameter(Qt3DCore::QNodeId parameterId);
    QList<Qt3DCore::QNodeId> parameters() const;

    void setParameters(const Qt3DCore::QNodeIdVector &parameterIds)
    {
        m_peers = parameterIds;
    }

private:
    QList<Qt3DCore::QNodeId> m_peers;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PARAMETERPACK_H
