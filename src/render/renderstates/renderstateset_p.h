// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERSTATE_H
#define QT3DRENDER_RENDER_RENDERSTATE_H

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

#include <Qt3DRender/private/genericstate_p.h>
#include <Qt3DRender/private/renderstates_p.h>
#include <Qt3DRender/private/statevariant_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderState;

namespace Render {

class RenderState;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderStateSet
{
public:
    RenderStateSet();
    ~RenderStateSet();

    template<typename GenericState>
    void addState(const GenericState &state)
    {
        addState(StateVariant::fromValue(state));
    }

    /**
     * @brief changeCost - metric of cost to change to this state-set from
     * a candidate previous state-set. This is used to find an optimal
     * ordering of state-sets when sending draw commands.
     * @param previousState
     * @return
     */
    int changeCost(RenderStateSet* previousState);

    StateMaskSet stateMask() const;
    void merge(const RenderStateSet *other);

    const std::vector<StateVariant>& states() const noexcept { return m_states; }
    std::vector<StateVariant>& states() noexcept { return m_states; }

    bool canAddStateOfType(StateMask type) const;

    /**
     * @brief contains - check if this set contains a matching piece of state
     * @param ds
     * @return
     */
    bool contains(const StateVariant &ds) const;

private:
    bool hasStateOfType(StateMask type) const;
    bool allowMultipleStatesOfType(StateMask type) const;

private:
    StateMaskSet m_stateMask;
    std::vector<StateVariant> m_states;
};

template<>
Q_3DRENDERSHARED_PRIVATE_EXPORT void RenderStateSet::addState<StateVariant>(const StateVariant &state);

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERSTATE_H
