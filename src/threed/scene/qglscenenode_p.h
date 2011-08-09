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

#ifndef QGLSCENENODE_P_H
#define QGLSCENENODE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglnamespace.h"
#include "qglscenenode.h"
#include "qgraphicstransform3d.h"

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qlist.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qset.h>

class QGLAbstractEffect;
class QGLPickNode;

class QGLSceneNodePrivate
{
public:
    QGLSceneNodePrivate()
        : palette(0)
        , localEffect(QGL::FlatColor)   // 0 - zero
        , customEffect(0)
        , hasEffect(false)
        , material(-1)
        , backMaterial(-1)
        , start(0)
        , count(0)
        , options(0)
        , pickNode(0)
        , boxValid(false)
        , drawingMode(QGL::Triangles)
        , culled(false)
    {
    }

    // This constructor is used by QGLSceneNode::clone().
    QGLSceneNodePrivate(const QGLSceneNodePrivate *other)
        : geometry(other->geometry)
        , palette(other->palette)
        , localTransform(other->localTransform)
        , translate(other->translate)
        , localEffect(other->localEffect)
        , customEffect(other->customEffect)
        , hasEffect(other->hasEffect)
        , material(other->material)
        , backMaterial(other->backMaterial)
        , start(other->start)
        , count(other->count)
        , options(other->options)
        , pickNode(0)   // Explicitly not cloned.
        , bb(other->bb)
        , boxValid(other->boxValid)
        , drawingMode(other->drawingMode)
        , drawingWidth(1.0)
        , culled(other->culled)
    {
    }

    inline void invalidateParentBoundingBox() const
    {
        QList<QGLSceneNode*>::const_iterator it = parentNodes.constBegin();
        for ( ; it != parentNodes.constEnd(); ++it)
            (*it)->invalidateBoundingBox();
    }

    QGeometryData geometry;
    QGLMaterialCollection *palette;
    QMatrix4x4 localTransform;
    QVector3D translate;
    QList<QGraphicsTransform3D *> transforms;
    QGL::StandardEffect localEffect;
    QGLAbstractEffect *customEffect;
    QList<QGLSceneNode*> childNodes;
    QList<QGLSceneNode*> parentNodes;
    bool hasEffect;
    int material;
    int backMaterial;
    int start;
    int count;
    QGLSceneNode::Options options;
    QGLPickNode *pickNode;
    mutable QBox3D bb;
    mutable bool boxValid;
    QGL::DrawingMode drawingMode;
    qreal drawingWidth;
    bool culled;
};

#endif // QGLSCENENODE_P_H
