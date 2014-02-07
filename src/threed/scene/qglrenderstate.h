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


#ifndef QGLRENDERSTATE_H
#define QGLRENDERSTATE_H

#include <Qt3D/qglnamespace.h>
#include <Qt3D/qglscenenode.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QGLAbstractEffect;
class QGLMaterial;

class QGLRenderStatePrivate;

class Q_QT3D_EXPORT QGLRenderState
{
public:
    QGLRenderState();
    QGLRenderState(const QGLRenderState &other);
    virtual ~QGLRenderState();
    QGLRenderState &operator=(const QGLRenderState &rhs);
    virtual void updateFrom(const QGLSceneNode *node);
    QGLAbstractEffect *userEffect() const;
    QGL::StandardEffect standardEffect() const;
    bool hasEffect() const;
    QGLMaterial *material() const;
    QGLMaterial *backMaterial() const;
    const QGLSceneNode *node() const;
    uint hash() const;
    bool operator==(const QGLRenderState &rhs) const
    {
        if (userEffect() != rhs.userEffect())
            return false;
        if (standardEffect() != rhs.standardEffect())
            return false;
        if (hasEffect() != rhs.hasEffect())
            return false;
        if (material() != rhs.material())
            return false;
        if (backMaterial() != rhs.backMaterial())
            return false;
        return true;
    }
    bool isValid() const;
private:
    void detach();
    QGLRenderStatePrivate *d;
};

inline uint qHash(const QGLRenderState &s)
{
    return s.hash();
}

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLRenderState &order);
#endif

QT_END_NAMESPACE

#endif // QGLRENDERSTATE_H
