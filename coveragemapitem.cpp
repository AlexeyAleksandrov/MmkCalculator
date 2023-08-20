#include "coveragemapitem.h"

CoverageMapItem::CoverageMapItem()
{

}

const QString &CoverageMapItem::getValue() const
{
    return value;
}

void CoverageMapItem::setValue(const QString &newValue)
{
    value = newValue;
}

const QString &CoverageMapItem::getGlue() const
{
    return glue;
}

void CoverageMapItem::setGlue(const QString &newGlue)
{
    glue = newGlue;
}

bool CoverageMapItem::isCoating() const
{
    return coating;
}

void CoverageMapItem::setCoating(bool newCoating)
{
    coating = newCoating;
}

bool CoverageMapItem::isCore() const
{
    return core;
}

void CoverageMapItem::setCore(bool newCore)
{
    core = newCore;
}
