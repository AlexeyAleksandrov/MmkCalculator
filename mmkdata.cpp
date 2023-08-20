#include "mmkdata.h"

MmkData::MmkData(const QString &func16, const QString &func2, const QStringList &truthTable, MmkType mmkType, const QStringList &valuesTable, const QList<Glue> &glueList, const CoverageMap &coverageMap, const QStringList &coreDnf, const QList<QStringList> &allVariativePart, const QList<QStringList> &optimalVariativePart, const QList<QStringList> &results) : func16(func16),
    func2(func2),
    truthTable(truthTable),
    mmkType(mmkType),
    valuesTable(valuesTable),
    glueList(glueList),
    coverageMap(coverageMap),
    coreDnf(coreDnf),
    allVariativePart(allVariativePart),
    optimalVariativePart(optimalVariativePart),
    results(results)
{}

const QList<Glue> &MmkData::getGlueList() const
{
    return glueList;
}

const QList<QStringList> &MmkData::getResults() const
{
    return results;
}

// Реализация функции toString()
QString MmkData::toString() const
{
    QString result;

    // Добавляем информацию о функции в 16-й ССЧ
    result += "func16: " + func16 + "\n";

    // Добавляем информацию о функции в 2-й ССЧ
    result += "func2: " + func2 + "\n";

    // Добавляем информацию о таблице истинности
    result += "truthTable:\n";
    for (const QString& row : truthTable)
    {
        result += row + "\n";
    }

    // Добавляем информацию о типе минимизации
    result += "mmkType: ";
    result += (mmkType == MKNF) ? "MKNF\n" : "MDNF\n";

    // Добавляем информацию о таблице значений
    result += "valuesTable:\n";
    for (const QString& value : valuesTable)
    {
        result += value + "\n";
    }

    // Добавляем информацию о списке склеек
    result += "glueList:\n";
    for (const Glue& list : glueList)
    {
        for (const QString& value : list.getAllGluePart())
        {
            result += value + " ";
        }
        result += "\n";
    }

    // Добавляем информацию о карте покрытия
    result += "coverageMap:\n";
    result += "horizontalHeaders: " + coverageMap.getHorizontalHeaders().join(", ") + "\n";
    result += "verticalHeaders: " + coverageMap.getVerticalHeaders().join(", ") + "\n";
    for (const QVector<CoverageMapItem>& row : coverageMap.getCoating())
    {
        for (const CoverageMapItem &value : row)
        {
            result += value.isCoating() ? "true " : "false ";
        }
        result += "\n";
    }

    // Добавляем информацию о ядровой ДНФ
    result += "coreDnf:\n";
    result += coreDnf.join(", ") + "\n";

    // Добавляем информацию о всех возможных наборах значений вариативной части
    result += "allVariativePart:\n";
    for (const QStringList& list : allVariativePart)
    {
        result += list.join(", ") + "\n";
    }

    // Добавляем информацию об оптимальных наборах значений вариативной части
    result += "optimalVariativePart:\n";
    for (const QStringList& list : optimalVariativePart)
    {
        result += list.join(", ") + "\n";
    }

    // Добавляем информацию о результирующих минимализированных наборах
    result += "results:\n";
    for (const QStringList& list : results)
    {
        result += list.join(", ") + "\n";
    }

    return result;
}

const QString &MmkData::getFunc16() const
{
    return func16;
}

const QString &MmkData::getFunc2() const
{
    return func2;
}

const QStringList &MmkData::getTruthTable() const
{
    return truthTable;
}

MmkData::MmkType MmkData::getMmkType() const
{
    return mmkType;
}

const QStringList &MmkData::getValuesTable() const
{
    return valuesTable;
}

const CoverageMap &MmkData::getCoverageMap() const
{
    return coverageMap;
}

const QStringList &MmkData::getCoreDnf() const
{
    return coreDnf;
}

const QList<QStringList> &MmkData::getAllVariativePart() const
{
    return allVariativePart;
}

const QList<QStringList> &MmkData::getOptimalVariativePart() const
{
    return optimalVariativePart;
}
