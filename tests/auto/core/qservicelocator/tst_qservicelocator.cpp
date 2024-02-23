// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qopenglinformationservice_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p.h>

#include <QScopedPointer>

using namespace Qt3DCore;

class DummyService : public QAbstractServiceProvider
{
public:
    DummyService()
        : QAbstractServiceProvider(QServiceLocator::UserService + 1,
                                   QStringLiteral("Dummy Service"))
    {}

    int data() const { return 10; }
    float moreData() const { return 3.14159f; }
};


class DummySystemInfoService : public QSystemInformationService
{
public:
    DummySystemInfoService()
        : QSystemInformationService(nullptr, QStringLiteral("Dummy System Information Service"))
    {}
};


class tst_QServiceLocator : public QObject
{
    Q_OBJECT

public:
    tst_QServiceLocator() : QObject() {}
    ~tst_QServiceLocator() {}

private slots:
    void construction();
    void defaultServices();
    void addRemoveDefaultService();
    void addRemoveUserService();
};

void tst_QServiceLocator::construction()
{
    QServiceLocator locator;
    QVERIFY(locator.serviceCount() == QServiceLocator::DefaultServiceCount);
}

void tst_QServiceLocator::defaultServices()
{
    QServiceLocator locator;
    QOpenGLInformationService *glInfo = locator.openGLInformation();
    QVERIFY(glInfo != nullptr);
    QVERIFY(glInfo->description() == QStringLiteral("Null OpenGL Information Service"));

    QSystemInformationService *sysInfo = locator.systemInformation();
    QVERIFY(sysInfo != nullptr);
    QVERIFY(sysInfo->description() == QStringLiteral("Default System Information Service"));
    QVERIFY(sysInfo->threadPoolThreadCount() != 0);
}

void tst_QServiceLocator::addRemoveDefaultService()
{
    // Create locator and register a custom service that replaces a default service
    QScopedPointer<DummySystemInfoService> dummy(new DummySystemInfoService);
    QServiceLocator locator;
    locator.registerServiceProvider(QServiceLocator::SystemInformation, dummy.data());
    QVERIFY(locator.serviceCount() == QServiceLocator::DefaultServiceCount);

    // Get the service from the locator and check it works as expected
    QSystemInformationService *service = locator.systemInformation();
    QVERIFY(service == dummy.data());

    // Ensure the other default services work
    QOpenGLInformationService *glInfo = locator.openGLInformation();
    QVERIFY(glInfo != nullptr);
    QVERIFY(glInfo->description() == QStringLiteral("Null OpenGL Information Service"));
    QVERIFY(glInfo->format() == QSurfaceFormat());

    // Remove custom service
    locator.unregisterServiceProvider(QServiceLocator::SystemInformation);
    QVERIFY(locator.serviceCount() == QServiceLocator::DefaultServiceCount);

    // Check the dummy service still exists
    QVERIFY(!dummy.isNull());
}

void tst_QServiceLocator::addRemoveUserService()
{
    // Create locator and register a custom service
    QScopedPointer<DummyService> dummy(new DummyService);
    QServiceLocator locator;
    locator.registerServiceProvider(dummy->type(), dummy.data());
    QVERIFY(locator.serviceCount() == QServiceLocator::DefaultServiceCount + 1);

    // Get the service from the locator and check it works as expected
    DummyService *service = locator.service<DummyService>(dummy->type());
    QVERIFY(service == dummy.data());
    QVERIFY(service->data() == 10);
    QVERIFY(qFuzzyCompare(service->moreData(), 3.14159f));

    // Ensure the default services work
    QSystemInformationService *sysInfo = locator.systemInformation();
    QVERIFY(sysInfo != nullptr);
    QVERIFY(sysInfo->description() == QStringLiteral("Default System Information Service"));
    QVERIFY(sysInfo->threadPoolThreadCount() != 0);

    // Remove custom service
    locator.unregisterServiceProvider(dummy->type());
    QVERIFY(locator.serviceCount() == QServiceLocator::DefaultServiceCount);

    // Check the dummy service still exists
    QVERIFY(dummy->data() == 10);
}

QTEST_MAIN(tst_QServiceLocator)

#include "tst_qservicelocator.moc"
