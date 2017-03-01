/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscene2d.h"
#include "qscene2d_p.h"
#include "scene2d_p.h"
#include "scene2dmanager_p.h"
#include "scene2devent_p.h"

#include <Qt3DCore/QPropertyUpdatedChange>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

namespace Quick {

/*!
    \class Qt3DRender::QScene2D
    \inmodule Qt3DRender

    \brief This class enables rendering qml into a texture, which then can be
    used as a part of 3D scene.

    The component uses QQuickRenderControl to render the given QML source into an
    offscreen surface, which is attached to a texture provided by the user. This allows the
    component to directly render into the texture without intermediate copy and the user to
    freely specify how the texture is used in the 3D scene.

    \since 5.9
*/

/*!
    \qmltype Scene2D
    \inqmlmodule Qt3D.Render
    \since
    \ingroup
    \instantiates Qt3DRender::QScene2D
    \brief Scene2D
 */

/*!
    \enum QScene2D::RenderPolicy

    This enum type describes types of render policies available.
    \value Continuous The Scene2D is rendering continuously. This is the default render policy.
    \value SingleShot The Scene2D renders to the texture only once after which the resources
                      allocated for rendering are released.
*/

/*!
    \qmlproperty RenderTargetOutput Qt3D.Render::Scene2D::output
    Holds the RenderTargetOutput, which specifies where the Scene2D is rendering to.
 */

/*!
    \qmlproperty QUrl Qt3D.Render::Scene2D::source
    Holds the qml source url.
 */

/*!
    \qmlproperty enumeration Qt3D.Render::Scene2D::renderPolicy
    Holds the render policy of this Scene2D.
 */

/*!
    \qmlproperty bool Qt3D.Render::Scene2D::loaded
    Holds whether the source has been loaded.
 */

QScene2DPrivate::QScene2DPrivate()
    : Qt3DCore::QNodePrivate()
    , m_renderManager(new Scene2DManager(this))
    , m_output(nullptr)
{
}

QScene2DPrivate::~QScene2DPrivate()
{
    m_renderManager->cleanup();
    delete m_renderManager;
}


/*!
    The constructor creates a new QScene2D instance with the specified \a parent.
 */
QScene2D::QScene2D(Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QScene2DPrivate, parent)
{
    Q_D(QScene2D);
    connect(d->m_renderManager, &Scene2DManager::onLoadedChanged,
            this, &QScene2D::sourceLoaded);
}

QScene2D::QScene2D(QQmlEngine *engine, Qt3DCore::QNode *parent)
    : Qt3DCore::QNode(*new QScene2DPrivate, parent)
{
    Q_D(QScene2D);
    connect(d->m_renderManager, &Scene2DManager::onLoadedChanged,
            this, &QScene2D::sourceLoaded);
    d->m_renderManager->setEngine(engine);
}

QScene2D::~QScene2D()
{
}

bool QScene2D::loaded() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_initialized;
}

/*!
    \property QScene2D::source
    \brief Specifies the url for the qml.

    This property specifies the url to the qml being rendered to the texture.
    The source must specify QQuickItem as a root. The item must specify width
    and height. The rendered qml is scaled to the texture size.
    The property can not be changed after the rendering has been initialized.
 */
QUrl QScene2D::source() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_source;
}

void QScene2D::setSource(const QUrl &url)
{
    Q_D(QScene2D);
    if (d->m_renderManager->m_initialized) {
        qWarning() << "Unable to set source after initialization.";
        return;
    }
    if (d->m_renderManager->m_source != url) {
        d->m_renderManager->setSource(url);
        emit sourceChanged(url);
    }
}

QQuickItem* QScene2D::item() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_item;
}

void QScene2D::setItem(QQuickItem *item)
{
    Q_D(QScene2D);
    if (d->m_renderManager->m_initialized) {
        qWarning() << "Unable to set item after initialization.";
        return;
    }
    if (d->m_renderManager->m_item != item) {
        d->m_renderManager->setItem(item);
        emit itemChanged(item);
    }
}

/*!
    \property QScene2D::renderPolicy

    Holds the render policy of this Scene2D.
 */
QScene2D::RenderPolicy QScene2D::renderPolicy() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_renderPolicy;
}

void QScene2D::setRenderPolicy(QScene2D::RenderPolicy renderPolicy)
{
    Q_D(const QScene2D);
    if (d->m_renderManager->m_renderPolicy != renderPolicy) {
        d->m_renderManager->m_renderPolicy = renderPolicy;
        emit renderPolicyChanged(renderPolicy);
    }
}

/*!
    \property QScene2D::output
    Holds the QRenderTargetOutput, which specifies where the QScene2D is
    rendering to.
 */
Qt3DRender::QRenderTargetOutput *QScene2D::output() const
{
    Q_D(const QScene2D);
    return d->m_output;
}

void QScene2D::setOutput(Qt3DRender::QRenderTargetOutput *output)
{
    Q_D(QScene2D);
    if (d->m_output != output) {
        if (d->m_output)
            d->unregisterDestructionHelper(d->m_output);
        d->m_output = output;
        if (output)
            d->registerDestructionHelper(output, &QScene2D::setOutput, d->m_output);
        emit outputChanged(output);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QScene2D::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QScene2DData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QScene2D);
    data.renderPolicy = d->m_renderManager->m_renderPolicy;
    data.sharedObject = d->m_renderManager->m_sharedObject;
    data.output = d->m_output ? d->m_output->id() : Qt3DCore::QNodeId();
    return creationChange;
}

bool QScene2D::event(QEvent *event)
{
    Q_D(QScene2D);
    d->m_renderManager->forwardEvent(event);
    return true;
}

/*!
    Returns the qml engine used by the QScene2D.
 */
QQmlEngine *QScene2D::engine() const
{
    Q_D(const QScene2D);
    return d->m_renderManager->m_qmlEngine;
}

/*!
    \internal
 */
void QScene2D::sourceLoaded()
{
    emit loadedChanged(true);
}

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE
