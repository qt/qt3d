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

#ifndef QGLBuilder_H
#define QGLBuilder_H

#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include <QtGui/qvector3d.h>
#include <QtOpenGL/qgl.h>

#include "qglnamespace.h"
#include "qglscenenode.h"
#include "qglattributevalue.h"
#include "qgeometrydata.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLSection;
class QGLMaterialCollection;
class QGLBuilderPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLBuilder
{
public:
    explicit QGLBuilder(QGLMaterialCollection *materials = 0);
    virtual ~QGLBuilder();

    // section management
    void newSection(QGL::Smoothing sm = QGL::Smooth);

    // scene management
    QGLSceneNode *sceneNode();
    QGLSceneNode *currentNode();
    QGLSceneNode *newNode();
    QGLSceneNode *pushNode();
    QGLSceneNode *popNode();
    QGLMaterialCollection *palette();
    QGLSceneNode *finalizedSceneNode();

    // geometry building by primitive
    void addTriangles(const QGeometryData &triangle);
    void addQuads(const QGeometryData &quad);
    void addTriangleFan(const QGeometryData &fan);
    void addTriangleStrip(const QGeometryData &strip);
    void addTriangulatedFace(const QGeometryData &face);
    void addQuadStrip(const QGeometryData &strip);
    void addQuadsInterleaved(const QGeometryData &top,
                        const QGeometryData &bottom);
    inline void addPane(qreal size = 1.0f);
    inline void addPane(QSizeF size);

protected:
    // internal and test functions
    QGLSection *currentSection() const;
    QList<QGLSection*> sections() const;
    void setDefaultThreshold(int);

private:
    Q_DISABLE_COPY(QGLBuilder);
    void addSection(QGLSection *section);

    friend class QGLSection;

    QGLBuilderPrivate *dptr;
};

inline void QGLBuilder::addPane(qreal size)
{
    addPane(QSizeF(size, size));
}

inline void QGLBuilder::addPane(QSizeF size)
{
    QSizeF f = size / 2.0f;
    QVector2D a(-f.width(), -f.height());
    QVector2D b(f.width(), -f.height());
    QVector2D c(f.width(), f.height());
    QVector2D d(-f.width(), f.height());
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    QVector2D td(0.0f, 1.0f);
    QGeometryData quad;
    quad.appendVertex(a, b, c, d);
    quad.appendTexCoord(ta, tb, tc, td);
    addQuads(quad);
}

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGL::Smoothing& smoothing);
Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGeometryData& triangles);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLBuilder_H
