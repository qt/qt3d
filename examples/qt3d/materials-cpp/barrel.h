/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BARREL_H
#define BARREL_H

#include <Qt3DRenderer/QNormalDiffuseSpecularMapMaterial>
#include <Qt3DRenderer/qtexture.h>
#include "renderableentity.h"

class Barrel : public RenderableEntity
{
public:
    Barrel(Qt3D::QNode *parent = 0);
    ~Barrel();

    enum DiffuseColor {
        Red = 0,
        Blue,
        Green,
        RustDiffuse,
        StainlessSteelDiffuse
    };

    enum SpecularColor {
        RustSpecular = 0,
        StainlessSteelSpecular,
        None
    };

    enum Bumps {
        NoBumps = 0,
        SoftBumps,
        MiddleBumps,
        HardBumps
    };

    void setDiffuse(DiffuseColor diffuse);
    void setSpecular(SpecularColor specular);
    void setBumps(Bumps bumps);
    void setShininess(float shininess);

    DiffuseColor diffuse() const;
    SpecularColor specular() const;
    Bumps bumps() const;
    float shininess() const;

private:
    Bumps m_bumps;
    DiffuseColor m_diffuseColor;
    SpecularColor m_specularColor;
    Qt3D::QNormalDiffuseSpecularMapMaterial *m_material;
    Qt3D::QAbstractTextureProvider *m_diffuseTexture;
    Qt3D::QAbstractTextureProvider *m_normalTexture;
    Qt3D::QAbstractTextureProvider *m_specularTexture;
    Qt3D::QTextureImage *m_diffuseTextureImage;
    Qt3D::QTextureImage *m_normalTextureImage;
    Qt3D::QTextureImage *m_specularTextureImage;

    void setNormalTextureSource();
    void setDiffuseTextureSource();
    void setSpecularTextureSource();

};

#endif // BARREL_H
