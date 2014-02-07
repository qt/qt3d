/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>
#include <QtCore/qset.h>
#include "qglpickcolors_p.h"

class tst_QGLPickColors : public QObject
{
    Q_OBJECT
public:
    tst_QGLPickColors() {}
    ~tst_QGLPickColors() {}

private slots:
    void distinctColors();
    void colorSpread();

    // Round-trip tests check various kinds of ways in which GPU's
    // may take the incoming RGB888 value for the pick color and turn
    // it into a value in the framebuffer, and then convert that
    // back into an RGB888 value later.
    //
    // For example, converting RGB888 -> RGB565 -> RGB888 will lose some
    // precision.  The amount of lost precision will depend upon the
    // specific way in which the GPU does the conversion.  The function
    // qt_qgl_normalize_pick_color() corrects for this and must always
    // return to the same value irrespective of how the GPU works.
    void roundTripExact();
    void roundTripTrunc565();
    void roundTripScale565();
    void roundTripDup565();
    void roundTripTrunc555();
    void roundTripScale555();
    void roundTripDup555();
    void roundTripScale444();       // Trunc doesn't work well with 444.
    void roundTripDup444();
};

// Check that all pick colors are distinct, opaque, and not black.
void tst_QGLPickColors::distinctColors()
{
    QSet<QRgb> used;
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index)) != 0) {
        QVERIFY(!used.contains(rgb));
        used += rgb;
        QVERIFY(qAlpha(rgb) == 255);                     // Opaque.
        QVERIFY(qRed(rgb) || qGreen(rgb) || qBlue(rgb)); // Not black.
        ++index;
    }
    QCOMPARE(rgb, QRgb(0));
    QCOMPARE(qt_qgl_pick_color(-1), QRgb(0));
    QVERIFY(index >= 1024);
}

// Check that the colors are well spread out so that accidental
// mismatches will not occur.
void tst_QGLPickColors::colorSpread()
{
    int index1 = 0;
    int index2;
    QRgb rgb1, rgb2;
    while ((rgb1 = qt_qgl_pick_color(index1)) != 0) {
        index2 = qMax(index1 - 50, 0);
        while (index2 < index1) {
            rgb2 = qt_qgl_pick_color(index2);
            int rdiff = qRed(rgb1) - qRed(rgb2);
            int gdiff = qGreen(rgb1) - qGreen(rgb2);
            int bdiff = qBlue(rgb1) - qBlue(rgb2);
            int dist = rdiff * rdiff + gdiff * gdiff + bdiff * bdiff;
            if (dist < 15 * 15) {   // FIXME: need more distance.
                qDebug() << index1 << index2 << dist
                         << QString::number(rgb1, 16)
                         << QString::number(rgb2, 16);
            }
            QVERIFY(dist >= 15 * 15);
            ++index2;
        }
        ++index1;
    }
}

// Check that round-tripping a color via exact representation works.
void tst_QGLPickColors::roundTripExact()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0)
        QVERIFY(qt_qgl_normalize_pick_color(rgb) == rgb);
}

// Check round-tripping when RGB888 -> RGB565 uses truncation.
void tst_QGLPickColors::roundTripTrunc565()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0) {
        QRgb truncrgb = qRgb(qRed(rgb) & 0xF8,
                             qGreen(rgb) & 0xFC,
                             qBlue(rgb) & 0xF8);

        QVERIFY(qt_qgl_normalize_pick_color(truncrgb) == rgb);
    }
}

// Check round-tripping when RGB888 -> RGB565 scales with floating point.
void tst_QGLPickColors::roundTripScale565()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0) {
        int r, g, b;
        r = int(qRed(rgb) * 31.0f / 255.0f);
        g = int(qGreen(rgb) * 63.0f / 255.0f);
        b = int(qBlue(rgb) * 31.0f / 255.0f);

        QRgb scalergb = qRgb(r * 255.0f / 31.0f,
                             g * 255.0f / 63.0f,
                             b * 255.0f / 31.0f);

        QVERIFY(qt_qgl_normalize_pick_color(scalergb) == rgb);

        // Scale up, but duplicate down.
        QRgb duprgb = qRgb((r << 3) | (r >> 2),
                           (g << 2) | (g >> 4),
                           (b << 3) | (b >> 2));

        QVERIFY(qt_qgl_normalize_pick_color(duprgb) == rgb);
    }
}

