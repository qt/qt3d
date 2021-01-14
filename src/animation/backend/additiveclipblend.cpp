/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#include "additiveclipblend_p.h"
#include <Qt3DAnimation/qadditiveclipblend.h>
#include <Qt3DAnimation/private/qadditiveclipblend_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace Animation {

AdditiveClipBlend::AdditiveClipBlend()
    : ClipBlendNode(ClipBlendNode::AdditiveBlendType)
    , m_baseClipId()
    , m_additiveClipId()
    , m_additiveFactor(0.0f)
{
}

AdditiveClipBlend::~AdditiveClipBlend()
{
}

void AdditiveClipBlend::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAdditiveClipBlend *node = qobject_cast<const QAdditiveClipBlend *>(frontEnd);
    if (!node)
        return;

    m_additiveFactor = node->additiveFactor();
    m_baseClipId = Qt3DCore::qIdForNode(node->baseClip());
    m_additiveClipId = Qt3DCore::qIdForNode(node->additiveClip());
}

ClipResults AdditiveClipBlend::doBlend(const QList<ClipResults> &blendData) const
{
    Q_ASSERT(blendData.size() == 2);
    Q_ASSERT(blendData[0].size() == blendData[1].size());
    const int elementCount = blendData.first().size();
    ClipResults blendResults(elementCount);

    for (int i = 0; i < elementCount; ++i)
        blendResults[i] = blendData[0][i] + m_additiveFactor * blendData[1][i];

    return blendResults;
}

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE
