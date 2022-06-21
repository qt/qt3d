// Copyright (C) 2015 Paul Lemire (paul.lemire350@gmail.com)
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qeventfilterservice_p.h"

#include <QtCore/QObject>
#include <QtGui/QHoverEvent>

#include <Qt3DCore/private/qabstractserviceprovider_p.h>

#include <algorithm>
#include <vector>

QT_BEGIN_NAMESPACE

namespace {
    struct FilterPriorityPair
    {
        QObject *filter;
        int priority;
    };

    const auto byPriority = [](const FilterPriorityPair &a, const FilterPriorityPair &b) noexcept
    {
        return a.priority < b.priority;
    };
}

Q_DECLARE_TYPEINFO(FilterPriorityPair, Q_PRIMITIVE_TYPE);

namespace Qt3DCore {

namespace {

class InternalEventListener : public QObject
{
    Q_OBJECT
public:
    explicit InternalEventListener(QEventFilterServicePrivate *filterService, QObject *parent = nullptr);
    bool eventFilter(QObject *obj, QEvent *e) final;
    QEventFilterServicePrivate* m_filterService;
};

} // anonymous


class QEventFilterServicePrivate : public QAbstractServiceProviderPrivate
{
public:
    QEventFilterServicePrivate()
        : QAbstractServiceProviderPrivate(QServiceLocator::EventFilterService, QStringLiteral("Default event filter service implementation"))
    {}

    Q_DECLARE_PUBLIC(QEventFilterService)

    void registerEventFilter(QObject *eventFilter, int priority)
    {
        FilterPriorityPair fpPair;
        fpPair.filter = eventFilter;
        fpPair.priority = priority;
        const auto it = std::lower_bound(m_eventFilters.begin(), m_eventFilters.end(), fpPair, byPriority);
        if (it == m_eventFilters.end() || it->priority != priority)
            m_eventFilters.insert(it, std::move(fpPair));
    }

    void unregisterEventFilter(QObject *eventFilter)
    {
        for (auto it = m_eventFilters.begin(), end = m_eventFilters.end(); it != end; ++it) {
            if (it->filter == eventFilter) {
                m_eventFilters.erase(it);
                return;
            }
        }
    }

    std::unique_ptr<InternalEventListener> m_eventDispatcher;
    std::vector<FilterPriorityPair> m_eventFilters;
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
    if (eventSource == nullptr) {
        d->m_eventDispatcher.reset();
    } else {
        d->m_eventDispatcher.reset(new InternalEventListener(d));
        eventSource->installEventFilter(d->m_eventDispatcher.get());
    }
}

void QEventFilterService::shutdown(QObject *eventSource)
{
    Q_D(QEventFilterService);
    if (eventSource && d->m_eventDispatcher.get())
        eventSource->removeEventFilter(d->m_eventDispatcher.get());
}

void QEventFilterService::registerEventFilter(QObject *eventFilter, int priority)
{
    Q_D(QEventFilterService);
    d->registerEventFilter(eventFilter, priority);
}

void QEventFilterService::unregisterEventFilter(QObject *eventFilter)
{
    Q_D(QEventFilterService);
    d->unregisterEventFilter(eventFilter);
}

namespace {

InternalEventListener::InternalEventListener(QEventFilterServicePrivate *filterService, QObject *parent)
    : QObject(parent)
    , m_filterService(filterService)
{
}

bool InternalEventListener::eventFilter(QObject *obj, QEvent *e)
{
    for (auto i = m_filterService->m_eventFilters.size(); i > 0; --i) {
        const FilterPriorityPair &fPPair = m_filterService->m_eventFilters[i - 1];
        if (fPPair.filter->eventFilter(obj, e))
            return true;
    }
    return false;
}

} // namespace

} // Qt3DCore

QT_END_NAMESPACE

#include "moc_qeventfilterservice_p.cpp"

#include "qeventfilterservice.moc"
