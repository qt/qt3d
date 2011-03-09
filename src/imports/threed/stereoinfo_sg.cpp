/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "stereoinfo_sg.h"
#include <QtDeclarative/qsgcontext.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

QSGStereoInfo::QSGStereoInfo(QSGContext *context)
    : m_context(context)
    , m_hasStereo(false)
    , m_hasPreTransform(false)
{
    QVariant v = m_context->property("hasStereo");
    if (!v.isNull())
        m_hasStereo = v.toBool();

    v = m_context->property("hasPreTransform");
    if (!v.isNull())
        m_hasPreTransform = v.toBool();

    const QMetaObject *meta = m_context->metaObject();
    m_eyeProperty = meta->property(meta->indexOfProperty("eye"));
}

QSGStereoInfo::~QSGStereoInfo()
{
}

/*!
    \fn QSGContext *QSGStereoInfo::context() const

    Returns the scene graph rendering context that is being tracked
    by this information object.

    \internal
*/

/*!
    \fn bool QSGStereoInfo::hasStereo() const

    Returns true if context() supports stereo rendering; false otherwise.

    \sa eye()
    \internal
*/

/*!
    \fn bool QSGStereoInfo::hasPreTransform() const

    Returns true if context() supports the pre-multiplied transformation
    node type QSGPreTransformNode; false otherwise.  Pre-multiplied
    transformations are used to assist with implementing FloatingItem.

    \internal
*/

/*!
    Returns the current eye that is being rendered.  If context() does
    not support stereo, then QGL::NoEye is returned.  Otherwise
    QGL::LeftEye or QGL::RightEye is returned.

    This function is normally called from a scene graph node's preprocess()
    function to adjust the behavior of the node depending upon which
    eye is currently being rendered.

    \sa hasStereo()
    \internal
*/
QGL::Eye QSGStereoInfo::eye() const
{
    // The "eye" property is assumed to be of type "int" with values
    // 0, 1, and 2, which we cast to NoEye, LeftEye, and RightEye.
    QVariant v = m_eyeProperty.read(m_context);
    if (v.isNull())
        return QGL::NoEye;
    else
        return QGL::Eye(v.toInt());
}

QT_END_NAMESPACE
