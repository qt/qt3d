
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

#ifndef QT3D_QLISTRESOURCESMANAGER_H
#define QT3D_QLISTRESOURCESMANAGER_H

#include <QtGlobal>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qhandlemanager.h>
#include <Qt3DCore/qabstractresourcesmanager.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template <typename T, typename C, int INDEXBITS = 16>
class QT3DCORESHARED_EXPORT QListResourcesManager : public QAbstractResourcesManager<T, C, INDEXBITS>
{

public:

    QListResourcesManager() :
        QAbstractResourcesManager<T, C, INDEXBITS>()
    {
    }

    ~QListResourcesManager()
    {
        m_resourceEntries.clear();
    }

    // QAbstractResourcesManager interface
    QHandle<T, INDEXBITS> acquire() Q_DECL_OVERRIDE;
    T *data(const QHandle<T, INDEXBITS> &handle) Q_DECL_OVERRIDE;
    void release(const QHandle<T, INDEXBITS> &handle) Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;

protected:
    QList<T> m_resourceEntries;
    QList<QHandle<T, INDEXBITS> >m_handleToResource;
};

template <typename T, typename C, int INDEXBITS>
QHandle<T, INDEXBITS> QListResourcesManager<T, C, INDEXBITS>::acquire()
{
    m_resourceEntries << T();
    QHandle<T, INDEXBITS> handle = QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.acquire(&m_resourceEntries.last());
    m_handleToResource << handle;
    return handle;
}

template <typename T, typename C, int INDEXBITS>
T *QListResourcesManager<T, C, INDEXBITS>::data(const QHandle<T, INDEXBITS> &handle)
{
    return QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.data(handle);
}

template <typename T, typename C, int INDEXBITS>
void QListResourcesManager<T, C, INDEXBITS>::release(const QHandle<T, INDEXBITS> &handle)
{
    int idx = m_handleToResource.indexOf(handle);
    QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.release(handle);
    m_resourceEntries.removeAt(idx);
    m_handleToResource.removeAt(idx);

}

template <typename T, typename C, int INDEXBITS>
void QListResourcesManager<T, C, INDEXBITS>::reset()
{
    QAbstractResourcesManager<T, C, INDEXBITS>::m_handleManager.reset();
    m_handleToResource.clear();
    m_resourceEntries.clear();
}

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QLISTRESOURCESMANAGER_H
