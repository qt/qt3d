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


#ifndef QT3D_CAMERALENS_P_H
#define QT3D_CAMERALENS_P_H

#include <QMatrix4x4>

namespace Qt3D {

class CameraLensPrivate
{
public:
    CameraLensPrivate( CameraLens *qq ) :
        q_ptr(qq)
      , m_projectionType( CameraLens::OrthogonalProjection )
      , m_nearPlane( 0.1f )
      , m_farPlane( 1024.0f )
      , m_fieldOfView( 25.0f )
      , m_aspectRatio( 1.0f )
      , m_left( -0.5 )
      , m_right( 0.5f )
      , m_bottom( -0.5f )
      , m_top( 0.5f )
      , m_syncNeeded( false )
    {
        updateOrthogonalProjection();
    }

    ~CameraLensPrivate()
    {
    }

    inline void updatePerpectiveProjection()
    {
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.perspective( m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane );
        m_syncNeeded = true;
    }

    inline void updateOrthogonalProjection()
    {
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.ortho( m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane );
        m_syncNeeded = true;
    }


    Q_DECLARE_PUBLIC( CameraLens )
    CameraLens *q_ptr;

    CameraLens::ProjectionType m_projectionType;

    float m_nearPlane;
    float m_farPlane;

    float m_fieldOfView;
    float m_aspectRatio;

    float m_left;
    float m_right;
    float m_bottom;
    float m_top;

    mutable QMatrix4x4 m_projectionMatrix;
    mutable bool m_syncNeeded;
};

}

#endif // QT3D_CAMERALENS_P_H
