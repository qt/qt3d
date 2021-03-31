/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qt3dquick3dcoreplugin.h"

#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DCore/qboundingvolume.h>
#include <QtCore/qvariantanimation.h>

#include <Qt3DQuick/private/quick3dentity_p.h>
#include <Qt3DQuick/private/quick3dentityloader_p.h>
#include <Qt3DQuick/private/quick3dnodeinstantiator_p.h>
#include <Qt3DQuick/private/quick3djoint_p.h>
#include <Qt3DQuick/private/qquaternionanimation_p.h>
#include <Qt3DQuick/private/qt3dquick_global_p.h>
#include <Qt3DQuick/private/quick3dbuffer_p.h>
#include <Qt3DQuick/private/quick3dgeometry_p.h>

#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DCorePlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri);
    Qt3DCore::Quick::Quick3D_initialize();
    Qt3DCore::Quick::Quick3D_registerType("QEntity", "Qt3D.Core/Entity", 2, 0);
    Qt3DCore::Quick::Quick3D_registerType("QGeometry", "Qt3D.Core/Geometry", 2, 0);
    qRegisterAnimationInterpolator<QQuaternion>(Qt3DCore::Quick::q_quaternionInterpolator);
}

Qt3DQuick3DCorePlugin::~Qt3DQuick3DCorePlugin()
{
    Qt3DCore::Quick::Quick3D_uninitialize();
}

QT_END_NAMESPACE
