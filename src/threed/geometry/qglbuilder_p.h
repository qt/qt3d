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

#ifndef QGLBuilder_P_H
#define QGLBuilder_P_H

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

#include "qglbuilder.h"

#include <QtCore/qmap.h>
#include <QPointer>

QT_BEGIN_NAMESPACE

class QGLBuilder;
class QGLSection;
class QGeometryData;

class QGLBuilderPrivate
{
public:
    QGLBuilderPrivate(QGLBuilder *parent);
    ~QGLBuilderPrivate();
    inline void setDirty(bool dirty = true);
    void addTriangle(int a, int b, int c, const QGeometryData &p, int &count);
    void adjustSectionNodes(QGLSection *sec, int offset, const QGeometryData &geom);
    int adjustNodeTree(QGLSceneNode *top, int offset, const QGeometryData &geom,
                       QList<QGLSceneNode*> &deleted);

    QList<QGLSection*> sections;
    QGLSection *currentSection;
    QList<QGLSceneNode*> nodeStack;
    QGLSceneNode *currentNode;
    QGLSceneNode *rootNode;
    int defThreshold;
    QGLBuilder *q;
};

QT_END_NAMESPACE

#endif // QGLBuilder_P_H
