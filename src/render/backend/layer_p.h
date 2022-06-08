// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LAYER_H
#define QT3DRENDER_RENDER_LAYER_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qt3drender_global.h>
#include <QStringList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLayer;

namespace Render {

class LayerManager;

class Layer : public BackendNode
{
public:
    Layer();
    ~Layer();
    void cleanup();

    // QBackendNode interface
    bool recursive() const;
    void setRecursive(bool recursive);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    bool m_recursive;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LAYER_H
