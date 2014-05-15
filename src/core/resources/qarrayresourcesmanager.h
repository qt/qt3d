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

#ifndef QT3D_QARRAYRESOURCESMANAGER_H
#define QT3D_QARRAYRESOURCESMANAGER_H

#include <QtGlobal>
#include <QVector>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qabstractresourcesmanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template <typename T, typename C, int INDEXBITS = 16>
class QT3DCORESHARED_EXPORT QArrayResourcesManager : public QAbstractResourcesManager<T, C, INDEXBITS>
{
public:

    QArrayResourcesManager() :
        QAbstractResourcesManager<T, C, INDEXBITS>(),
        m_resourceEntries(1 << INDEXBITS)
    {
        reset();
    }

    ~QArrayResourcesManager()
    {
        m_resourceEntries.clear();
        m_handleToResource.clear();
        m_freeEntryIndices.clear();
    }

    QHandle<T, INDEXBITS> acquire() Q_DECL_OVERRIDE
    {
        Q_ASSERT(!m_freeEntryIndices.isEmpty());
        int idx = m_freeEntryIndices.takeFirst();
        m_resourceEntries[idx] = T();
        QHandle<T, INDEXBITS> handle = QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.acquire(m_resourceEntries.begin() + idx);
        m_handleToResource[handle.index()] = idx;
        return handle;
    }

    T *data(const QHandle<T, INDEXBITS> &handle) Q_DECL_OVERRIDE
    {
        return QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.data(handle);
    }

    void release(const QHandle<T, INDEXBITS> &handle) Q_DECL_OVERRIDE
    {
        Q_ASSERT(m_handleToResource.contains(handle.index()));
        m_freeEntryIndices << m_handleToResource[handle.index()];
        QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.release(handle);
        m_handleToResource.remove(handle.index());
        // Maybe trigger some kind of memory reordering and update
        // Handles and their pointer
    }

    void reset() Q_DECL_OVERRIDE
    {
        QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.reset();
        m_handleToResource.clear();
        for (int i = 0; i < m_resourceEntries.size(); i++)
            m_freeEntryIndices << i;
    }

protected:
    QVector<T> m_resourceEntries;
    QHash<int, int> m_handleToResource;
    QList<int> m_freeEntryIndices;
};

}// Qt3D

QT_END_NAMESPACE

#endif // QARRAYRESOURCESMANAGER_H
