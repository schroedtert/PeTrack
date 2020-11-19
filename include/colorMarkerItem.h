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

#ifndef ColorMarkerItem_H
#define ColorMarkerItem_H

#include <QGraphicsItem>

#include "vector.h"

class Petrack;
class Control;
class Tracker;

class ColorMarkerItem : public QGraphicsItem
{
private:
    Petrack *mMainWindow;
    QImage *mImage;
//    IplImage *mMask;
    cv::Mat mMask;
    Vec2F mUlc;  // upper left corner to draw

public:
    ColorMarkerItem(QWidget *wParent, QGraphicsItem * parent = NULL);
    QRectF boundingRect() const;
    void setRect(Vec2F& v);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    cv::Mat getMask()
    {
        return mMask;
    }
    void setMask(cv::Mat &mask);
    cv::Mat createMask(int w, int h);
};

#endif
