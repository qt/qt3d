// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qhandle_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>

class tst_Handle : public QObject
{
    Q_OBJECT
public:
    tst_Handle() {}
    ~tst_Handle() {}

private slots:
    void defaultConstruction();
    void construction();
    void copyConstruction();
    void assignment();
    void equality();
    void inequality();
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
typedef Qt3DCore::QHandleData<SimpleResource> HandleData;

void tst_Handle::defaultConstruction()
{
    Handle h;
    QVERIFY(h.isNull() == true);
    QVERIFY(h.handle() == 0);
}

void tst_Handle::construction()
{
    HandleData d;
    Handle h(&d);
    QVERIFY(h.isNull() == false);
    qDebug() << h;
    QVERIFY(h.handle() == reinterpret_cast<quintptr>(&d));
}

void tst_Handle::copyConstruction()
{
    HandleData d;
    Handle h(&d);
    Handle h2(h);
    QVERIFY(h2.isNull() == false);
    QVERIFY(h2.handle() == h.handle());
}

void tst_Handle::assignment()
{
    HandleData d;
    Handle h(&d);
    Handle h2;
    h2 = h;
    QVERIFY(h2.isNull() == false);
    QVERIFY(h2.handle() == h.handle());
}

void tst_Handle::equality()
{
    HandleData d;
    Handle h1(&d);
    Handle h2(&d);
    QVERIFY(h1.isNull() == false);
    QVERIFY(h1 == h2);
}

void tst_Handle::inequality()
{
    HandleData d1;
    HandleData d2;
    Handle h1(&d1);
    Handle h2(&d2);
    QVERIFY(h1.isNull() == false);
    QVERIFY(h1 != h2);
}

QTEST_APPLESS_MAIN(tst_Handle)

#include "tst_handle.moc"
