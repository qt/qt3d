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

#ifndef QQUICKITEM3D_H
#define QQUICKITEM3D_H

#include <Qt3DQuick/qt3dquickglobal.h>
#include <Qt3DQuick/qquickanimation3d.h>
#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qgraphicstransform3d.h>

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector3d.h>

#include <QtQuick/qquickitem.h>

QT_BEGIN_NAMESPACE

class QQuickItem3DPrivate;
class QQuickMesh;
class QQuickEffect;
class QQuickViewport;

class Q_QT3D_QUICK_EXPORT QQuickItem3D : public QQuickItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_ENUMS(SortMode)
    Q_ENUMS(CullFace)
    Q_FLAGS(CullFaces)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY position3dChanged)
    Q_PROPERTY(float x READ x WRITE setX NOTIFY position3dChanged)
    Q_PROPERTY(float y READ y WRITE setY NOTIFY position3dChanged)
    Q_PROPERTY(float z READ z WRITE setZ NOTIFY position3dChanged)
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scale3dChanged)
    Q_PROPERTY(QQmlListProperty<QQuickQGraphicsTransform3D> transform READ transform DESIGNABLE false FINAL)
    Q_PROPERTY(QQmlListProperty<QQuickQGraphicsTransform3D> pretransform READ pretransform DESIGNABLE false FINAL)
    Q_PROPERTY(QQuickMesh *mesh READ mesh WRITE setMesh NOTIFY meshChanged)
    Q_PROPERTY(QQuickEffect *effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(QGLLightParameters *light READ light WRITE setLight NOTIFY lightChanged)
    Q_PROPERTY(QQmlListProperty<QObject> resources READ resources DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(CullFaces cullFaces READ cullFaces WRITE setCullFaces NOTIFY meshChanged)
    Q_PROPERTY(SortMode sortChildren READ sortChildren WRITE setSortChildren NOTIFY sortChildrenChanged)
    Q_PROPERTY(QString meshNode READ meshNode WRITE setMeshNode NOTIFY meshNodeChanged)
    Q_PROPERTY(bool inheritEvents READ inheritEvents WRITE setInheritEvents NOTIFY inheritEventsChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QQmlListProperty<QQuickAnimation3D> animations READ animations NOTIFY animationsChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    QQuickItem3D(QObject *parent = 0);
    ~QQuickItem3D();

    enum CullFace
    {
        CullDisabled                = 0,
        CullFrontFaces              = 0x0404, // GL_FRONT
        CullBackFaces               = 0x0405, // GL_BACK
        CullAllFaces                = 0x0408, // GL_FRONT_AND_BACK
        CullClockwise               = 0x10000
    };
    Q_DECLARE_FLAGS(CullFaces, CullFace)

    enum SortMode
    {
        DefaultSorting,
        BackToFront
    };

    QVector3D position() const;
    void setPosition(const QVector3D& value);

    float x() const;
    void setX(float value);
    float y() const;
    void setY(float value);
    float z() const;
    void setZ(float value);

    float scale() const;
    void setScale(float value);

    QQuickMesh *mesh() const;
    void setMesh(QQuickMesh* value);

    bool inheritEvents() const;
    void setInheritEvents(bool inherit);

    QQuickEffect *effect() const;
    void setEffect(QQuickEffect *value);

    QGLLightParameters *light() const;
    void setLight(QGLLightParameters *value);

    QQmlListProperty<QObject> data();
    QQmlListProperty<QObject> resources();

    QQmlListProperty<QQuickQGraphicsTransform3D> transform();
    QQmlListProperty<QQuickQGraphicsTransform3D> pretransform();

    CullFaces cullFaces() const;
    void setCullFaces(CullFaces value);

    SortMode sortChildren() const;
    void setSortChildren(SortMode mode);

    QString meshNode() const;
    void setMeshNode(const QString &);

    bool isEnabled() const;
    void setEnabled(bool value);

    QQmlListProperty<QQuickAnimation3D> animations();

    virtual void draw(QGLPainter *painter);
    virtual void initialize(QGLPainter *painter);
    bool isInitialized() const;

    Q_INVOKABLE QVector3D localToWorld(const QVector3D &point = QVector3D()) const;
    Q_INVOKABLE QVector3D worldToLocal(const QVector3D &point = QVector3D()) const;

    void componentComplete();

    int objectPickId() const;

public Q_SLOTS:
    void update();
    void updateAnimations();

protected:
    virtual void drawLightingSetup(QGLPainter *painter, const QGLLightParameters *currentLight, QMatrix4x4 &currentLightTransform);
    virtual void drawLightingCleanup(QGLPainter *painter, const QGLLightParameters *currentLight, QMatrix4x4 &currentLightTransform);

    virtual void drawCullSetup();
    virtual void drawCullCleanup();

    virtual void drawEffectSetup(QGLPainter *painter, bool &viewportBlend, bool &effectBlend);
    virtual void drawEffectCleanup(QGLPainter *painter, bool &viewportBlend, bool &effectBlend);

    virtual void drawChildren(QGLPainter *painter);
    virtual void drawItem(QGLPainter *painter);

    virtual void drawTransformSetup(QGLPainter *painter);
    virtual void drawTransformCleanup(QGLPainter *painter);

    bool event(QEvent *e);

private Q_SLOTS:
    void handleEffectChanged();
    void handleOpenglContextIsAboutToBeDestroyed();

Q_SIGNALS:
    void position3dChanged();
    void scale3dChanged();
    void meshChanged();
    void meshNodeChanged();
    void effectChanged();
    void lightChanged();
    void clicked();
    void doubleClicked();
    void pressed();
    void released();
    void hoverEnter();
    void hoverLeave();
    void inheritEventsChanged();
    void enabledChanged();
    void sortChildrenChanged();
    void animationsChanged();

private:
    QQuickItem3DPrivate *d;

    friend class QQuickItem3DPrivate;
    friend class QQuickViewport;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QQuickItem3D::CullFaces)

Q_QT3D_QUICK_EXPORT void qDumpItem(QQuickItem3D *item, bool detailed = true, int indent = 0);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickItem3D)

#endif
