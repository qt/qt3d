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

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqml.h>

#include "qquickitem3d.h"
#include "qquickmesh.h"
#include "qquickanimation3d.h"
#include "viewport.h"
#include "qquickeffect.h"
#include "scale3d.h"
#include "skybox.h"

#include "point.h"
#include "line.h"
#include "capsulemesh.h"
#include "cylindermesh.h"
#include "spheremesh.h"

#include "qgraphicsrotation3d.h"
#include "qgraphicstranslation3d.h"
#include "qgraphicsscale3d.h"
#include "qglscenenode.h"

#include "qgraphicslookattransform.h"
#include "shaderprogram.h"
#include "qt3dnamespace.h"

#include "billboarditem3d.h"

QML_DECLARE_TYPE(QQuickQGraphicsTransform3D)
QML_DECLARE_TYPE(QGraphicsRotation3D)
QML_DECLARE_TYPE(QGraphicsTranslation3D)
QML_DECLARE_TYPE(QGraphicsScale3D)
QML_DECLARE_TYPE(QGraphicsLookAtTransform)
QML_DECLARE_TYPE(QGLMaterial)
QML_DECLARE_TYPE(QGLLightModel)
QML_DECLARE_TYPE(QGLLightParameters)
QML_DECLARE_TYPE(QGLTexture2D)
QML_DECLARE_TYPE(QGLCamera)

QT_BEGIN_NAMESPACE

class QThreedQmlModule : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt3D"));
        qmlRegisterType<QGLSceneNode>(uri,2,0,"SceneNode");
        qmlRegisterType<QQuickEffect>(uri,2,0,"Effect");
        qmlRegisterType<QQuickMesh>(uri,2,0,"Mesh");
        qmlRegisterType<QQuickItem3D>(uri,2,0,"Item3D");
        qmlRegisterType<QQuickAnimation3D>(uri,2,0,"Animation3D");
        qmlRegisterType<QGLLightModel>(uri,2,0,"LightModel");
        qmlRegisterType<QGLLightParameters>(uri,2,0,"Light");
        qmlRegisterType<QGLCamera>(uri,2,0,"Camera");
        qmlRegisterType<QGraphicsRotation3D>(uri,2,0,"Rotation3D");
        qmlRegisterType<QGraphicsTranslation3D>(uri,2,0,"Translation3D");
        qmlRegisterType<Scale3D>(uri,2,0,"Scale3D");
        qmlRegisterType<QGraphicsLookAtTransform>(uri,2,0,"LookAt");
        qmlRegisterType<QGLMaterial>(uri,2,0,"Material");
        qmlRegisterType<QGLTexture2D>(uri,2,0,"Texture2D");
        qmlRegisterType<ShaderProgram>(uri,2,0,"ShaderProgram");
        qmlRegisterType<Skybox>(uri,2,0, "Skybox");
        qmlRegisterType<BillboardItem3D>(uri,2,0, "BillboardItem3D");

        qmlRegisterType<Point>(uri,2,0,"Point");
        qmlRegisterType<Line>(uri,2,0,"Line");
        qmlRegisterType<CapsuleMesh>(uri,2,0,"CapsuleMesh");
        qmlRegisterType<CylinderMesh>(uri,2,0,"CylinderMesh");
        qmlRegisterType<SphereMesh>(uri,2,0,"SphereMesh");

        qmlRegisterType<Viewport>(uri,2,0,"Viewport");

        // Needed to make QQmlListProperty<QQuickQGraphicsTransform3D> work.
        qmlRegisterType<QQuickQGraphicsTransform3D>();
        qmlRegisterType<QGraphicsScale3D>();
    }
    void initializeEngine(QQmlEngine *engine, const char *uri)
    {
        Q_UNUSED(uri);
        QQmlContext *context = engine->rootContext();
        context->setContextProperty(QLatin1String("Qt3D"), new Qt3DNamespace);
    }
};

QT_END_NAMESPACE

#include "threed.moc"

