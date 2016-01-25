/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire (paul.lemire350@gmail.com)
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qeventfilterservice_p.h"
#include "qabstractserviceprovider_p.h"
#include <QMap>
#include <QObject>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace {

struct FilterPriorityPair
{
    QObject *filter;
    int priority;
};

bool operator <(const FilterPriorityPair &a, const FilterPriorityPair &b)
{
    return a.priority < b.priority;
}

class InternalEventListener : public QObject
{
    Q_OBJECT
public:
    explicit InternalEventListener(QObject *parent = Q_NULLPTR)
        : QObject(parent)
    {
    }

    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_FINAL
    {
        for (int i = m_eventFilters.size() - 1; i >= 0; --i) {
            const FilterPriorityPair &fPPair = m_eventFilters.at(i);
            if (fPPair.filter->eventFilter(obj, e))
                return true;
        }
        return false;
    }

    void registerEventFilter(QObject *eventFilter, int priority)
    {
        for (int i = 0, m = m_eventFilters.size(); i < m; ++i)
            if (m_eventFilters.at(i).priority == priority)
                return;

        FilterPriorityPair fpPair;
        fpPair.filter = eventFilter;
        fpPair.priority = priority;
        m_eventFilters.push_back(fpPair);
        std::sort(m_eventFilters.begin(), m_eventFilters.end());
    }

    void unregisterEventFilter(QObject *eventFilter)
    {
        QVector<FilterPriorityPair>::iterator it = m_eventFilters.begin();
        const QVector<FilterPriorityPair>::iterator end = m_eventFilters.end();
        while (it != end) {
            if (it->filter == eventFilter) {
                m_eventFilters.erase(it);
                return;
            }
            ++it;
        }
    }

private:
    QVector<FilterPriorityPair> m_eventFilters;
};

} // anonymous

class QEventFilterServicePrivate : public QAbstractServiceProviderPrivate
{
public:
    QEventFilterServicePrivate()
        : QAbstractServiceProviderPrivate(QServiceLocator::EventFilterService, QStringLiteral("Default event filter service implementation"))
    {}

    QScopedPointer<InternalEventListener> m_eventDispatcher;
};


/* !\internal
    \class Qt3DCore::QEventFilterService
    \inmodule Qt3DCore

    \brief Allows to register event filters with a priority.

    The QEventFilterService class allows registering prioritized event filters.
    Events are dispatched to the event filter with the highest priority first,
    and then propagates to lower priority event filters if the event wasn't
    accepted.
 */

QEventFilterService::QEventFilterService()
    : QAbstractServiceProvider(*new QEventFilterServicePrivate())
{
}

QEventFilterService::~QEventFilterService()
{
}

// Note: event filters can only be registered to QObject in the same thread
void QEventFilterService::initialize(QObject *eventSource)
{
    Q_D(QEventFilterService);
    d->m_eventDispatcher.reset(new InternalEventListener());
    eventSource->installEventFilter(d->m_eventDispatcher.data());
}

void QEventFilterService::registerEventFilter(QObject *eventFilter, int priority)
{
    Q_D(QEventFilterService);
    if (!d->m_eventDispatcher)
        return;
    d->m_eventDispatcher->registerEventFilter(eventFilter, priority);
}

void QEventFilterService::unregisterEventFilter(QObject *eventFilter)
{
    Q_D(QEventFilterService);
    if (!d->m_eventDispatcher)
        return;
    d->m_eventDispatcher->unregisterEventFilter(eventFilter);
}

} // Qt3DCore

QT_END_NAMESPACE

#include "qeventfilterservice.moc"
