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

#ifndef QT3D_QNODEID_H
#define QT3D_QNODEID_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QT3DCORESHARED_EXPORT QNodeId
{
public:
    QNodeId()
        : m_uuid(0)
    {}

    static QNodeId createUuid();

    inline bool isNull() const
    {
        return m_uuid == 0;
    }

    inline bool operator ==(const QNodeId &other) const
    {
        return other.m_uuid == m_uuid;
    }

    inline bool operator !=(const QNodeId &other) const
    {
        return !operator ==(other);
    }

    inline bool operator <(const QNodeId &other) const
    {
        return m_uuid < other.m_uuid;
    }

    inline bool operator >(const QNodeId &other) const
    {
        return m_uuid > other.m_uuid;
    }

    inline quint64 guid() const
    {
        return m_uuid;
    }

private:
    quint64 m_uuid;
};


#ifndef QT_NO_DEBUG_STREAM
QT3DCORESHARED_EXPORT QDebug operator<<(QDebug d, const QNodeId &id);
#endif

QT3DCORESHARED_EXPORT inline uint qHash(const QNodeId &uuid, uint) Q_DECL_NOTHROW
{
    return uuid.guid();
}

} // Qt3D

Q_DECLARE_TYPEINFO(Qt3D::QNodeId, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::QNodeId)


#endif // QT3D_QNODEID_H
