/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "billboarditem3d.h"
#include "qgraphicsbillboardtransform.h"
#include "qquickeffect.h"

/*!
    \qmltype BillboardItem3D
    \instantiates BillboardItem3D
    \brief The BillboardItem3D will always face toward the viewer.
    \since 4.8
    \ingroup qt3d::qml3d

    Sometimes it is desirable to have objects which always face toward the
    camera.  For example, a quad with text on it may always face the camera
    so as to be readable at all times.

    While this can be achieved with a QGraphicsLookAtTransform on a normal
    Item3D, the BillboardItem3D class provides a more efficient "cheat" which
    takes advantage of the underlying 3D mathematics.

    To use a BillboardItem3D declare it exactly like a regular Item3D.  When
    the item is drawn a QGraphicsBillboardTransform will be applied to the item
    after all other transforms have been performed.

    For example:

    \code
    Viewport {
        BillboardItem3D {
            mesh: Mesh { source: "model.obj" }
            effect: Effect {
                blending: true
                texture: "texture.png"
            }
        }
    }
    \endcode

    For a practical illustration of its use see the forest example.
*/
BillboardItem3D::BillboardItem3D(QObject *parent)
    : QQuickItem3D(parent),
      m_preserveUpVector(false),
      m_bConnectedToOpenGLContextSignal(false)
{
}

/*!
    \qmlproperty bool BillboardItem3D::preserveUpVector

    This property specifies whether the billboard transform should
    preserve the "up vector" so that objects stay at right angles
    to the ground plane in the scene.

    The default value for this property is false, which indicates that
    the object being transformed should always face directly to the camera
    This is also known as a "spherical billboard".

    If the value for this property is true, then the object will have
    its up orientation preserved.  This is also known as a "cylindrical
    billboard".
*/
bool BillboardItem3D::preserveUpVector() const
{
    return m_preserveUpVector;
}

void BillboardItem3D::setPreserveUpVector(bool value)
{
    m_preserveUpVector = value;
    update();
}

/*!
    \internal
    This replaces the standard draw() as used in Item3D.  In this instance all drawing
    carried out using \a painter follows the standard sequence.  However, after the
    transforms for the item have been applied, a QGraphicsBillboardTransform is applied
    to the model-view matrix.

    After the current item is drawn the model-view matrix from immediately before the
    billboard transform being applied will be restored so child items are not affected by it.
*/
void BillboardItem3D::draw(QGLPainter *painter)
{
    // Bail out if this item and its children have been disabled.
    if (!isEnabled())
        return;
    if (!isInitialized())
        initialize(painter);

    if (!m_bConnectedToOpenGLContextSignal) {
        QOpenGLContext* pOpenGLContext = QOpenGLContext::currentContext();
        if (pOpenGLContext) {
            bool Ok = QObject::connect(pOpenGLContext, SIGNAL(aboutToBeDestroyed()), this, SLOT(handleOpenglContextIsAboutToBeDestroyed()), Qt::DirectConnection);
            Q_UNUSED(Ok);  // quell compiler warning
            Q_ASSERT(Ok);
            m_bConnectedToOpenGLContextSignal = true;
        }
    }

    //Setup picking
    int prevId = painter->objectPickId();
    painter->setObjectPickId(objectPickId());

    //Setup effect (lighting, culling, effects etc)
    const QGLLightParameters *currentLight = 0;
    QMatrix4x4 currentLightTransform;
    drawLightingSetup(painter, currentLight, currentLightTransform);
    bool viewportBlend, effectBlend;
    drawEffectSetup(painter, viewportBlend, effectBlend);
    drawCullSetup();

    //Local and Global transforms
    drawTransformSetup(painter);

    //After all of the other transforms, apply the billboard transform to
    //ensure forward facing.
    painter->modelViewMatrix().push();
    QGraphicsBillboardTransform bill;
    bill.setPreserveUpVector(m_preserveUpVector);
    bill.applyTo(const_cast<QMatrix4x4 *>(&painter->modelViewMatrix().top()));

    //Drawing
    drawItem(painter);

    //Pop the billboard transform from the model-view matrix stack so that it
    //is not applied to child items.
    painter->modelViewMatrix().pop();

    //Draw children
    drawChildren(painter);

    //Cleanup
    drawTransformCleanup(painter);
    drawLightingCleanup(painter, currentLight, currentLightTransform);
    drawEffectCleanup(painter, viewportBlend, effectBlend);
    drawCullCleanup();

    //Reset pick id.
    painter->setObjectPickId(prevId);
}

void BillboardItem3D::handleOpenglContextIsAboutToBeDestroyed()
{
    if (effect()) {
        effect()->openglContextIsAboutToBeDestroyed();
    }
}

