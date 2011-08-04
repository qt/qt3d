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

#ifndef QGLDOME_H
#define QGLDOME_H

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLBuilder;

class Q_QT3D_EXPORT QGLDome
{
public:
    explicit QGLDome(qreal diameter = 1.0f, int depth = 3, bool baseEnabled = true)
        : m_diameter(diameter), m_subdivisionDepth(depth), m_baseEnabled(baseEnabled) {}
    virtual ~QGLDome();

    qreal diameter() const { return m_diameter; }
    void setDiameter(qreal diameter) { m_diameter = diameter; }

    int subdivisionDepth() const { return m_subdivisionDepth; }
    void setSubdivisionDepth(int depth) { m_subdivisionDepth = depth; }

    bool baseEnabled() const {return m_baseEnabled; }
    void setBaseEnabled(bool baseEnabled) {m_baseEnabled = baseEnabled;}

private:
    qreal m_diameter;
    int m_subdivisionDepth;
    bool m_baseEnabled;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLDome& dome);

QT_END_NAMESPACE

QT_END_HEADER

#endif
