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

#ifndef BARREL_H
#define BARREL_H

#include <Qt3DRenderer/QNormalDiffuseSpecularMapMaterial>
#include <Qt3DRenderer/QTexture>
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
    Qt3D::QTexture *m_diffuseTexture;
    Qt3D::QTexture *m_normalTexture;
    Qt3D::QTexture *m_specularTexture;

    void setNormalTextureSource();
    void setDiffuseTextureSource();
    void setSpecularTextureSource();

};

#endif // BARREL_H
