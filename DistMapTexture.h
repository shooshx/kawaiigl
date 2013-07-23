#pragma once

#include <QString>


class DistMapTexture {
public:
    DistMapTexture() {}
    static void convert(const QString& filename, int smallSz, float halo);


};