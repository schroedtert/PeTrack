/*
 * PeTrack - Software for tracking pedestrians movement in videos
 * Copyright (C) 2010-2020 Forschungszentrum Jülich GmbH,
 * Maik Boltes, Juliane Adrian, Ricardo Martin Brualla, Arne Graf, Paul Häger, Daniel Hillebrand,
 * Deniz Kilic, Paul Lieberenz, Daniel Salden, Tobias Schrödter, Ann Katrin Seemann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HELPER_H
#define HELPER_H

#include <QFileInfo>
#include <QString>
#include <opencv2/opencv.hpp>

extern QString commandLineOptionsString;
extern QString proFileName; ///< Path to the project (.pet) file; defined in helper.cpp

constexpr const char *file_name(const char *path)
{
    const char *file = path;
    while(*path)
    {
        const char current = *path;
        ++path;
        if(current == '/' || current == '\\')
        {
            file = path;
        }
    }
    return file;
}

// gleiche Variable wie QT benutzt, es gibt auch noch QT_NO_DEBUG and QT_NO_WARNING_OUTPUT
#ifdef QT_NO_DEBUG_OUTPUT
#define debout //
#else
#define debout std::cout << __func__ << " in " << file_name(__FILE__) << " line " << __LINE__ << ": "
#endif

#include <QString>
#include <iostream>
inline std::ostream &operator<<(std::ostream &s, const QString &t)
{
    s << t.toStdString();
    return s;
}


#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

inline constexpr double PI = 3.141592654;

// SHARING MAY ALSO WORK:
//    > IplImage* frame = cvQueryFrame(mCamera);
//    > QImage qtImage = QImage(QSize(frame->width, frame->height), QImage::Format_RGB888);
//    > cvImage = cvCreateImageHeader(cvSize(frame->width, frame->height), 8, 3);
//    > cvImage->imageData = (char *)qtImage.bits();
// Static function that converts an iplImg in a qImg.
// If the images are not the same size, a new qImg will be created with the same size as the iplImg.
#include <QImage>
#ifndef STEREO_DISABLED
#include "cxcore.h"
void copyToQImage(QImage *qImg, IplImage *iplImg); // war static functin in animatioln class
#endif
void copyToQImage(QImage &qImg, cv::Mat &img);

#include <QLabel>
#ifndef STEREO_DISABLED
void showImg(QLabel *l, IplImage *i);
#endif

cv::Mat getRoi(cv::Mat &img, const QRect &roi, cv::Rect &rect, bool evenPixelNumber = true);

inline double getMedianOf3(double a, double b, double c)
{
    if(a < b)
    {
        if(b < c)
        {
            return b; // a b c
        }
        else if(c < b && c < a)
        {
            return a; // c a b
        }
        else
        {
            return c; // a c b
        }
    }
    else // b<=a
    {
        if(a < c)
        {
            return a; // b a c
        }
        else if(c < a && c < b)
        {
            return b; // c b a
        }
        else
        {
            return c; // b c a
        }
    }
}

// d darf keine seiteneffekte haben!!!
// myround genommen, da round manchmal in math.h existiert, aber nicht immer
#define myRound(d)          (static_cast<int>(((d) < 0 ? (d) -.5 : (d) + .5)))
#define myClip(d, min, max) (((d) < min) ? (min) : (((d) > max) ? (max) : (d)))

// get image color from 3-channel-images with values 0..255
#define getR(img, x, y) ((int) *(uchar *) ((img)->imageData + (img)->widthStep * (y) + (img)->nChannels * (x) + 2))
#define getG(img, x, y) ((int) *(uchar *) ((img)->imageData + (img)->widthStep * (y) + (img)->nChannels * (x) + 1))
#define getB(img, x, y) ((int) *(uchar *) ((img)->imageData + (img)->widthStep * (y) + (img)->nChannels * (x)))

// get image grey value from grey-images with values 0..255 (may be also 3 channels???)
#define getGrey(img, x, y) ((int) *(uchar *) ((img)->imageData + (img)->widthStep * (y) + (x)))

inline cv::Scalar qcolor2scalar(QColor color)
{
    int r, g, b;
    color.getRgb(&r, &g, &b);
    return cv::Scalar(b, g, r); // swap RGB-->BGR
}

inline QColor scalar2qcolor(cv::Scalar color)
{
    QColor ret;
    ret.setHsv(0, 0, color[0]);
    return ret; // swap RGB-->BGR
}

inline QColor getValue(const cv::Mat &img, int x, int y)
{
    QColor     ret;
    cv::Scalar scalar;
    cv::Vec3b  val;
    switch(img.channels())
    {
        case 1:
            scalar = img.at<uchar>(cv::Point(x, y));
            ret    = scalar2qcolor(scalar);
            break;
        case 3:
        case 4:
            val = img.at<cv::Vec3b>(cv::Point(x, y));
            ret.setRgb(val.val[2], val.val[1], val.val[0]);
            break;
        default:;
    }
    return ret;
}
#include <QColor>
#include <QTextStream>
inline std::ostream &operator<<(std::ostream &s, const QColor &col)
{
    if(col.isValid())
    {
        s << col.red() << " " << col.green() << " " << col.blue();
    }
    else
    {
        s << -1 << " " << -1 << " " << -1;
    }
    return s;
}
inline QTextStream &operator<<(QTextStream &s, const QColor &col)
{
    if(col.isValid())
    {
        s << col.red() << " " << col.green() << " " << col.blue();
    }
    else
    {
        s << -1 << " " << -1 << " " << -1;
    }
    return s;
}
inline QTextStream &operator>>(QTextStream &s, QColor &col)
{
    int i;
    // leave invalid, if one number is -1
    s >> i;
    if(i != -1)
    {
        col.setRed(i);
        s >> i;
        col.setGreen(i);
        s >> i;
        col.setBlue(i);
    }
    else
    {
        s >> i >> i;
    }
    return s;
}

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
/**
 * @brief checks the ;-separated file names for existence and returns the first
 *
 * Interesting for working with absolute <em>and</em> relative paths.
 *
 * @param fileList
 * @param relToFileName
 * @return first file to exist
 */
