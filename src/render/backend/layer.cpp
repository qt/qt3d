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

#include "layer_p.h"
#include "qlayer.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Layer::Layer()
    : BackendNode()
{
}

Layer::~Layer()
{
    cleanup();
}

void Layer::cleanup()
{
}

void Layer::updateFromPeer(Qt3DCore::QNode *peer)
{
    QLayer *layer = static_cast<QLayer *>(peer);
    m_layers = layer->names();
    m_layerIds.reserve(m_layers.size());
    Q_FOREACH (const QString &name, m_layers)
        m_layerIds.push_back(StringToInt::lookupId(name));
    m_enabled = layer->isEnabled();
}

QStringList Layer::layers() const
{
    return m_layers;
}

void Layer::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    if (e->type() == NodeUpdated && propertyChange->propertyName() == QByteArrayLiteral("names")) {
        m_layers = propertyChange->value().toStringList();
        m_layerIds.clear();
        m_layerIds.reserve(m_layers.size());
        Q_FOREACH (const QString &name, m_layers)
            m_layerIds.push_back(StringToInt::lookupId(name));
    } else if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
        m_enabled = propertyChange->value().toBool();
    }
    markDirty(BackendNodeDirtyFlag::Any);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
