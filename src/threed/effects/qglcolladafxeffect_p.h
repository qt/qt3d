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

#ifndef QGLCOLLADAFXEFFECT_P_H
#define QGLCOLLADAFXEFFECT_P_H

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

#include "qgltexture2d.h"
#include "qglcolladafxeffect.h"

#include <QString>
#include <QMap>
#include <QColor>

QT_BEGIN_NAMESPACE

class QGLPainter;
class QOpenGLShaderProgram;
class QGLMaterial;

class QGLColladaFxEffectPrivate
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaFxEffectPrivate();
    ~QGLColladaFxEffectPrivate();

    void addMaterialChannelsToShaderSnippets(const QGLMaterial *material);
    void resetGlueSnippets();
    void setTextureUniform(QOpenGLShaderProgram *program, QGLPainter* painter, QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor);
    void updateMaterialChannelSnippets(QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor);

    QString id;
    QString sid;
    QString name;

    // The spec allows for 3D textures as well, but for now only 2D is
    // supported
    QGLTexture2D* emissiveTexture;
    QGLTexture2D* ambientTexture;
    QGLTexture2D* diffuseTexture;
    QGLTexture2D* specularTexture;
    int lighting;
    QGLMaterial* material;

    QStringList vertexShaderCodeSnippets;
    QStringList vertexShaderDeclarationSnippets;
    QStringList vertexShaderVariableNames;

    QStringList fragmentShaderCodeSnippets;
    QStringList fragmentShaderDeclarationSnippets;
    QStringList fragmentShaderVariableNames;

    QString vertexShaderEndGlueSnippet;
    QString vertexShaderMainGlueSnippet;
    QString fragmentShaderEndGlueSnippet;
    QString fragmentShaderMainGlueSnippet;

    bool currentlyActive;
};

QT_END_NAMESPACE

#endif // QGLCOLLADAFXEFFECT_P_H
