#include "coveragemap.h"

CoverageMap::CoverageMap()
{

}

const QStringList &CoverageMap::getHorizontalHeaders() const
{
    return horizontalHeaders;
}

void CoverageMap::setHorizontalHeaders(const QStringList &newHorizontalHeaders)
{
    horizontalHeaders = newHorizontalHeaders;
}

const QStringList &CoverageMap::getVerticalHeaders() const
{
    return verticalHeaders;
}

void CoverageMap::setVerticalHeaders(const QStringList &newVerticalHeaders)
{
    verticalHeaders = newVerticalHeaders;
}

const QVector<QVector<CoverageMapItem> > &CoverageMap::getCoating() const
{
    return coating;
}

void CoverageMap::setCoating(const QVector<QVector<CoverageMapItem> > &newCoating)
{
    coating = newCoating;
}
