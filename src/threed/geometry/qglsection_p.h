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

#ifndef QGLSECTION_H
#define QGLSECTION_H

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

#include "qglpainter.h"
#include "qlogicalvertex.h"
#include "qbox3d.h"
#include "qglnamespace.h"

#include <QMatrix4x4>

QT_BEGIN_NAMESPACE

class QGLPainter;
class QGLBuilder;
class QGLSectionPrivate;
class QGeometryData;
class QGLSceneNode;

class Q_QT3D_EXPORT QGLSection : public QGeometryData
{
public:
    QGLSection(QGLBuilder *d, QGL::Smoothing sm = QGL::Smooth);
    ~QGLSection();

    void reserve(int amount);

    void append(const QLogicalVertex &lv);
    void append(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c);
    void appendSmooth(const QLogicalVertex &lv);
    void appendSmooth(const QLogicalVertex &lv, int index);
    void appendSmooth(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
    {
        appendSmooth(a);
        appendSmooth(b);
        appendSmooth(c);
    }
    void appendFaceted(const QLogicalVertex &lv);
    void appendFaceted(const QLogicalVertex &a, const QLogicalVertex &b, const QLogicalVertex &c)
    {
        appendFaceted(a);
        appendFaceted(b);
        appendFaceted(c);
    }

    inline QGL::Smoothing smoothing() const;
    inline void setSmoothing(QGL::Smoothing s);
    int mapThreshold() const;
    void setMapThreshold(int);
    QList<QGLSceneNode*> nodes() const;
    void addNode(QGLSceneNode *node);
    bool deleteNode(QGLSceneNode *node);
private:
    Q_DISABLE_COPY(QGLSection);
    friend class QGLBuilder;

    int appendOne(const QLogicalVertex &vertex);

    QGL::Smoothing m_smoothing;
    QGLSectionPrivate *d;
};

inline QGL::Smoothing QGLSection::smoothing() const
{
    return m_smoothing;
}

inline void QGLSection::setSmoothing(QGL::Smoothing s)
{
    m_smoothing = s;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGLSection &section);
#endif

QT_END_NAMESPACE

#endif // QGLSECTION_H
