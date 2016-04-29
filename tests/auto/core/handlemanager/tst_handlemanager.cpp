/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DCore/private/qhandlemanager_p.h>
#include <Qt3DCore/private/qhandle_p.h>

class tst_HandleManager : public QObject
{
    Q_OBJECT
public:
    tst_HandleManager() {}
    ~tst_HandleManager() {}

private slots:
    void construction();
    void correctPointer();
    void correctPointers();
    void correctConstPointer();
    void nullForRemovedEntry();
    void validHandleForReplacementEntry();
    void updateChangesValue();
    void resetRemovesAllEntries();
    void maximumEntries();
};

class SimpleResource
{
public:
    SimpleResource()
        : m_value(0)
    {}

    int m_value;
};

typedef Qt3DCore::QHandle<SimpleResource> Handle;

void tst_HandleManager::construction()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    QVERIFY(manager.activeEntries() == 0);
}

void tst_HandleManager::correctPointer()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    SimpleResource *p1 = (SimpleResource *)0xdeadbeef;
    const Handle h = manager.acquire(p1);

    bool ok = false;
    SimpleResource *p2 = manager.data(h, &ok);
    QVERIFY(ok == true);
    QVERIFY(p1 == p2);
}

void tst_HandleManager::correctPointers()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    SimpleResource *p[3];
    p[0] = (SimpleResource *)0xdeadbeef;
    p[1] = (SimpleResource *)0x11111111;
    p[2] = (SimpleResource *)0x22222222;

    for (int i = 0; i < 3; ++i) {
        const Handle h = manager.acquire(p[i]);

        bool ok = false;
        SimpleResource *q = manager.data(h, &ok);
        QVERIFY(ok == true);
        QVERIFY(p[i] == q);
    }

    QVERIFY(manager.activeEntries() == 3);
}

void tst_HandleManager::correctConstPointer()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    QSharedPointer<SimpleResource> p1(new SimpleResource);
    const Handle h = manager.acquire(p1.data());

    bool ok = false;
    const SimpleResource *p2 = manager.constData(h, &ok);
    QVERIFY(ok == true);
    QVERIFY(p1.data() == p2);
}

void tst_HandleManager::nullForRemovedEntry()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    QSharedPointer<SimpleResource> p1(new SimpleResource);
    const Handle h = manager.acquire(p1.data());
    manager.release(h);

    bool ok = false;
    SimpleResource *p2 = manager.data(h, &ok);
    QVERIFY(ok == false);
    QVERIFY(p2 == nullptr);
}

void tst_HandleManager::validHandleForReplacementEntry()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    QSharedPointer<SimpleResource> p1(new SimpleResource);
    const Handle h = manager.acquire(p1.data());
    QVERIFY(manager.activeEntries() == 1);
    manager.release(h);
    QVERIFY(manager.activeEntries() == 0);

    QSharedPointer<SimpleResource> p2(new SimpleResource);
    const Handle h2 = manager.acquire(p2.data());
    QVERIFY(h2.isNull() == false);
    QVERIFY(h2.counter() == 2);
    QVERIFY(manager.activeEntries() == 1);

    bool ok = false;
    SimpleResource *p3 = manager.data(h2, &ok);
    QVERIFY(ok == true);
    QVERIFY(p3 == p2);
}

void tst_HandleManager::updateChangesValue()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    QSharedPointer<SimpleResource> p1(new SimpleResource);
    const Handle h = manager.acquire(p1.data());

    QSharedPointer<SimpleResource> p2(new SimpleResource);
    manager.update(h, p2.data());
    QVERIFY(manager.activeEntries() == 1);

    bool ok = false;
    SimpleResource *p3 = manager.data(h, &ok);
    QVERIFY(ok == true);
    QVERIFY(p3 == p2);
}

void tst_HandleManager::resetRemovesAllEntries()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;
    for (int i = 0; i < 100; ++i) {
        SimpleResource *p = (SimpleResource *) 0xdead0000 + i;
        const Handle h = manager.acquire(p);

        bool ok = false;
        SimpleResource *q = manager.data(h, &ok);
        QVERIFY(ok == true);
        QVERIFY(p == q);
    }

    QVERIFY(manager.activeEntries() == 100);

    manager.reset();
    QVERIFY(manager.activeEntries() == 0);
}

void tst_HandleManager::maximumEntries()
{
    Qt3DCore::QHandleManager<SimpleResource> manager;

    QCOMPARE(Handle::maxIndex(), (uint)((1 << 16) - 1));

    for (int i = 0; i < (int)Handle::maxIndex(); ++i) {
        SimpleResource *p = (SimpleResource *) 0xdead0000 + i;
        const Handle h = manager.acquire(p);

        bool ok = false;
        SimpleResource *q = manager.data(h, &ok);
        QVERIFY(ok == true);
        QVERIFY(p == q);
    }

    QVERIFY(manager.activeEntries() == Handle::maxIndex());
    manager.reset();
    QVERIFY(manager.activeEntries() == 0);
}

QTEST_APPLESS_MAIN(tst_HandleManager)

#include "tst_handlemanager.moc"
