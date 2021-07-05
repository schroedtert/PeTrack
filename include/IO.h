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
 * along with this program.  If not, see <https://cdwww.gnu.org/licenses/>.
 */

#ifndef IO_H
#define IO_H

#include <variant>
#include <unordered_map>
#include <string>
#include <QString>
#include <ezc3d_all.h>
#include <opencv2/opencv.hpp>
class MoCapStorage;
class MoCapPerson;
class MoCapPersonMetadata;


namespace IO
{
    std::variant<std::unordered_map<int, float>, std::string>
    readHeightFile(const QString& heightFileName);

    void readMoCapC3D(MoCapStorage &storage, const MoCapPersonMetadata &metadata);
    void readSkeletonC3D_XSENS(
        const ezc3d::c3d &c3d,
        MoCapPerson &person,
        const std::function<cv::Point3f(const ezc3d::DataNS::Points3dNS::Point&)>& c3dToPoint3f
        );

    std::variant<std::unordered_map<int, int>, std::string>
    readMarkerIDFile(const QString& markerFileName);

    std::vector<std::string> readAuthors(const QString & authorsFile);
}
#endif //IO_H
