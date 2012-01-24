/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import "testlogger.js" as TestLogger

Item {
    id: testCase
    visible: false

    // Name of the test case to prefix the function name in messages.
    property string name

    // Set to true to start the test running.
    property bool when: true

    // Set to true once the test has completed.
    property bool completed: false

    // Set to true when the test is running but not yet complete.
    property bool running: false

    // Set to true if the test doesn't have to run (because some
    // other test failed which this one depends on).
    property bool optional: false

    // Internal private state
    property string currentTestCase
    property bool expectingFail
    property string expectFailMsg
    property bool prevWhen: true
    property int testId: -1

    TestReport { id: reporter }

    function fail(msg) {
        if (!msg)
            msg = "";
        if (expectingFail) {
            TestLogger.log_expect_fail(currentTestCase, expectFailMsg, msg)
            throw new Error("QtTest::expect_fail")
        } else {
            TestLogger.log_fail(currentTestCase, msg)
            throw new Error("QtTest::fail")
        }
    }

    function fail2(msg, msg2) {
        if (msg)
            fail(msg + ": " + msg2)
        else
            fail(msg2)
    }

    function verify(cond, msg) {
        if (!cond)
            fail(msg)
    }

    function compare(actual, expected, msg) {
        if (typeof actual == "number" && typeof expected == "number") {
            // Use a fuzzy compare if the two values are floats
            if (Math.abs(actual - expected) <= 0.00001)
                return
        } else if (typeof actual == "object" && typeof expected == "object") {
            // Does the expected value look like a vector3d?
            if ("x" in expected && "y" in expected && "z" in expected) {
                if (Math.abs(actual.x - expected.x) <= 0.00001 &&
                        Math.abs(actual.y - expected.y) <= 0.00001 &&
                        Math.abs(actual.z - expected.z) <= 0.00001)
                    return
                fail2(msg, "actual: Qt.vector3d(" +
                           actual.x + ", " + actual.y + ", " + actual.z +
                           "), expected: Qt.vector3d(" +
                           expected.x + ", " + expected.y + ", " + expected.z +
                           ")")
                return
            }
            if (actual == expected)
                return
        } else if (actual == expected) {
            return
        }
        fail2(msg, "actual: " + actual + ", expected: " + expected)
    }

    function skip(msg) {
        TestLogger.log_skip(currentTestCase, msg)
        throw new Error("QtTest::skip")
    }

    function expectFail(msg) {
        expectingFail = true
        expectFailMsg = msg
    }

    property variant testCaseResult

    function runInternal(prop, dataDriven, arg, tag) {
        currentTestCase = TestLogger.log_prefixed_name(name, prop)
        if (dataDriven && tag)
            currentTestCase += " [" + tag + "]"
        expectingFail = false
        var success = true
        try {
            testCaseResult = testCase[prop](arg)
            if (expectingFail) {
                success = false
                TestLogger.log_expect_fail_pass(currentTestCase)
            } else if (!dataDriven) {
                TestLogger.log_pass(currentTestCase)
            }
        } catch (e) {
            testCaseResult = []
            if (e.message == "QtTest::fail") {
                success = false
            } else if (e.message.indexOf("QtTest::") != 0) {
                // Test threw an unrecognized exception - fail.
                TestLogger.log_fail(currentTestCase, e.message)
                success = false
            }
        }
        return success
    }

    function run() {
        TestLogger.log_start_test(reporter)
        var success = true
        running = true
        var testList = []
        for (var prop in testCase) {
            if (prop.indexOf("test_") != 0)
                continue
            if (filterTestCases.length > 0) {
                // if there is a list, only run the tests in the list
                var excludeTest = true
                for (var i = 0; i < filterTestCases.length && excludeTest; i++)
                {
                    if (filterTestCases[i] == prop)
                        excludeTest = false
                }
                if (excludeTest)
                    continue
            }
            var tail = prop.lastIndexOf("_data");
            if (tail != -1 && tail == (prop.length - 5))
                continue
            testList.push(prop)
        }
        testList.sort()
        for (var index in testList) {
            var prop = testList[index]
            var datafunc = prop + "_data"
            if (datafunc in testCase) {
                if (runInternal(datafunc, true)) {
                    var table = testCaseResult
                    var successThis = true
                    var haveData = false
                    for (var index in table) {
                        haveData = true
                        var row = table[index]
                        if (!runInternal(prop, true, row, row.tag))
                            successThis = false
                    }
                    if (!haveData)
                        TestLogger.log_message("WARNING: no data supplied for " + prop + "() by " + datafunc + "()")
                    if (successThis) {
                        var prefix;
                        if (name)
                            prefix = name + "::"
                        currentTestCase = prefix + prop + "()"
                        TestLogger.log_pass(currentTestCase)
                    } else {
                        success = false
                    }
                } else {
                    success = false
                }
            } else {
                if (!runInternal(prop, false))
                    success = false
            }
        }
        currentTestCase = ""
        running = false
        completed = true
        TestLogger.log_complete_test(testId, reporter)
        return success
    }

    onWhenChanged: {
        if (when != prevWhen) {
            prevWhen = when
            if (when && !completed && !running)
                run()
        }
    }

    onOptionalChanged: {
        if (!completed) {
            if (optional)
                TestLogger.log_optional_test(testId)
            else
                TestLogger.log_mandatory_test(testId)
        }
    }

    Component.onCompleted: {
        testId = TestLogger.log_register_test(name)
        if (optional)
            TestLogger.log_optional_test(testId)
        prevWhen = when
        if (when && !completed && !running)
            run()
    }
}
