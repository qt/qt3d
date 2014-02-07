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

#ifndef QGLSCENENODE_H
#define QGLSCENENODE_H

#include <QtCore/qobject.h>

#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglmaterialcollection.h>

QT_BEGIN_NAMESPACE

class QGLSceneNodePrivate;
class QGLAbstractEffect;
class QGLPickNode;
class QQuickQGraphicsTransform3D;

class Q_QT3D_EXPORT QGLSceneNode : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLSceneNode)
    Q_ENUMS(Option)
    Q_FLAGS(Options)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY updated)
    Q_PROPERTY(float x READ x WRITE setX NOTIFY updated)
    Q_PROPERTY(float y READ y WRITE setY NOTIFY updated)
    Q_PROPERTY(float z READ z WRITE setZ NOTIFY updated)
    Q_PROPERTY(QGLMaterial *material READ material WRITE setMaterial NOTIFY updated)
    Q_PROPERTY(QGLMaterial *backMaterial READ backMaterial WRITE setBackMaterial NOTIFY updated)
    Q_PROPERTY(Options options READ options WRITE setOptions NOTIFY updated)
public:
    explicit QGLSceneNode(QObject *parent = 0);
    explicit QGLSceneNode(const QGeometryData &geometry, QObject *parent = 0);
    virtual ~QGLSceneNode();

    enum Option
    {
        NoOptions       = 0x0000,
        CullBoundingBox = 0x0001,
        ViewNormals     = 0x0002,
        ReportCulling   = 0x0004,
        HideNode        = 0x0008
    };
    Q_DECLARE_FLAGS(Options, Option)

    QGLSceneNode::Options options() const;
    void setOptions(QGLSceneNode::Options options);
    void setOption(QGLSceneNode::Option option, bool value);

    QGeometryData geometry() const;
    void setGeometry(QGeometryData);

    QBox3D boundingBox() const;

    QMatrix4x4 localTransform() const;
    void setLocalTransform(const QMatrix4x4 &);
    QVector3D position() const;
    void setPosition(const QVector3D &p);
    float x() const;
    void setX(float x);
    float y() const;
    void setY(float y);
    float z() const;
    void setZ(float z);

    QList<QQuickQGraphicsTransform3D *> transforms() const;
    void setTransforms(const QList<QQuickQGraphicsTransform3D *> &transforms);
    void addTransform(QQuickQGraphicsTransform3D *transform);

    QGL::DrawingMode drawingMode() const;
    void setDrawingMode(QGL::DrawingMode mode);

    float drawingWidth() const;
    void setDrawingWidth(float width);

    QGL::StandardEffect effect() const;
    void setEffect(QGL::StandardEffect);

    QGLAbstractEffect *userEffect() const;
    void setUserEffect(QGLAbstractEffect *effect);

    bool hasEffect() const;
    void setEffectEnabled(bool);

    int start() const;
    void setStart(int start);

    int count() const;
    void setCount(int count);

    int materialIndex() const;
    void setMaterialIndex(int material);
    int backMaterialIndex() const;
    void setBackMaterialIndex(int material);

    QGLMaterial *material() const;
    void setMaterial(QGLMaterial *material);
    QGLMaterial *backMaterial() const;
    void setBackMaterial(QGLMaterial *material);

    QSharedPointer<QGLMaterialCollection> palette() const;
    void setPalette(QSharedPointer<QGLMaterialCollection> palette);

    QList<QGLSceneNode*> allChildren() const;
    QList<QGLSceneNode *> children() const;

    void addNode(QGLSceneNode *node);
    void addNodes(const QList<QGLSceneNode *> &nodes);
    void removeNode(QGLSceneNode *node);
    void removeNodes(const QList<QGLSceneNode *> &nodes);

    QGLSceneNode *findSceneNode(QString &nodePath);
    QGLSceneNode *findSceneNode(QStringList &nodePath);

    virtual void draw(QGLPainter *painter);

    QGLPickNode *pickNode() const;
    void setPickNode(QGLPickNode *node);

    Q_INVOKABLE QGLSceneNode *get(const QString &name, QObject *parent = 0, bool forceCopy = false) ;
    Q_INVOKABLE QGLSceneNode *get(const QStringList &names, QObject *parent = 0, bool forceCopy = false) ;
    Q_INVOKABLE QGLSceneNode *except(const QString &name, QObject *parent = 0) ;
    Q_INVOKABLE QGLSceneNode *except(const QStringList &names, QObject *parent = 0) ;

    Q_INVOKABLE QGLSceneNode *clone(QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *cloneWithChildren(QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *cloneNoChildren(QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *allExcept(const QString &name, QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *only(const QString &name, QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *allExcept(const QStringList &names, QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *only(const QStringList &names, QObject *parent = 0) const;

Q_SIGNALS:
    void updated();
    void culled();
    void displayed();

protected:
    virtual void drawGeometry(QGLPainter *painter);

private Q_SLOTS:
    void transformChanged();

private:
    QMatrix4x4 transform() const;
    void invalidateBoundingBox() const;
    void invalidateTransform() const;
    void drawNormalIndicators(QGLPainter *painter);
    const QGLMaterial *setPainterMaterial(int material, QGLPainter *painter,
                                    QGL::Face faces, bool &changedTex);

    Q_DISABLE_COPY(QGLSceneNode)

    QScopedPointer<QGLSceneNodePrivate> d_ptr;

    QGLSceneNode(QGLSceneNodePrivate *d, QObject *parent);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLSceneNode::Options)

Q_QT3D_EXPORT void qDumpScene(QGLSceneNode *, bool detailed = true, int indent = 0, const QSet<QGLSceneNode*> &loop = QSet<QGLSceneNode*>());

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLSceneNode &node);
#endif

QT_END_NAMESPACE

#endif
