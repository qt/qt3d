// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qalphacoverage.h"
#include "qrenderstate_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QAlphaCoverage
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender
    \brief Enable alpha-to-coverage multisampling mode.

    A Qt3DRender::QAlphaCoverage class enables alpha-to-coverage multisampling mode.
    When enabled, the fragment alpha value is used as a coverage for the sample
    and combined with fragment coverage value. Qt3DRender::QAlphaCoverage does
    nothing if multisampling is disabled. Alpha-to-coverage is most useful when
    order independent blending is required, for example when rendering leaves,
    grass and other rich vegetation.

    It can be added to a QRenderPass by calling QRenderPass::addRenderState():

    \code
    QRenderPass *renderPass = new QRenderPass();

    // Create a alpha coverage render state
    QAlphaCoverage *alphaCoverage = new QAlphaCoverage();
    QMultiSampleAntiAliasing *multiSampleAntialiasing = new QMultiSampleAntiAliasing();

    // Add the render states to the render pass
    renderPass->addRenderState(alphaCoverage);
    renderPass->addRenderState(multiSampleAntialiasing);
    \endcode

    Or to a QRenderStateSet by calling QRenderStateSet::addRenderState():

    \code
    QRenderStateSet *renderStateSet = new QRenderStateSet();

    // Create a alpha coverage render state
    QAlphaCoverage *alphaCoverage = new QAlphaCoverage();
    QMultiSampleAntiAliasing *multiSampleAntialiasing = new QMultiSampleAntiAliasing();

    // Add the render states to the render state set
    renderStateSet->addRenderState(alphaCoverage);
    renderStateSet->addRenderState(multiSampleAntialiasing);
    \endcode

    \sa Qt3DRender::QMultiSampleAntiAliasing
 */

/*!
    \qmltype AlphaCoverage
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QAlphaCoverage
    \inherits RenderState
    \brief Enable alpha-to-coverage multisampling mode.

    An AlphaCoverage type enables alpha-to-coverage multisampling mode.
    When enabled, the fragment alpha value is used as a coverage for the sample
    and combined with fragment coverage value. AlphaCoverage does nothing if
    multisampling is disabled. Alpha-to-coverage is most useful when
    order independent blending is required, for example when rendering leaves,
    grass and other rich vegetation.

    It can be added to a RenderPass:

    \qml
    RenderPass {
        shaderProgram: ShaderProgram {
            // ...
        }
        renderStates: [
            AlphaCoverage {},
            MultiSampleAntiAliasing {}
        ]
    }
    \endqml

    Or to a RenderStateSet:

    \qml
    RenderStateSet {
        renderStates: [
            AlphaCoverage {},
            MultiSampleAntiAliasing {}
        ]
    }
    \endqml

    \sa MultiSampleAntiAliasing
 */

class QAlphaCoveragePrivate : public QRenderStatePrivate
{
public :
    QAlphaCoveragePrivate()
        : QRenderStatePrivate(Render::AlphaCoverageStateMask)
    {}

    Q_DECLARE_PUBLIC(QAlphaCoverage)
};

/*!
    The constructor creates a new QAlphaCoverage::QAlphaCoverage instance
    with the specified \a parent.
 */
QAlphaCoverage::QAlphaCoverage(QNode *parent)
    : QRenderState(*new QAlphaCoveragePrivate, parent)
{
}

/*! \internal */
QAlphaCoverage::~QAlphaCoverage()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qalphacoverage.cpp"
