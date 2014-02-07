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

#include "compareimage.h"

//#define DEBUG_QIMAGECOMPARE

#ifdef DEBUG_QIMAGECOMPARE
#include <QDebug>
#endif

// qProportionDifferentRGBPixels compares two images and returns a float
// between 0.0 and 1.0 indicating the proportion of pixels with a difference
// greater than the relevant threshold in RGB space, where 0.0 indicates that
// every pixel matches, and 1.0 indicates that no pixels match.
//
// For example, the RGB colors 0x0088ff and 0x000000 have a red difference
// of 0, a green difference of 0x88, and a blue difference of 0xff.  If any
// of these values is greater than the corresponding threshold, the pixel
// would be considered "different".
//
// If the images are different sizes, the function compares those pixels that
// fall in the overlapping area.  If one image is simply smaller than the
// other, this will be the size of the smaller image.

float qProportionDifferentRGBPixels(const QImage& img1, const QImage& img2, int redThreshold, int greenThreshold, int blueThreshold, int alphaThreshold)
{
    int result = 0;

    int width = qMin(img1.width(), img2.width());
    int height = qMin(img1.height(), img2.height());
    int pixelcount = 0;

#ifdef DEBUG_QIMAGECOMPARE
    int sumDiff = 0;
    QImage *resultImage = new QImage (width,height,QImage::Format_RGB16);
#endif


    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            QColor pix1 = img1.pixel(x,y);
            QColor pix2 = img2.pixel(x,y);
            int redDiff = qMax(pix1.red(),pix2.red()) - qMin(pix1.red(),pix2.red());
            int greenDiff = qMax(pix1.green(),pix2.green()) - qMin(pix1.green(),pix2.green());
            int blueDiff = qMax(pix1.blue(),pix2.blue()) - qMin(pix1.blue(),pix2.blue());
            int alphaDiff = qMax(pix1.alpha(),pix2.alpha()) - qMin(pix1.alpha(),pix2.alpha());

#ifdef DEBUG_QIMAGECOMPARE
            QColor colorDiff;
            colorDiff.setRed(redDiff);
            colorDiff.setGreen(greenDiff);
            colorDiff.setBlue(blueDiff);
            colorDiff.setAlpha(255-alphaDiff);
            sumDiff += redDiff+greenDiff+blueDiff;
#endif

            pixelcount++;
            if (redDiff > redThreshold
                || greenDiff > greenThreshold
                || blueDiff > blueThreshold
                || alphaDiff > alphaThreshold)
            {
                result++;
#ifdef DEBUG_QIMAGECOMPARE
                resultImage->setPixel(x,y, colorDiff.rgba());
#endif
            }
        }
    }
#ifdef DEBUG_QIMAGECOMPARE
    qDebug() << "qimageCompare result was: " << result << "/"<<pixelcount<<" = "<< (int)(((float)result/(float)pixelcount)*100) << "%";
    qDebug() << "Sum of absolute differences was:"<< sumDiff;
    qDebug() << "Average difference per pixel was:" << (float)sumDiff/(float)pixelcount;
#endif
    Q_ASSERT(pixelcount>0);
    return (float)result/(float)pixelcount;
}



inline float qProportionDifferentRGBPixels(const QImage& img1, const QImage& img2, int threshold )
{
    return qProportionDifferentRGBPixels(img1, img2, threshold, threshold, threshold);
}

bool qFuzzyCompare(const QImage& img1, const QImage& img2)
{
    // The two images must have the same size.
    if (img1.width() != img2.width() || img1.height() != img2.height())
        return false;


    int count = 0;
    for (int y = 0; y < img1.height(); ++y) {
        for (int x = 0; x < img1.width(); ++x) {
            // Compare the top two bits of the red, green, and blue
            // values for equality.  This should factor out rounding
            // errors due to 16-bit vs 32-bit conversions and so on.
            // We assume that the source images use primary colors.
            if ((img1.pixel(x, y) & 0x00C0C0C0) == (img2.pixel(x, y) & 0x00C0C0C0))
                ++count;
        }
    }

    // We consider the images the same if at least 95% of the
    // pixels have the same or very similar RGB values.

    // 64 is a rough approximation of bitwise & 0xC0
    return qProportionDifferentRGBPixels(img1, img2, 64) <= 0.05001;
    //return (count >= (img1.width() * img1.height() * 95 / 100));
}

bool qFuzzyCompare(const QImage& img1, const QColor& color)
{
    int count = 0;
    for (int y = 0; y < img1.height(); ++y) {
        for (int x = 0; x < img1.width(); ++x) {
            if ((img1.pixel(x, y) & 0x00C0C0C0) == (color.rgb() & 0x00C0C0C0))
                ++count;
        }
    }
    return (count >= (img1.width() * img1.height() * 95 / 100));
}


