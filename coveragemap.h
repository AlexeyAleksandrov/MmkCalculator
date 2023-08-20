#ifndef COVERAGEMAP_H
#define COVERAGEMAP_H

#include "coveragemapitem.h"

#include <QStringList>

class CoverageMap   // карта покрытия
{
public:
    CoverageMap();

    const QStringList &getHorizontalHeaders() const;
    void setHorizontalHeaders(const QStringList &newHorizontalHeaders);
    const QStringList &getVerticalHeaders() const;
    void setVerticalHeaders(const QStringList &newVerticalHeaders);
    const QVector<QVector<CoverageMapItem> > &getCoating() const;
    void setCoating(const QVector<QVector<CoverageMapItem> > &newCoating);

private:
    QStringList horizontalHeaders;  // значения функции
    QStringList verticalHeaders;    // значения склеек
    QVector<QVector<CoverageMapItem>> coating;    // карта покрытия (строим двумерный массив покрытия значений). Формат: row<col>
};

#endif // COVERAGEMAP_H
