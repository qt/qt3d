/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "parameter_p.h"
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/private/qparameter_p.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/private/buffer_p.h>

#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/stringtoint_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Parameter::Parameter()
    : BackendNode()
    , m_nameId(-1)
{
}

void Parameter::cleanup()
{
    QBackendNode::setEnabled(false);
    m_nameId = -1;
    m_name.clear();
    m_uniformValue = UniformValue();
    m_backendValue = {};
}

void Parameter::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QParameter *node = qobject_cast<const QParameter *>(frontEnd);
    if (!node)
        return;

    AbstractRenderer::BackendNodeDirtySet dirty = firstTime ? AbstractRenderer::ParameterDirty : static_cast<AbstractRenderer::BackendNodeDirtyFlag>(0);
    if (node->isEnabled() != isEnabled())
        dirty |= (AbstractRenderer::MaterialDirty | AbstractRenderer::ParameterDirty);

    if (node->name() != m_name) {
        m_name = node->name();
        m_nameId = StringToInt::lookupId(m_name);
        dirty |= (AbstractRenderer::MaterialDirty | AbstractRenderer::ParameterDirty);
    }

    QParameterPrivate* d = static_cast<QParameterPrivate *>(QParameterPrivate::get(const_cast<QParameter *>(node)));
    if (d->m_backendValue != m_backendValue) {
        m_backendValue = d->m_backendValue;
        m_uniformValue = UniformValue::fromVariant(m_backendValue);
        dirty |= (AbstractRenderer::ParameterDirty);
    }

    if (dirty)
        markDirty(dirty);

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
}

QString Parameter::name() const
{
    return m_name;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
