// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#include "qcullface.h"
#include "qcullface_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QCullFace
    \brief The QCullFace class specifies whether front or back face culling
    is enabled.
    \since 5.7
    \inmodule Qt3DRender
    \ingroup renderstates

    QCullFace sets whether the front or back facets are culled.
    Facets include triangles, quadrilaterals, polygons and rectangles.

    It can be added by calling the addRenderState() method on a QRenderPass:

    \code
    // using namespace Qt3DRender;

    QRenderPass *renderPass = new QRenderPass();

    // Create a front face culling render state
    QCullFace *cullFront = new QCullFace();
    cullFront->setMode(QCullFace::Front);

    // Add the render state to the render pass
    renderPass->addRenderState(cullFront);
    \endcode

    Or by calling the addRenderState() method on a QRenderStateSet:

    \code
    // using namespace Qt3DRender;

    QRenderStateSet *renderStateSet = new QRenderStateSet();

    // Create a front face culling render state
    QCullFace *cullFront = new QCullFace();
    cullFront->setMode(QCullFace::Front);

    // Add the render state to the render pass
    renderStateSet->addRenderState(cullFront);
    \endcode

    \sa QFrontFace
 */

/*!
    \qmltype CullFace
    \brief The CullFace type specifies whether front or back face culling
    is enabled.
    \since 5.7
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QCullFace
    \inherits RenderState
    \ingroup renderstates

    CullFace sets whether the front or back facets are culled.
    Facets include triangles, quadrilaterals, polygons and rectangles.

    It can be added to the renderStates property of a RenderPass:

    \qml
    RenderPass {
        shaderProgram: ShaderProgram {
            // ...
        }
        renderStates: [
            CullFace {
                mode: CullFace.Front
            }
        ]
    }
    \endqml

    Or added to the renderStates property of a RenderStateSet:

    \qml
    RenderStateSet {
        renderStates: [
            CullFace {
                mode: CullFace.Front
            }
        ]
    }
    \endqml

    \sa FrontFace
 */

/*!
    \enum Qt3DRender::QCullFace::CullingMode

    This enumeration specifies values for the culling mode.

    \value NoCulling Culling is disabled
    \value Front Culling is enabled for front facing polygons
    \value Back Culling is enabled for back facing polygons
    \value FrontAndBack Culling is enabled for all polygons, points and lines are drawn
*/

/*!
    \qmlproperty enumeration CullFace::mode
    Holds the culling mode used by CullFace. Default is set to QCullFace.Back.

    \list
    \li CullFace.NoCulling - culling is disabled
    \li CullFace.Front - culling is enabled for front facing polygons
    \li CullFace.Back - culling is enabled for back facing polygons
    \li CullFace.FrontAndBack - culling is enabled for all polygons, but points and lines are drawn
    \endlist
*/

/*!
    \property Qt3DRender::QCullFace::mode
    Holds the culling mode used by QCullFace. Default is set to QCullFace.Back.
*/

/*!
     Constructs a new QCullFace::QCullFace instance with \a parent as parent.
 */
QCullFace::QCullFace(QNode *parent)
    : QRenderState(*new QCullFacePrivate, parent)
{
}

/*! \internal */
QCullFace::~QCullFace()
{
}

QCullFace::CullingMode QCullFace::mode() const
{
    Q_D(const QCullFace);
    return d->m_mode;
}

void QCullFace::setMode(QCullFace::CullingMode mode)
{
    Q_D(QCullFace);
    if (d->m_mode != mode) {
        d->m_mode = mode;
        emit modeChanged(mode);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qcullface.cpp"
