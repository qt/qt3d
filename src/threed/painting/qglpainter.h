/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef QGLPAINTER_H
#define QGLPAINTER_H

#include <QtOpenGL/qgl.h>
#include "qglnamespace.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qmatrix4x4.h>
#include "qbox3d.h"
#include "qopenglfunctions.h"
#include "qglvertexbundle.h"
#include "qglindexbuffer.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglmaterial.h"
#include "qglabstractsurface.h"
#include "qmatrix4x4stack.h"
#include "qglcamera.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qvector4darray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLAbstractEffect;
class QGLPainterPrivate;
class QGLTexture2D;
class QGLTextureCube;
class QGeometryData;
class QGLShaderProgram;
class QGLFramebufferObject;
class QGLSceneNode;
class QGLRenderSequencer;
class QGLAbstractSurface;

class Q_QT3D_EXPORT QGLPainter : public QOpenGLFunctions
{
public:
    QGLPainter();
    explicit QGLPainter(const QGLContext *context);
    explicit QGLPainter(QGLWidget *widget);
    explicit QGLPainter(QPainter *painter);
    explicit QGLPainter(QGLAbstractSurface *surface);
    virtual ~QGLPainter();

    bool begin();
    bool begin(const QGLContext *context);
    bool begin(QGLWidget *widget);
    bool begin(QPainter *painter);
    bool begin(QGLAbstractSurface *surface);
    bool end();
    bool isActive() const;

    const QGLContext *context() const;

    bool isFixedFunction() const;

    enum Update
    {
        UpdateColor                 = 0x00000001,
        UpdateModelViewMatrix       = 0x00000002,
        UpdateProjectionMatrix      = 0x00000004,
        UpdateMatrices              = 0x00000006,
        UpdateLights                = 0x00000008,
        UpdateMaterials             = 0x00000010,
        UpdateViewport              = 0x00000020,
        UpdateAll                   = 0x7FFFFFFF
    };
    Q_DECLARE_FLAGS(Updates, Update);

    void setClearColor(const QColor& color);

    void setScissor(const QRect& rect);

    QMatrix4x4Stack& projectionMatrix();
    QMatrix4x4Stack& modelViewMatrix();
    QMatrix4x4 combinedMatrix() const;
    QMatrix3x3 normalMatrix() const;
    QMatrix4x4 worldMatrix() const;

    QGL::Eye eye() const;
    void setEye(QGL::Eye eye);

    void setCamera(const QGLCamera *camera);

    bool isCullable(const QVector3D& point) const;
    bool isCullable(const QBox3D& box) const;
    QGLRenderSequencer *renderSequencer();

    qreal aspectRatio() const;

    QGLAbstractEffect *effect() const;

    QGLAbstractEffect *userEffect() const;
    void setUserEffect(QGLAbstractEffect *effect);

    QGL::StandardEffect standardEffect() const;
    void setStandardEffect(QGL::StandardEffect effect);

    void disableEffect();

    QGLShaderProgram *cachedProgram(const QString& name) const;
    void setCachedProgram(const QString& name, QGLShaderProgram *program);

    QColor color() const;
    void setColor(const QColor& color);

    QGLAttributeSet attributes() const;
    void clearAttributes();

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);
    void setVertexBundle(const QGLVertexBundle& buffer);

    void update();
    void updateFixedFunction(QGLPainter::Updates updates);

    void draw(QGL::DrawingMode mode, int count, int index = 0);
    void draw(QGL::DrawingMode mode, const ushort *indices, int count);
    void draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices);
    virtual void draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices, int offset, int count);

    void pushSurface(QGLAbstractSurface *surface);
    QGLAbstractSurface *popSurface();
    void setSurface(QGLAbstractSurface *surface);
    QGLAbstractSurface *currentSurface() const;

    const QGLLightModel *lightModel() const;
    void setLightModel(const QGLLightModel *value);

    const QGLLightParameters *mainLight() const;
    void setMainLight(const QGLLightParameters *parameters);
    void setMainLight
        (const QGLLightParameters *parameters, const QMatrix4x4& transform);
    QMatrix4x4 mainLightTransform() const;

    int addLight(const QGLLightParameters *parameters);
    int addLight(const QGLLightParameters *parameters, const QMatrix4x4 &transform);
    void removeLight(int lightId);

    int maximumLightId() const;
    const QGLLightParameters *light(int lightId) const;
    QMatrix4x4 lightTransform(int lightId) const;

    const QGLMaterial *faceMaterial(QGL::Face face) const;
    void setFaceMaterial(QGL::Face face, const QGLMaterial *value);
    void setFaceColor(QGL::Face face, const QColor& color);

    bool isPicking() const;
    void setPicking(bool value);

    int objectPickId() const;
    void setObjectPickId(int value);
    void clearPickObjects();

    QColor pickColor() const;

    int pickObject(int x, int y) const;

private:
    Q_DISABLE_COPY(QGLPainter)

    QGLPainterPrivate *d_ptr;

    QGLPainterPrivate *d_func() const { return d_ptr; }

    friend class QGLAbstractEffect;

    bool begin(const QGLContext *context, QGLAbstractSurface *surface,
               bool destroySurface = true);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLPainter::Updates)

QT_END_NAMESPACE

QT_END_HEADER

#endif
