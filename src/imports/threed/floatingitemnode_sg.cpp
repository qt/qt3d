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

#include "floatingitemnode_sg.h"
#include <QtDeclarative/qsgcontext.h>

QT_BEGIN_NAMESPACE

FloatingItemSGNode::FloatingItemSGNode(QSGContext *context)
    : m_stereoInfo(context), m_depth(0.0f)
{
    // Preprocessing is required only if the context supports pre-transforms.
    if (m_stereoInfo.hasPreTransform())
        setFlag(Node::UsePreprocess, true);
}

FloatingItemSGNode::~FloatingItemSGNode()
{
}

void FloatingItemSGNode::setDepth(qreal depth)
{
    m_depth = depth;
}

Node::NodeType FloatingItemSGNode::type() const
{
    // If the device-specific rendering context does not inherit
    // from QSGStereoContext, then it won't be aware of how to
    // update pre-transform nodes.  In that case, act like a
    // regular transform node and ignore the depth adjustment.
    if (m_stereoInfo.hasPreTransform())
        return PreTransformNodeType;
    else
        return TransformNodeType;
}

void FloatingItemSGNode::preprocess()
{
    QMatrix4x4 adjustMatrix;
    QGL::Eye eye = m_stereoInfo.eye();
    if (eye != QGL::NoEye && m_depth != 0.0f) {
        // Correct the depth value for the screen's DPI.  We treat 100 DPI
        // as "normal" and scale the depth value accordingly.  This way,
        // the same number of millimeters are used on all displays viewed
        // at the same viewing distance.  A depth of 1 is 0.254 millimeters.
        // Note: we should probably correct for viewing distance also,
        // but that is harder to determine.
        qreal depth = m_depth * m_stereoInfo.context()->glContext()->device()->logicalDpiX() / 100.0f;

        // Determine the transformation to pre-multiply with the modelview.
        if (eye == QGL::LeftEye)
            adjustMatrix.translate(depth / 2.0f, 0.0f, 0.0f);
        else
            adjustMatrix.translate(-depth / 2.0f, 0.0f, 0.0f);
    }
    setPreMatrix(adjustMatrix);
}

QT_END_NAMESPACE
