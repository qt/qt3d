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
#include <QReadWriteLock>
#include <QHash>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qhandlemanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

struct QT3DCORESHARED_EXPORT NonLockingPolicy
{
    void lockForRead() {}
    void lockForWrite() {}
    void unlock() {}
};

class QT3DCORESHARED_EXPORT LockingPolicy
{
public :

    void lockForRead()
    {
        m_lock.lockForRead();
    }

    void lockForWrite()
    {
        m_lock.lockForWrite();
    }

    void unlock()
    {
        m_lock.unlock();
    }

private:
    QReadWriteLock m_lock;
};

template <typename T, int INDEXBITS>
class ArrayAllocatingPolicy
{
public:
    ArrayAllocatingPolicy()
        : m_resourcesEntries(1 << INDEXBITS)
    {
        reset();
    }

    T* allocateResource()
    {
        int idx = m_freeEntryIndices.takeFirst();
        T* newT = m_resourcesEntries.begin() + idx;
        m_resourcesToIndices[newT] = idx;
        return newT;
    }

    void releaseResource(T *r)
    {
        if (m_resourcesToIndices.contains(r)) {
            int idx = m_resourcesToIndices.take(r);
            // THIS CAN CAUSE A BUG SOMEHOW
            // IF UNEXPECTED CRASHES HAPPEN, LOOK HERE
            // STILL WE NEED THIS TO HAVE A CLEAN ITEM SET ON RELEASE
            m_resourcesEntries[idx] = T();
            m_freeEntryIndices.append(idx);
        }
    }

    void reset()
    {
        m_resourcesToIndices.clear();
        m_resourcesEntries.clear();
        m_resourcesEntries.resize(1 << INDEXBITS);
        for (int i = 0; i < m_resourcesEntries.size(); i++)
            m_freeEntryIndices << i;
    }

private:
    QVector<T> m_resourcesEntries;
    QList<int> m_freeEntryIndices;
    QHash<T*, int> m_resourcesToIndices;

};

template <typename T, int INDEXBITS>
class ListAllocatingPolicy
{
public:
    ListAllocatingPolicy()
    {
    }

    T* allocateResource()
    {
        m_resourcesEntries << T();
        int idx = m_resourcesEntries.size() - 1;
        T* newT = &m_resourcesEntries.last();
        m_resourcesToIndices[newT] = idx;
        return newT;
    }

    void releaseResource(T *r)
    {
        if (m_resourcesToIndices.contains(r))
            m_resourcesEntries.removeAt(m_resourcesToIndices[r]);
    }

    void reset()
    {
        m_resourcesEntries.clear();
    }

private:
    QList<T> m_resourcesEntries;
    QHash<T*, int> m_resourcesToIndices;
};

template <typename T, typename C, int INDEXBITS = 16,
          template <typename, int> class AllocatingPolicy = ArrayAllocatingPolicy,
          class LockingPolicy = NonLockingPolicy
          >
class QAbstractResourcesManager
        : public AllocatingPolicy<T, INDEXBITS>
        , public LockingPolicy
{
public:
    QAbstractResourcesManager() :
        AllocatingPolicy<T, INDEXBITS>(),
        m_maxResourcesEntries(1 << INDEXBITS)
    {
    }

    ~QAbstractResourcesManager()
    {
    }

    QHandle<T, INDEXBITS> acquire()
    {
        LockingPolicy::lockForWrite();
        QHandle<T, INDEXBITS> handle = m_handleManager.acquire(AllocatingPolicy<T, INDEXBITS>::allocateResource());
        LockingPolicy::unlock();
        return handle;
    }

    T* data(const QHandle<T, INDEXBITS> &handle)
    {
        LockingPolicy::lockForRead();
        T* data = m_handleManager.data(handle);
        LockingPolicy::unlock();
        return data;
    }

    void release(const QHandle<T, INDEXBITS> &handle)
    {
        T *val = data(handle);
        LockingPolicy::lockForWrite();
        AllocatingPolicy<T, INDEXBITS>::releaseResource(val);
        m_handleManager.release(handle);
        LockingPolicy::unlock();
    }

    void reset()
    {
        LockingPolicy::lockForWrite();
        m_handleManager.reset();
        AllocatingPolicy<T, INDEXBITS>::reset();
        LockingPolicy::unlock();
    }

    bool contains(const C &id)
    {
        LockingPolicy::lockForRead();
        bool contained = m_handleToResourceMapper.contains(id);
        LockingPolicy::unlock();
        return contained;
    }

    QHandle<T, INDEXBITS> getOrAcquireHandle(const C &id)
    {
        if (!contains(id))
            m_handleToResourceMapper[id] = acquire();
        LockingPolicy::lockForRead();
        QHandle<T, INDEXBITS> handle = m_handleToResourceMapper[id];
        LockingPolicy::unlock();
        return handle;
    }

    QHandle<T, INDEXBITS> lookupHandle(const C &id)
    {
        LockingPolicy::lockForRead();
        QHandle<T, INDEXBITS> handle;
        if (m_handleToResourceMapper.contains(id))
            handle = m_handleToResourceMapper[id];
        LockingPolicy::unlock();
        return handle;
    }

    T *lookupResource(const C &id)
    {
        LockingPolicy::lockForRead();
        T *ret = Q_NULLPTR;
        if (m_handleToResourceMapper.contains(id))
            ret = data(m_handleToResourceMapper[id]);
        LockingPolicy::unlock();
        return ret;
    }

    T *getOrCreateResource(const C &id)
    {
        if (!contains(id))
            m_handleToResourceMapper[id] = acquire();
        T *ret = data(m_handleToResourceMapper[id]);
        return ret;
    }

    void releaseResource(const C &id)
    {
        if (m_handleToResourceMapper.contains(id))
            release(m_handleToResourceMapper[id]);
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
