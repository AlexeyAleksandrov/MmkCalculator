#ifndef MMKDATA_H
#define MMKDATA_H

#include "coveragemap.h"
#include "glue.h"

#include <QString>
#include <QStringList>

//struct CoverageMap      // карта покрытия
//{
//    QStringList horizontalHeaders;  // значения функции
//    QStringList verticalHeaders;    // значения склеек
//    QVector<QVector<bool>> coating;    // карта покрытия (строим двумерный массив покрытия значений). Формат: row<col>
//};

// класс представляющий собой информацию о минимизации
class MmkData
{
public:
    enum MmkType    // тип минимизации
    {
        MKNF = 0,
        MDNF = 1
    };

    MmkData() {}
    MmkData(const QString &func16, const QString &func2, const QStringList &truthTable, MmkType mmkType, const QStringList &valuesTable, const QList<Glue> &glueList, const CoverageMap &coverageMap, const QStringList &coreDnf, const QList<QStringList> &allVariativePart, const QList<QStringList> &optimalVariativePart, const QList<QStringList> &results);

private:
    QString func16;     // функция в 16й ССЧ
    QString func2;      // функция в 2й ССЧ
    QStringList truthTable;     // таблица истинности
    MmkType mmkType;        // тип минимизации
    QStringList valuesTable;    //таблица значений (для формирования МДНФ/МКНФ)
    QList<Glue> glueList;    // список склеек
    CoverageMap coverageMap;        // карта покрытия
    QStringList coreDnf;     // значения, формирующие ядровую ДНФ
    QList<QStringList> allVariativePart;   // все возможные наборы значений вариативной части (без ядровой)
    QList<QStringList> optimalVariativePart;   // оптимальные (с минимальным кол-вом минтермов) возможные наборы значений вариативной части (без ядровой)
    QList<QStringList> results;   // результирующие минимализированные наборы

public:
    const QString &getFunc16() const;
    const QString &getFunc2() const;
    const QStringList &getTruthTable() const;
    MmkType getMmkType() const;
    const QStringList &getValuesTable() const;
    const QList<Glue> &getGlueList() const;
    const CoverageMap &getCoverageMap() const;
    const QStringList &getCoreDnf() const;
    const QList<QStringList> &getAllVariativePart() const;
    const QList<QStringList> &getOptimalVariativePart() const;
    const QList<QStringList> &getResults() const;
    QString toString() const;   // функция для получения строкового представления объекта
};

#endif // MMKDATA_H
