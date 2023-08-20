#ifndef MMKCALCULATOR_H
#define MMKCALCULATOR_H

#include "coveragemap.h"
#include "mmkdata.h"
#include "splitter.h"
#include <math.h>
#include <QMap>

class MmkCalculator
{
public:
    MmkCalculator();

    bool calculateFunction(QString func16, MmkData::MmkType type, MmkData &result);    // рассчитать минимизацию функции

    QString getFormulaByResult(QStringList result, MmkData::MmkType type);     // конвертирует набор значений ДНФ в формулу

private:
    bool to2ssch(QString number16, QString &number2); // перевод из 16 в 2ю
    bool to16ssch(QString number2, QString &number16); // перевод из 2 в 16ю

    QStringList getTruthTable(QString &function2);  // получить таблицу истинности для значения
    QStringList getValuesTable(QString &function16, QStringList truthTable, MmkData::MmkType &type);  // получить список значений для данного типа минимизации (все равны 0 или все равны 1)

    void createGlue(QStringList numbersList, QStringList &skleykiList, bool *wasSkleyka = nullptr);  // функция создания склеек
    void createAllGlues(Glue lastGlue, Glue &nextGlue);      // функция, которая создает все возможные склейки

    bool checkPokritie(QString &value, QString &skleyka);       // функция, проверяющая, покрывается ли данное значение данной склейкой
    CoverageMap calculateCoverageMap(QStringList &valuesList, QStringList &gluesList);   // функция рассчёта карты покрытия

    QStringList getCoreDnf(CoverageMap &coverageMap);   // составляет ядровую ДНФ
    QList<QStringList> getVariativePart(QStringList &coreDnf, CoverageMap &coverageMap);     // составляет наборы всех возможных вариативных частей ДНФ на основе карты покрытия и ядровой ДНФ
    QList<QStringList> getOptimalVariativePart(QList<QStringList> allVariativePart);     // составляет наборы только минимальных возможных вариативных частей на основе списка всех возможных вариативных частей
    QList<QStringList> getResultDnf(QStringList &coreDnf, QList<QStringList> &optimalVariativePart);    // формирует список вариантов итоговой функции

    bool checkLists(const QStringList& list1, const QStringList& list2);    // если все элементы обоих списков содержатся в каждом из них, возвращает true
};

#endif // MMKCALCULATOR_H
