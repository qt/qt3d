// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DRender/private/genericlambdajob_p.h>
#include <functional>

namespace {

int randomTestValue = 0;

void freeStandingFunction()
{
    randomTestValue = 883;
}

} // anonymous

class tst_GenericLambdaJob : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void lambdaTest()
    {
        // GIVEN
        int a = 0;
        auto incremeneterLambda = [&] () { ++a; };
        Qt3DRender::Render::GenericLambdaJob<decltype(incremeneterLambda)> job(incremeneterLambda);

        // THEN
        QCOMPARE(a, 0);

        // WHEN
        job.run();

        // THEN
        QCOMPARE(a, 1);

        // WHEN
        job.run();
        QCOMPARE(a, 2);
    }

    void functionTest()
    {
        // GIVEN
        Qt3DRender::Render::GenericLambdaJob<decltype(&freeStandingFunction)> job(&freeStandingFunction);

        // THEN
        QCOMPARE(randomTestValue, 0);

        // WHEN
        job.run();

        // THEN
        QCOMPARE(randomTestValue, 883);
    }

    void stdFunctionTest()
    {
        // GIVEN
        int u = 1584;
        auto decrementerLambda = [&] () { u = 0; };
        std::function<void ()> func = decrementerLambda;
        Qt3DRender::Render::GenericLambdaJob<decltype(func)> job(func);

        // THEN
        QCOMPARE(u, 1584);

        // WHEN
        job.run();

        // THEN
        QCOMPARE(u, 0);
    }
};

QTEST_APPLESS_MAIN(tst_GenericLambdaJob)

#include "tst_genericlambdajob.moc"
