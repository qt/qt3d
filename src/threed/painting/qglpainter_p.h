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

#ifndef QGLPAINTER_P_H
#define QGLPAINTER_P_H

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

#include "qglpainter.h"
#include "qglrendersequencer.h"

#include <QtCore/qatomic.h>
#include <QtCore/qmap.h>
#include <QtCore/qstack.h>

QT_BEGIN_NAMESPACE

#define QGL_MAX_LIGHTS      32
#define QGL_MAX_STD_EFFECTS 16

class QGLPainterPickPrivate
{
public:
    QGLPainterPickPrivate();
    ~QGLPainterPickPrivate();

    bool isPicking;
    int objectPickId;
    int pickColorIndex;
    QRgb pickColor;
    QMap<int, QRgb> pickObjectToColor;
    QMap<QRgb, int> pickColorToObject;
    QGLAbstractEffect *defaultPickEffect;
};

struct QGLPainterSurfaceInfo
{
    QGLAbstractSurface *surface;
    bool destroySurface;
    bool mainSurface;
};

class QGLPainterPrivate
{
public:
    QGLPainterPrivate();
    ~QGLPainterPrivate();

    QAtomicInt ref;
    QAtomicInt badShaderCount;
    QOpenGLContext *context;
    QMatrix4x4Stack projectionMatrix;
    QMatrix4x4Stack modelViewMatrix;
    QMatrix4x4 inverseEyeMatrix;
    QGL::Eye eye;
    QGLAbstractEffect *effect;
    QGLAbstractEffect *userEffect;
    QGL::StandardEffect standardEffect;
    QGLAbstractEffect *stdeffects[QGL_MAX_STD_EFFECTS];
    const QGLLightModel *lightModel;
    QGLLightModel *defaultLightModel;
    QGLLightParameters *defaultLight;
    QArray<const QGLLightParameters *> lights;
    QArray<QMatrix4x4> lightTransforms;
    const QGLMaterial *frontMaterial;
    const QGLMaterial *backMaterial;
    QGLMaterial *defaultMaterial;
    QGLMaterial *frontColorMaterial;
    QGLMaterial *backColorMaterial;
    QBox3D viewingCube;
    QColor color;
    QGLPainter::Updates updates;
    QGLPainterPickPrivate *pick;
    QMap<QString, QOpenGLShaderProgram *> cachedPrograms;
    QStack<QGLPainterSurfaceInfo> surfaceStack;
    GLuint boundVertexBuffer;
    GLuint boundIndexBuffer;
    QGLRenderSequencer *renderSequencer;
    bool isFixedFunction;
    QGLAttributeSet attributeSet;

    inline void ensureEffect(QGLPainter *painter)
        { if (!effect) createEffect(painter); }
    void createEffect(QGLPainter *painter);
};

class QGLPainterPrivateCache : public QObject
{
    Q_OBJECT
public:
    QGLPainterPrivateCache();
    ~QGLPainterPrivateCache();

    QMap<const QOpenGLContext *, QGLPainterPrivate *> cache;

    QGLPainterPrivate *fromContext(QOpenGLContext *context);

    static QGLPainterPrivateCache *instance();

public Q_SLOTS:
    void contextDestroyed();

Q_SIGNALS:
    void destroyedContext(const QOpenGLContext *context);
};

QT_END_NAMESPACE

#endif
