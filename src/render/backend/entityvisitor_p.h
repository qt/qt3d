// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_ENTITYVISITOR_H
#define QT3DRENDER_RENDER_ENTITYVISITOR_H

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

#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class Entity;
class NodeManagers;

class Q_AUTOTEST_EXPORT EntityVisitor
{
public:
    enum Operation {
        Continue,   //! continue traversal
        Prune,      //! don't traverse children
        Stop        //! abort traversal
    };

    EntityVisitor(NodeManagers *manager);
    virtual ~EntityVisitor();

    virtual Operation visit(Entity *entity = nullptr);

    bool pruneDisabled() const;
    void setPruneDisabled(bool pruneDisabled);

    bool apply(Entity *root);

protected:
    NodeManagers *m_manager;

private:
    bool m_pruneDisabled;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ENTITYVISITOR_H
