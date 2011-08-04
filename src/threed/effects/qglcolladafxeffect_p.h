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

#include <QString>
#include <QMap>
#include <QColor>
#include "qgltexture2d.h"
#include "qglcolladafxeffect.h"

class QGLPainter;
class QGLShaderProgram;
class QGLMaterial;

class QGLColladaFxEffectPrivate
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaFxEffectPrivate();
    ~QGLColladaFxEffectPrivate();

    void addMaterialChannelsToShaderSnippets(const QGLMaterial *material);
    void resetGlueSnippets();
    void setTextureUniform(QGLShaderProgram *program, QGLPainter* painter, QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor);
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

#endif // QGLCOLLADAFXEFFECT_P_H
