/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qrendersettings.h"
#include "qrendersettings_p.h"
#include "qframegraphnode.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QRenderSettings
 * \brief The QRenderSettings class
 * \since 5.7
 * \inmodule Qt3DRender
 */

/*! \internal */
QRenderSettingsPrivate::QRenderSettingsPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_activeFrameGraph(nullptr)
    , m_renderPolicy(QRenderSettings::Always)
{
}

/*! \internal */
void QRenderSettingsPrivate::init()
{
    Q_Q(QRenderSettings);
    QObject::connect(&m_pickingSettings, SIGNAL(pickMethodChanged(QPickingSettings::PickMethod)),
                     q, SLOT(_q_onPickingMethodChanged(QPickingSettings::PickMethod)));
    QObject::connect(&m_pickingSettings, SIGNAL(pickResultModeChanged(QPickingSettings::PickResultMode)),
                     q, SLOT(_q_onPickResultModeChanged(QPickingSettings::PickResultMode)));
}

/*! \internal */
void QRenderSettingsPrivate::_q_onPickingMethodChanged(QPickingSettings::PickMethod pickMethod)
{
    notifyPropertyChange("pickMethod", pickMethod);
}

/*! \internal */
void QRenderSettingsPrivate::_q_onPickResultModeChanged(QPickingSettings::PickResultMode pickResultMode)
{
    notifyPropertyChange("pickResultMode", pickResultMode);
}

/*!
 * The constructor creates a new QRenderSettings::QRenderSettings instance with the
 * specified \a parent.
 * \param parent
 */
QRenderSettings::QRenderSettings(Qt3DCore::QNode *parent)
    : QRenderSettings(*new QRenderSettingsPrivate, parent) {}

/*! \internal */
QRenderSettings::QRenderSettings(QRenderSettingsPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(dd, parent)
{
    Q_D(QRenderSettings);
    d->init();
}

/*! \internal */
QRenderSettings::~QRenderSettings()
{
}

/*!
 * \return the current picking settings.
 */
QPickingSettings *QRenderSettings::pickingSettings()
{
    Q_D(QRenderSettings);
    return &(d->m_pickingSettings);
}

/*!
 * \return the current active framegraph.
 */
QFrameGraphNode *QRenderSettings::activeFrameGraph() const
{
    Q_D(const QRenderSettings);
    return d->m_activeFrameGraph;
}

/*!
 * \return the current render policy
 */
QRenderSettings::RenderPolicy QRenderSettings::renderPolicy() const
{
    Q_D(const QRenderSettings);
    return d->m_renderPolicy;
}

/*!
 * Sets the active framegraph to \a activeFrameGraph.
 * \param activeFrameGraph
 */
void QRenderSettings::setActiveFrameGraph(QFrameGraphNode *activeFrameGraph)
{
    Q_D(QRenderSettings);
    if (d->m_activeFrameGraph == activeFrameGraph)
        return;

    if (d->m_activeFrameGraph)
        d->unregisterDestructionHelper(d->m_activeFrameGraph);

    if (activeFrameGraph != nullptr && !activeFrameGraph->parent())
        activeFrameGraph->setParent(this);

    d->m_activeFrameGraph = activeFrameGraph;

    // Ensures proper bookkeeping
    if (d->m_activeFrameGraph)
        d->registerDestructionHelper(d->m_activeFrameGraph, &QRenderSettings::setActiveFrameGraph, d->m_activeFrameGraph);

    emit activeFrameGraphChanged(activeFrameGraph);
}

/*!
 * Sets the render policy to \a renderPolicy.
 * \param renderPolicy
 */
void QRenderSettings::setRenderPolicy(QRenderSettings::RenderPolicy renderPolicy)
{
    Q_D(QRenderSettings);
    if (d->m_renderPolicy == renderPolicy)
        return;

    d->m_renderPolicy = renderPolicy;
    emit renderPolicyChanged(renderPolicy);
}

Qt3DCore::QNodeCreatedChangeBasePtr QRenderSettings::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QRenderSettingsData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QRenderSettings);
    data.activeFrameGraphId = qIdForNode(d->m_activeFrameGraph);
    data.renderPolicy = d->m_renderPolicy;
    data.pickMethod = d->m_pickingSettings.pickMethod();
    data.pickResultMode = d->m_pickingSettings.pickResultMode();
    return creationChange;
}

} // namespace Qt3Drender

QT_END_NAMESPACE

#include "moc_qrendersettings.cpp"
