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

#ifndef QT3D_RESOURCESMANAGER_H
#define QT3D_RESOURCESMANAGER_H

#include <QtGlobal>
#include <QVector>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qhandlemanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class Node;

template <typename T, int INDEXBITS = 16>
class QT3DCORESHARED_EXPORT QResourcesManager
{
public:
    QResourcesManager() :
        m_handleManager(new QHandleManager<T, INDEXBITS>()),
        m_resourceEntries(1 << INDEXBITS)
    {
        reset();
    }

    ~QResourcesManager()
    {
        delete m_handleManager;
        m_resourceEntries.clear();
    }

    QHandle<T> acquire();
    T* data(const QHandle<T> &handle);
    void release(const QHandle<T> &handle);
    void reset();

private:
    int nextFreeEntry();

    QHandleManager<T> *m_handleManager;
    QVector<T> m_resourceEntries;
    QHash<int, int> m_handleToResource;
    QList<int> m_freeEntryIndices;
};

template <typename T, int INDEXBITS>
void QResourcesManager<T, INDEXBITS>::release(const QHandle<T> &handle)
{
    Q_ASSERT(m_handleToResource.contains(handle.index()));
    m_freeEntryIndices << m_handleToResource[handle.index()];
    m_handleManager->release(handle);
    m_handleToResource.remove(handle.index());
    // Maybe trigger some kind of memory reordering and update
    // Handles and their pointer
}

template <typename T, int INDEXBITS>
void QResourcesManager<T, INDEXBITS>::reset()
{
    m_handleManager->reset();
    m_handleToResource.clear();
    for (int i = 0; i < m_resourceEntries.size(); i++)
        m_freeEntryIndices << i;
}

template <typename T, int INDEXBITS>
T* QResourcesManager<T, INDEXBITS>::data(const QHandle<T> &handle)
{
    return m_handleManager->data(handle);
}

template <typename T, int INDEXBITS>
QHandle<T> QResourcesManager<T, INDEXBITS>::acquire()
{
    Q_ASSERT(!m_freeEntryIndices.isEmpty());
    int idx = m_freeEntryIndices.takeFirst();
    m_resourceEntries[idx] = T();
    QHandle<T> handle = m_handleManager->acquire(m_resourceEntries.begin() + idx);
    m_handleToResource[handle.index()] = idx;
    return handle;
}

}// Qt3D

QT_END_NAMESPACE

#endif // QT3D_RESOURCESMANAGER_H
