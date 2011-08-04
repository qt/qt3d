/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglrenderstate.h"

/*!
    \class QGLRenderState
    \brief The QGLRenderState class encapsulates the states of a rendering pass.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    The QGLRenderState class works with the QGLRenderOrder class to optimize
    the rendering order of scene nodes.

    In general instances of this class are managed by the render sequencer
    and it should not be necessary to explicitly create or manipulate them.

    A QGLRenderState instance encapsulates rendering properties for a given
    path through the rendering process.  The state includes properties such as
    effects, and materials; which may either be directly set on a scene node,
    or inherited from its parents.

    QGLRenderState may be subclassed in advanced applications to provide a
    different inheritance pattern for rendering properties - reimplement the
    updateFrom() function to do this.  The default implementation simply sets
    an effect on the state if the argument node has an effect (found by
    calling the QGLSceneNode::hasEffect() function), and sets a material on
    the state if the node has a non-null material; otherwise the existing
    state is left as it is.

    The render sequencer will ensure that the relevant render state is set
    onto the painter at the beginning of the pass which renders all nodes
    with that state.

    \sa QGLRenderOrder
*/

class QGLRenderStatePrivate : public QSharedData
{
public:
    QGLRenderStatePrivate();
    ~QGLRenderStatePrivate();
    QGLRenderStatePrivate *clone() const;

    QBasicAtomicInt ref;

    bool hasEffect;
    QGLMaterial *material;
    QGLMaterial *backMaterial;
    QGL::StandardEffect standardEffect;
    QGLAbstractEffect *userEffect;
    const QGLSceneNode *node;
};

QGLRenderStatePrivate::QGLRenderStatePrivate()
    : hasEffect(false)
    , material(0)
    , backMaterial(0)
    , standardEffect(QGL::FlatColor)
    , userEffect(0)
    , node(0)
{
    ref = 0;
}

QGLRenderStatePrivate::~QGLRenderStatePrivate()
{
}

QGLRenderStatePrivate *QGLRenderStatePrivate::clone() const
{
    QGLRenderStatePrivate *r = new QGLRenderStatePrivate;
    r->hasEffect = hasEffect;
    r->material = material;
    r->backMaterial = backMaterial;
    r->standardEffect = standardEffect;
    r->userEffect = userEffect;
    r->node = node;
    return r;
}

/*!
    Creates a new QGLRenderState empty of any values.
*/
QGLRenderState::QGLRenderState()
    : d(0)
{
}

/*!
    Creates a new QGLRenderState as a copy of \a other
*/
QGLRenderState::QGLRenderState(const QGLRenderState &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
    Destroys this QGLRenderState recovering and resources.
*/
QGLRenderState::~QGLRenderState()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Assigns this QGLRenderState to be a copy of \a rhs.
*/
QGLRenderState &QGLRenderState::operator=(const QGLRenderState &rhs)
{
    if (d != rhs.d)
    {
        if (d && !d->ref.deref())
            delete d;
        d = rhs.d;
        if (d)
            d->ref.ref();
    }
    return *this;
}

/*!
    Sets the values of this QGLRenderState from the \a node, where
    the node has a value.  For example if the \a node has an effect
    but no material, then this state will have its effect changed to
    that of the node, but this state's material will be unchanged.
*/
void QGLRenderState::updateFrom(const QGLSceneNode *node)
{
    detach();
    if (node->hasEffect())
    {
        d->hasEffect = true;
        if (node->userEffect())
            d->userEffect = node->userEffect();
        else
            d->standardEffect = node->effect();
    }
    if (node->material())
        d->material = node->material();
    if (node->backMaterial())
        d->backMaterial = node->backMaterial();
    d->node = node;
}

/*!
    Returns the user effect stored on this QGLRenderState, or null if no
    user effect has been set.  The default value is null.
*/
QGLAbstractEffect *QGLRenderState::userEffect() const
{
    QGLAbstractEffect *e = 0;
    if (d)
        e = d->userEffect;
    return e;
}

/*!
    Returns the standard effect stored on this QGLRenderState.  The
    default value is QGL::FlatColor.  To determine if an effect has
    been specifically set call hasEffect().
*/
QGL::StandardEffect QGLRenderState::standardEffect() const
{
    QGL::StandardEffect e = QGL::FlatColor;
    if (d)
        e = d->standardEffect;
    return e;
}

/*!
    Returns true is there is a valid effect on this QGLRenderState.  The
    default value is false.
*/
bool QGLRenderState::hasEffect() const
{
    bool r = false;
    if (d)
        r = d->hasEffect;
    return r;
}

/*!
    Returns the material stored on this QGLRenderState, or null if no
    material has been set.  The default value is null.
*/
QGLMaterial *QGLRenderState::material() const
{
    QGLMaterial *m = 0;
    if (d)
        m = d->material;
    return m;
}

/*!
    Returns the back material stored on this QGLRenderState, or null if no
    back material has been set.  The default value is null.
*/
QGLMaterial *QGLRenderState::backMaterial() const
{
    QGLMaterial *m = 0;
    if (d)
        m = d->backMaterial;
    return m;
}

/*!
    Returns the node used to populate this QGLRenderState.
*/
const QGLSceneNode *QGLRenderState::node() const
{
    const QGLSceneNode *s = 0;
    if (d)
        s = d->node;
    return s;
}

/*!
    Returns true if this is a valid representation of a render state, that
    is if it has ever been updated from a node; and false otherwise.
*/
bool QGLRenderState::isValid() const
{
    if (d && d->node)
        return true;
    return false;
}

/*!
    \fn bool QGLRenderState::operator==(const QGLRenderState &rhs) const
    Returns true if this state is equal to \a rhs, that is if each of the
    materials, effects and values are equal to those of \a rhs.
*/
void QGLRenderState::detach()
{
    if (!d) // lazy creation of data block
    {
        d = new QGLRenderStatePrivate;
        d->ref.ref();
    }
    else
    {
        if (d->ref > 1)  // being shared, must detach
        {
            QGLRenderStatePrivate *temp = d->clone();
            d->ref.deref();
            d = temp;
            d->ref.ref();
        }
    }
}

/*!
    Returns a hash value representing this state.
*/
uint QGLRenderState::hash() const
{
    const QByteArray bytes((const char *)d, sizeof(d));
    return qHash(bytes);
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLRenderState &order)
{
    dbg << "QGLRenderState" << &order << "-- user effect:" << order.userEffect()
            << "-- standardEffect:" << order.standardEffect()
            << "-- hasEffect:" << order.hasEffect()
            << "-- material:" << order.material()
            << "-- back material:" << order.backMaterial()
            << "-- node:" << order.node();
    return dbg;
}

#endif
