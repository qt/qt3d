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

#ifndef QVECTOR_UTILS_P_H
#define QVECTOR_UTILS_P_H

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

#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>

// Replacement for qFuzzyCompare(QVector3D, QVector3D) and friends,
// for a specific case where the results are going to be rendered
// by the GPU onto a display.
//
// The accuracy of this comparison should not change.  Especially it
// should not change when you go from doubles to floats or when
// you get close to zero.  If two verts or lighting normals are
// the same to within 5 places of floating point accuracy then they
// will dislay as being on top of each other.
//
// Also this avoids doing 3 floating point multiplications every time
// since the normal qFuzzyIsNull does a mul to scale the epsilon when
// close to zero.

inline bool qFskIsNull(double d)
{
    return qAbs(d) <= 0.00001;
}

inline bool qFskIsNull(float f)
{
    return qAbs(f) <= 0.00001f;
}

inline bool qFskCompare(float a, float b)
{
    return qFskIsNull(a - b);
}

inline bool qFskCompare(double a, double b)
{
    return qFskIsNull(a - b);
}

inline bool qFskCompare(const QVector3D &a, const QVector3D &b)
{
    return (
            qFskIsNull(a.x() - b.x()) &&
            qFskIsNull(a.y() - b.y()) &&
            qFskIsNull(a.z() - b.z())
            );
}

inline bool qFskCompare(const QVector2D &a, const QVector2D &b)
{
    return (
            qFskIsNull(a.x() - b.x()) &&
            qFskIsNull(a.y() - b.y())
            );
}

#endif // QVECTOR_UTILS_P_H
