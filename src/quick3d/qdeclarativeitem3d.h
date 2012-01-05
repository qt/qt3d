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

#ifndef QDECLARATIVEITEM3D_H
#define QDECLARATIVEITEM3D_H

#include "qt3dquickglobal.h"

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector3d.h>

#include <QtQuick/qquickitem.h>

#include "qglscenenode.h"
#include "qglpainter.h"
#include "qgraphicstransform3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDeclarativeItem3DPrivate;
class QDeclarativeMesh;
class QDeclarativeEffect;
class QDeclarativeViewport;

class Q_QT3D_QUICK_EXPORT QDeclarativeItem3D : public QQuickItem
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(SortMode)
    Q_ENUMS(CullFace)
    Q_FLAGS(CullFaces)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY positionChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsTransform3D> transform READ transform DESIGNABLE false FINAL)
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsTransform3D> pretransform READ pretransform DESIGNABLE false FINAL)
    Q_PROPERTY(QDeclarativeMesh *mesh READ mesh WRITE setMesh NOTIFY meshChanged)
    Q_PROPERTY(QDeclarativeEffect *effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(QGLLightParameters *light READ light WRITE setLight NOTIFY lightChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> resources READ resources DESIGNABLE false)
    Q_PROPERTY(QDeclarativeListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(CullFaces cullFaces READ cullFaces WRITE setCullFaces NOTIFY meshChanged)
    Q_PROPERTY(SortMode sortChildren READ sortChildren WRITE setSortChildren NOTIFY sortChildrenChanged)
    Q_PROPERTY(QString meshNode READ meshNode WRITE setMeshNode NOTIFY meshNodeChanged)
    Q_PROPERTY(bool inheritEvents READ inheritEvents WRITE setInheritEvents NOTIFY inheritEventsChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    QDeclarativeItem3D(QObject *parent = 0);
    ~QDeclarativeItem3D();

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

    qreal x() const;
    void setX(qreal value);
    qreal y() const;
    void setY(qreal value);
    qreal z() const;
    void setZ(qreal value);

    qreal scale() const;
    void setScale(qreal value);

    QDeclarativeMesh *mesh() const;
    void setMesh(QDeclarativeMesh* value);

    bool inheritEvents() const;
    void setInheritEvents(bool inherit);

    QDeclarativeEffect *effect() const;
    void setEffect(QDeclarativeEffect *value);

    QGLLightParameters *light() const;
    void setLight(QGLLightParameters *value);

    QDeclarativeListProperty<QObject> data();
    QDeclarativeListProperty<QObject> resources();

    QDeclarativeListProperty<QGraphicsTransform3D> transform();
    QDeclarativeListProperty<QGraphicsTransform3D> pretransform();

    CullFaces cullFaces() const;
    void setCullFaces(CullFaces value);

    SortMode sortChildren() const;
    void setSortChildren(SortMode mode);

    QString meshNode() const;
    void setMeshNode(const QString &);

    bool isEnabled() const;
    void setEnabled(bool value);

    virtual void draw(QGLPainter *painter);
    virtual void initialize(QGLPainter *painter);
    bool isInitialized() const;

    Q_INVOKABLE QVector3D localToWorld(const QVector3D &point = QVector3D()) const;
    Q_INVOKABLE QVector3D worldToLocal(const QVector3D &point = QVector3D()) const;

    void componentComplete();

    int objectPickId() const;

public Q_SLOTS:
    void update();

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

Q_SIGNALS:
    void positionChanged();
    void scaleChanged();
    void rotationChanged();
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

private:
    QDeclarativeItem3DPrivate *d;

    friend class QDeclarativeItem3DPrivate;
    friend class QDeclarativeViewport;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeItem3D::CullFaces)

Q_QT3D_QUICK_EXPORT void qDumpItem(QDeclarativeItem3D *item, bool detailed = true, int indent = 0);

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeItem3D)

QT_END_HEADER

#endif