// Check round-tripping when RGB565 -> RGB888 uses bit-duplication.
void tst_QGLPickColors::roundTripDup565()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0) {
        int r, g, b;
        r = qRed(rgb) & 0xF8;
        r = r | (r >> 5);
        g = qGreen(rgb) & 0xFC;
        g = g | (g >> 6);
        b = qBlue(rgb) & 0xF8;
        b = b | (b >> 5);

        QRgb duprgb = qRgb(r, g, b);

        QVERIFY(qt_qgl_normalize_pick_color(duprgb) == rgb);
    }
}

// Check round-tripping when RGB888 -> RGB555 uses truncation.
void tst_QGLPickColors::roundTripTrunc555()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0) {
        QRgb truncrgb = qRgb(qRed(rgb) & 0xF8,
                             qGreen(rgb) & 0xF8,
                             qBlue(rgb) & 0xF8);

        QVERIFY(qt_qgl_normalize_pick_color(truncrgb) == rgb);
    }
}

// Check round-tripping when RGB888 -> RGB555 scales with floating point.
void tst_QGLPickColors::roundTripScale555()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0) {
        int r, g, b;
        r = int(qRed(rgb) * 31.0f / 255.0f);
        g = int(qGreen(rgb) * 31.0f / 255.0f);
        b = int(qBlue(rgb) * 31.0f / 255.0f);

        QRgb scalergb = qRgb(r * 255.0f / 31.0f,
                             g * 255.0f / 31.0f,
                             b * 255.0f / 31.0f);

        QVERIFY(qt_qgl_normalize_pick_color(scalergb) == rgb);

        // Scale up, but duplicate down.
        QRgb duprgb = qRgb((r << 3) | (r >> 2),
                           (g << 3) | (g >> 2),
                           (b << 3) | (b >> 2));

        QVERIFY(qt_qgl_normalize_pick_color(duprgb) == rgb);
    }
}

// Check round-tripping when RGB555 -> RGB888 uses bit-duplication.
void tst_QGLPickColors::roundTripDup555()
{
    int index = 0;
    QRgb rgb;
    while ((rgb = qt_qgl_pick_color(index++)) != 0) {
        int r, g, b;
        r = qRed(rgb) & 0xF8;
        r = r | (r >> 5);
        g = qGreen(rgb) & 0xF8;
        g = g | (g >> 5);
        b = qBlue(rgb) & 0xF8;
        b = b | (b >> 5);

        QRgb duprgb = qRgb(r, g, b);

        QVERIFY(qt_qgl_normalize_pick_color(duprgb) == rgb);
    }
}

// Note: the RGB444 conversion falls apart after 1330 entries
// in the table.  It is no longer possible to distinguish the
// colors while retaining stability in the face of GPU differences.
#define RGB_444_LIMIT   1330

// Check round-tripping when RGB888 -> RGB444 scales with floating point.
void tst_QGLPickColors::roundTripScale444()
{
    int index = 0;
    QRgb rgb;
    while (index < RGB_444_LIMIT && (rgb = qt_qgl_pick_color(index++)) != 0) {
        int r, g, b;
        r = int(qRed(rgb) * 15.0f / 255.0f);
        g = int(qGreen(rgb) * 15.0f / 255.0f);
        b = int(qBlue(rgb) * 15.0f / 255.0f);

        QRgb scalergb = qRgb(r * 255.0f / 15.0f,
                             g * 255.0f / 15.0f,
                             b * 255.0f / 15.0f);

        QVERIFY(qt_qgl_normalize_pick_color(scalergb, true) == rgb);
    }
}

// Check round-tripping when RGB444 -> RGB888 uses bit-duplication.
// We assume that the values are scaled up, but duplicated down.
void tst_QGLPickColors::roundTripDup444()
{
    int index = 0;
    QRgb rgb;
    while (index < RGB_444_LIMIT && (rgb = qt_qgl_pick_color(index++)) != 0) {
        int r, g, b;
        r = int(qRed(rgb) * 15.0f / 255.0f);
        r = r | (r << 4);
        g = int(qGreen(rgb) * 15.0f / 255.0f);
        g = g | (g << 4);
        b = int(qBlue(rgb) * 15.0f / 255.0f);
        b = b | (b << 4);

        QRgb duprgb = qRgb(r, g, b);

        QVERIFY(qt_qgl_normalize_pick_color(duprgb, true) == rgb);
    }
}

QTEST_APPLESS_MAIN(tst_QGLPickColors)

#include "tst_qglpickcolors.moc"
