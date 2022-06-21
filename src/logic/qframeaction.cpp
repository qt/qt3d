// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qframeaction.h"

#include "qframeaction_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {

QFrameActionPrivate::QFrameActionPrivate()
    : QComponentPrivate()
{
}

/*!
    \namespace Qt3DLogic::Logic
    \inmodule Qt3DLogic

    \brief Used to import and use the module's QML types.
*/
/*!
    \class Qt3DLogic::QFrameAction
    \inmodule Qt3DLogic
    \since 5.5
    \brief Provides a way to have a synchronous function executed each frame.

    The QFrameAction provides a way to perform tasks each frame in a
    synchronized way with the Qt3D backend. This is useful to implement some
    aspects of application logic and to prototype functionality that can later
    be folded into an additional Qt3D aspect.

    For example, the QFrameAction can be used to animate a property in sync
    with the Qt3D engine where a Qt Quick animation element is not perfectly
    synchronized and may lead to stutters in some cases.

    To execute your own code each frame connect to the QFrameAction::triggered signal.
*/

/*!
    \qmltype FrameAction
    \inqmlmodule Qt3D.Logic
    \instantiates Qt3DLogic::QFrameAction
    \inherits Component3D
    \since 5.5
    \brief Provides a way to have a synchronous function executed each frame.

    The FrameAction provides a way to perform tasks each frame in a
    synchronized way with the Qt3D backend. This is useful to implement some
    aspects of application logic and to prototype functionality that can later
    be folded into an additional Qt3D aspect.

    For example, the FrameAction can be used to animate a property in sync
    with the Qt3D engine where a Qt Quick animation element is not perfectly
    synchronized and may lead to stutters in some cases.

    To execute your own code each frame connect to the FrameAction::triggered signal.
*/

/*!
    Constructs a new QFrameAction instance with parent \a parent.
 */
QFrameAction::QFrameAction(QNode *parent)
    : QComponent(*new QFrameActionPrivate, parent)
{
}

/*! \internal */
QFrameAction::~QFrameAction()
{
}

/*! \internal */
QFrameAction::QFrameAction(QFrameActionPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
    \internal
    This function will be called in a synchronous manner once each frame by
    the Logic aspect.
*/
void QFrameAction::onTriggered(float dt)
{
    // Emit signal so that QML instances get the onTriggered() signal
    // handler called
    emit triggered(dt);
}

/*!
    \qmlsignal Qt3D.Logic::FrameAction::triggered(real dt)
    This signal is emitted each frame with \a dt being the time (in seconds) since the last triggering.
*/

/*!
    \fn Qt3DLogic::QFrameAction::triggered(float dt)
    This signal is emitted each frame with \a dt being the time (in seconds) since the last triggering.
*/
} // namespace Qt3DLogic

QT_END_NAMESPACE

#include "moc_qframeaction.cpp"