inline QString getExistingFile(const QString &fileList, const QString &relToFileName = proFileName)
{
    QStringList list;
    list = fileList.split(";", Qt::SkipEmptyParts);
    for(int i = 0; i < list.size(); ++i)
    {
        if(QFile(list.at(i)).exists())
        {
            return list.at(i);
        }
        if(QFile(list.at(i).trimmed()).exists())
        {
            return list.at(i).trimmed();
        }
        if(QFile(QFileInfo(relToFileName).absolutePath() + "/" + list.at(i).trimmed()).exists())
        {
            return QFileInfo(relToFileName).absolutePath() + "/" + list.at(i).trimmed();
        }
    }
    return ""; // wenn keine der Dateien existiert
}

#include <QDir>
#include <QFileInfo>
inline QString getFileList(const QString &fileName, const QString &relToFileName = proFileName)
{
    QString seqAbs      = QFileInfo(fileName).absoluteFilePath();
    QString seqRelToPro = QDir(QFileInfo(relToFileName).absolutePath()).relativeFilePath(seqAbs);

    if(QFileInfo(fileName).isRelative())
    {
        if(fileName == seqRelToPro)
        {
            return fileName + ";" + seqAbs;
        }
        else
        {
            return fileName + ";" + seqAbs + ";" + seqRelToPro;
        }
    }
    else
    {
        return fileName + ";" + seqRelToPro;
    }
}

#include <ctime>
inline clock_t getElapsedTime()
{
    static clock_t lastTime = clock();
    static clock_t diffTime; // fuer performance
    diffTime = clock() - lastTime;
    lastTime = clock();
    return diffTime;
}

#endif
