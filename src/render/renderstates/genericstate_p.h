// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_GENERICSTATE_H
#define QT3DRENDER_RENDER_GENERICSTATE_H

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
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRender/private/statemask_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
//#include <Qt3DRender/private/statevariant_p.h>
#include <QList>
#include <QVector3D>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderState;

namespace Render {

class GraphicsContext;

typedef quint64 StateMaskSet;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderStateImpl
{
public:
    virtual ~RenderStateImpl() {}

    virtual StateMask mask() const = 0;
    virtual bool equalTo(const RenderStateImpl &renderState) const = 0;
    virtual void updateProperties(const QRenderState *);
};

template <class StateSetImpl, StateMask stateMask, typename ... T>
class GenericState : public RenderStateImpl
{
public:
    GenericState *set(T... values)
    {
        m_values = std::tuple<T ...>(values...);
        return this;
    }

    bool equalTo(const RenderStateImpl &renderState) const override
    {
        const GenericState *other = static_cast<const GenericState*>(&renderState);
        return (other != nullptr && other->m_values == m_values);
    }

    StateMask mask() const override
    {
        return GenericState::type();
    }

    static StateMask type()
    {
        return stateMask;
    }

    std::tuple<T ...> values() const
    {
        return m_values;
    }

protected:
    std::tuple<T ...> m_values;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GENERICSTATE_H
