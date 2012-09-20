/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QTEST_HELPERS_H
#define QTEST_HELPERS_H

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

#include <QtTest/QtTest>
#include <QtGui/qvector4d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>
#include "qcolor4ub.h"

QT_BEGIN_NAMESPACE

namespace QTest {
    char *toString(const QVector4D &v)
    {
        char *msg = new char[128];
        qsnprintf(msg, 128, "(%0.4f, %0.4f, %0.4f, %0.4f)",
                    v.x(), v.y(), v.z(), v.w());
        return msg;
    }

    char *toString(const QVector3D &v)
    {
        char *msg = new char[128];
        qsnprintf(msg, 128, "(%0.4f, %0.4f, %0.4f)", v.x(), v.y(), v.z());
        return msg;
    }

    char *toString(const QVector2D &v)
    {
        char *msg = new char[128];
        qsnprintf(msg, 128, "(%0.4f, %0.4f)", v.x(), v.y());
        return msg;
    }

    char *toString(const QColor4ub &c)
    {
        char *msg = new char[128];
        qsnprintf(msg, 128, "R: %0.2f, G: %0.2f, B: %0.2f, A: %0.2f)",
                    c.redF(), c.greenF(), c.blueF(), c.alphaF());
        return msg;
    }

    bool qCompare(const QVector4D &t1, const QVector4D &t2,
                  const char *actual, const char *expected,
                  const char *file, int line)
    {
        return compare_helper(t1 == t2, "Compared QVector4D values are not the same:",
                              toString(t1), toString(t2), actual, expected, file, line);
    }

    bool qCompare(const QVector3D &t1, const QVector3D &t2,
                  const char *actual, const char *expected,
                  const char *file, int line)
    {
        return compare_helper(t1 == t2, "Compared QVector3D values are not the same:",
                              toString(t1), toString(t2), actual, expected, file, line);
    }

    bool qCompare(const QVector2D &t1, const QVector2D &t2,
                  const char *actual, const char *expected,
                  const char *file, int line)
    {
        return compare_helper(t1 == t2, "Compared QVector2D values are not the same:",
                              toString(t1), toString(t2), actual, expected, file, line);
    }

    bool qCompare(const QColor4ub &t1, const QColor4ub &t2,
                  const char *actual, const char *expected,
                  const char *file, int line)
    {
        return compare_helper(t1 == t2, "Compared QColor4ub values are not the same:",
                              toString(t1), toString(t2), actual, expected, file, line);
    }

};

QT_END_NAMESPACE

#endif // QTEST_HELPERS_H
