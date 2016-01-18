/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "scene3dsgmaterial_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DCore::Scene3DSGMaterial
    \internal
    \inherit QSGMaterial

    \brief The Qt3DRender::Scene3DSGMaterial class is a custom QSGMaterial subclass used to
    render a Qt3DRender::Scene3DSGNode

    The Qt3DRender::Scene3DSGMaterial class renders a texture using premultiplied
    alpha unlike the QSGSimpleTextureMaterial.

    This is needed to properly integrate alpha blending from a Qt3D scene
    within a QtQuick 2 scene.
 */
Scene3DSGMaterial::Scene3DSGMaterial()
    : QSGMaterial()
    , m_texture(Q_NULLPTR)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE
