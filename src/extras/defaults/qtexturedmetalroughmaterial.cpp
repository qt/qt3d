/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qtexturedmetalroughmaterial.h"

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

/*!
    \class Qt3DExtras::QTexturedMetalRoughMaterial
    \brief This class is deprecated; use QMetalRoughMaterial instead.
    \inmodule Qt3DExtras
    \since 5.9
    \inherits Qt3DExtras::QMetalRoughMaterial

    \deprecated
*/

/*!
    Constructs a new QTexturedMetalRoughMaterial instance with parent object \a parent.
*/
QTexturedMetalRoughMaterial::QTexturedMetalRoughMaterial(QNode *parent)
    : QMetalRoughMaterial(parent)
{
}

} // namespace Qt3DExtras

QT_END_NAMESPACE
