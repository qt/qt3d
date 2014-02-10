/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include <window.h>

#include <camera.h>
#include <cameraselector.h>
#include <effect.h>
#include <entity.h>
#include <material.h>
#include <mesh.h>
#include <node.h>
#include <renderpass.h>
#include <renderpassfilter.h>
#include <shaderprogram.h>
#include <technique.h>
#include <techniquefilter.h>
#include <transform.h>
#include <viewport.h>
#include <scene.h>

#include <gltfparser.h>

#include <matrixtransform.h>
#include <rotatetransform.h>
#include <translatetransform.h>
#include <lookattransform.h>

#include <QGuiApplication>
#include <QtQml>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Qt3D::Node>("Qt3D", 2, 0, "Node");
    qmlRegisterType<Qt3D::Entity>("Qt3D", 2, 0, "Entity");
    qmlRegisterType<Qt3D::Scene>("Qt3D", 2, 0, "Scene");
    qmlRegisterType<Qt3D::Camera>("Qt3D", 2, 0, "Camera");
    qmlRegisterType<Qt3D::Transform>("Qt3D", 2, 0, "Transform");
    qmlRegisterType<Qt3D::Effect>("Qt3D", 2, 0, "Effect");
    qmlRegisterType<Qt3D::Technique>("Qt3D", 2, 0, "Technique");
    qmlRegisterType<Qt3D::Tag>("Qt3D", 2, 0, "Tag");
    qmlRegisterType<Qt3D::Mesh>("Qt3D", 2, 0, "Mesh");
    qmlRegisterType<Qt3D::Material>("Qt3D", 2, 0, "Material");
    qmlRegisterType<Qt3D::RenderPass>("Qt3D", 2, 0, "RenderPass");
    qmlRegisterType<Qt3D::ShaderProgram>("Qt3D", 2, 0, "ShaderProgram");
    qmlRegisterType<Qt3D::MatrixTransform>("Qt3D", 2, 0, "MatrixTransform");
    qmlRegisterType<Qt3D::TranslateTransform>("Qt3D", 2, 0, "Translate");
    qmlRegisterType<Qt3D::RotateTransform>("Qt3D", 2, 0, "Rotate");
    qmlRegisterType<Qt3D::LookAtTransform>("Qt3D", 2, 0, "LookAt");

    // Framegraph components - TODO RenderTarget, RenderTargetSelector
    qmlRegisterType<Qt3D::Viewport>("Qt3D", 2, 0, "Viewport");
    qmlRegisterType<Qt3D::CameraSelector>("Qt3D", 2, 0, "CameraSelector");
    qmlRegisterType<Qt3D::RenderPassFilter>("Qt3D", 2, 0, "RenderPassFilter");
    qmlRegisterType<Qt3D::TechniqueFilter>("Qt3D", 2, 0, "TechniqueFilter");

    Qt3D::Window view;
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
