// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QTECHNIQUE_P_H
#define QT3DRENDER_QTECHNIQUE_P_H

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
#include <Qt3DRender/qgraphicsapifilter.h>
#include <private/qgraphicsapifilter_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTechnique;
class QFilterKey;
class QParameter;
class QRenderPass;

class QTechniquePrivate : public Qt3DCore::QNodePrivate
{
public:
    QTechniquePrivate();
    ~QTechniquePrivate();

    Q_DECLARE_PUBLIC(QTechnique)
    QList<QFilterKey *> m_filterKeys;
    QList<QParameter *> m_parameters;
    QList<QRenderPass *> m_renderPasses;
    QGraphicsApiFilter m_graphicsApiFilter;

private:
    void _q_graphicsApiFilterChanged();
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QTECHNIQUE_P_H
