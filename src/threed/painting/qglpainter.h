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

#ifndef QGLPAINTER_H
#define QGLPAINTER_H

#include <QOpenGLFunctions>

#include <Qt3D/qglnamespace.h>
#include <Qt3D/qbox3d.h>
#include <Qt3D/qglvertexbundle.h>
#include <Qt3D/qglindexbuffer.h>
#include <Qt3D/qgllightmodel.h>
#include <Qt3D/qgllightparameters.h>
#include <Qt3D/qglmaterial.h>
#include <Qt3D/qglabstractsurface.h>
#include <Qt3D/qmatrix4x4stack.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qvector2darray.h>
#include <Qt3D/qvector3darray.h>
#include <Qt3D/qvector4darray.h>

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QPainter>

QT_BEGIN_NAMESPACE

class QGLAbstractEffect;
class QGLPainterPrivate;
class QGLTexture2D;
class QGLTextureCube;
class QGeometryData;
class QOpenGLShaderProgram;
class QOpenGLFramebufferObject;
class QGLSceneNode;
class QGLRenderSequencer;
class QGLAbstractSurface;

class Q_QT3D_EXPORT QGLPainter : public QOpenGLFunctions
{
public:
    QGLPainter();
    explicit QGLPainter(QOpenGLContext *context);
    explicit QGLPainter(QWindow *widget);
    explicit QGLPainter(QPainter *painter);
    explicit QGLPainter(QGLAbstractSurface *surface);
    virtual ~QGLPainter();

    bool begin();
    bool begin(QOpenGLContext *context);
    bool begin(QWindow *window);
    bool begin(QPainter *painter);
    bool begin(QGLAbstractSurface *surface);
    bool end();
    bool isActive() const;

    QOpenGLContext *context() const;

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
    Q_DECLARE_FLAGS(Updates, Update)

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

    float aspectRatio() const;

    QGLAbstractEffect *effect() const;

    QGLAbstractEffect *userEffect() const;
    void setUserEffect(QGLAbstractEffect *effect);

    QGL::StandardEffect standardEffect() const;
    void setStandardEffect(QGL::StandardEffect effect);

    void disableEffect();

    QOpenGLShaderProgram *cachedProgram(const QString& name) const;
    void setCachedProgram(const QString& name, QOpenGLShaderProgram *program);

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

    bool begin(QOpenGLContext *context, QGLAbstractSurface *surface,
               bool destroySurface = true);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLPainter::Updates)

QT_END_NAMESPACE

#endif
