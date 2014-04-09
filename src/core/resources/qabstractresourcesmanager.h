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

#ifndef QT3D_QABSTRACTRESOURCESMANAGER_H
#define QT3D_QABSTRACTRESOURCESMANAGER_H

#include <QtGlobal>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qhandlemanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template <typename T, typename C, int INDEXBITS = 16>
class QT3DCORESHARED_EXPORT QAbstractResourcesManager
{
public:
    QAbstractResourcesManager() :
        m_maxResourcesEntries(1 << INDEXBITS)
    {
    }

    virtual ~QAbstractResourcesManager()
    {
    }

    virtual QHandle<T, INDEXBITS> acquire() = 0;
    virtual T* data(const QHandle<T, INDEXBITS> &handle) = 0;
    virtual void release(const QHandle<T, INDEXBITS> &handle) = 0;
    virtual void reset() = 0;

    QHandle<T, INDEXBITS> lookupHandle(const C &id)
    {
        if (m_handleToResourceMapper.contains(id))
            return m_handleToResourceMapper[id];
        return QHandle<T, INDEXBITS>();
    }

    T *lookupResource(const C &id)
    {
        if (m_handleToResourceMapper.contains(id))
            return data(m_handleToResourceMapper[id]);
        return Q_NULLPTR;
    }

    T *getOrCreate(const C &id)
    {
        if (!m_handleToResourceMapper.contains(id))
            m_handleToResourceMapper[id] = acquire();
        return data(m_handleToResourceMapper[id]);
    }


    int maxResourcesEntries() const { return m_maxResourcesEntries; }

protected:
    QHandleManager<T, INDEXBITS> m_handleManager;
    QHash<C, QHandle<T, INDEXBITS> > m_handleToResourceMapper;
    int m_maxResourcesEntries;
};

}// Qt3D

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTRESOURCESMANAGER_H
