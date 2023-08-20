#include "mmkcalculator.h"

MmkCalculator::MmkCalculator()
{

}

bool MmkCalculator::calculateFunction(QString func16, MmkData::MmkType type, MmkData &result)
{
    // базовые проверки
    if(func16.size() != 4)  // если в функции не 4 переменных
    {
        return false;
    }

    // все символы - от A до F или от 0 до 9
    func16 = func16.toUpper();  // на всякий случай переволим всё в верхний регистр

    // перебираем каждый символ в строке
    for (const QChar& ch : qAsConst(func16))
    {
        // проверяем, что символ является буквой от A до F
        if (!((ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9')))
        {
            return false;  // если символ не удовлетворяет условию, возвращаем false
        }
    }

    // переводим в 2 ссч
    QString num2;
    bool ok = to2ssch(func16, num2);
    if(!ok)
    {
        return false;
    }

    // считаем таблицу истинности
    QStringList truthTable = getTruthTable(num2);

    // получаем значения для данного типа минимизации
    QStringList valuesTable = getValuesTable(num2, truthTable, type);
    if(valuesTable.isEmpty())   // если не удалось получить значения
    {
        return false;
    }

    // считаем склейки
    Glue glue[3];    // 3 этапа склеек
    Glue startGlue;     // первая, пустая склейка, в ней все исходные знаечния записываем как значения для создания склейки
    startGlue.setGluePart(valuesTable);     // задаем стартовые значения
    createAllGlues(startGlue, glue[0]);     // первый этап
    createAllGlues(glue[0], glue[1]);     // второй этап
    createAllGlues(glue[1], glue[2]);     // третий этап

    // добавляем все этапы склеек в список
    QList<Glue> glueList;
    glueList.append(glue[0]);
    glueList.append(glue[1]);
    glueList.append(glue[2]);

    // считаем карту покрытия
    QStringList resultGluesList = glue[2].getAllGluePart();     // получаем итоговый набор склеек
    CoverageMap coverageMap = calculateCoverageMap(valuesTable, resultGluesList);   // считаем карту покрытия

    // считаем ДНФ
    QStringList coreDnf = getCoreDnf(coverageMap);      // считаем ядровую ДНФ
    QList<QStringList> allVariativePart = getVariativePart(coreDnf, coverageMap);       // считаем все варианты вариативной части
    QList<QStringList> optimalVariativePart = getOptimalVariativePart(allVariativePart);        // оставляем только оптимальные варианты
    QList<QStringList> results = getResultDnf(coreDnf, optimalVariativePart);       // получаем итоговый вариант всех ДНФ

    result = MmkData(func16, num2, truthTable, type, valuesTable, glueList, coverageMap, coreDnf, allVariativePart, optimalVariativePart, results);     // формируем результат
    return true;
}

QString MmkCalculator::getFormulaByResult(QStringList result, MmkData::MmkType type)
{
    const QString separatorIn = (type == MmkData::MmkType::MDNF ? "*" : "+");   // разделитель внутри скобок
    const QString separatorOut = (type == MmkData::MmkType::MDNF ? "+" : "*");   // разделитель между скобок

    // собираем формулу
    QString formula;
    for(const QString &value : result)     // перебор значений
    {
        if(!formula.isEmpty())      // если формула не пустая
        {
            formula.append(separatorOut);   // доабвляем внешний разделитель между скобками
        }
        formula.append("(");
        QString disunkt;    // формируемый дизъюнкт
        for(int i=0; i<value.size(); i++)     // перебор символов внутри значения
        {
            QString ch = value[i];      // символ
            if(ch == "0" || ch == "1")      // если символ значащий
            {
                if(!disunkt.isEmpty())  // если дизъюнкт не пустой
                {
                    disunkt.append(separatorIn);    // добавляем внутренний разделитель
                }
                if(ch.toInt() != type)  // если значение не совпадает с типом минимизации, значит нужна инверсия
                {
                    disunkt.append("!");    // доабавляем инферсию
                }
                char v = 'A' + i;   // смещаемся от A на нужное кол-во символов
                disunkt.append(v);  // добавляем символ
            }
        }
        formula.append(disunkt);    // доабвляем дизъюнкт в формулу
        formula.append(")");
    }

    return formula;
}

bool MmkCalculator::to2ssch(QString number16, QString &number2)
{
    int razryad = number16.count(); // получаем количество разрядов числа
    if(razryad == 0) // если пустая строка
    {
        return false;
    }
    bool ok; // флаг перевода
    int ch10 = number16.toInt(&ok, 16); // переводим число из 16й в 10ю ССЧ
    if(!ok) // если число не переводится выдаем ошибку
    {
        return false; // не переводится
    }
    QString ch2 = QString::number(ch10, 2); // переводим число из 10й в 2ю ССЧ
    int count = ch2.count(); // определяем количество знаков
    int razryad2;
//    razryad2 = static_cast<int>(pow(2, razryad)); // получаем количество разрядов, которое должно быть в 2й записи числа
    razryad2 = 4 * razryad; // получаем количество разрядов, которое должно быть в 2й записи числа
    if(count < razryad2) // если количество разрядов после перевода меньше (int отбрасывает незначащие нули), то добавляем нули вперед до нужного количества
    {
        int k = razryad2 - count; // считаем количество нулей, котрое надо добавить
        for (int i=0; i<k; i++)
        {
            ch2 = "0" + ch2; // добавляем нули
        }
    }
    number2 = ch2; // возвращаем переведенное число
    return true; // перевод успешный
}

bool MmkCalculator::to16ssch(QString number2, QString &number16)
{
    if(number2 == "")
    {
        return false;
    }
    bool ok = false;
    int num10 = number2.toInt(&ok, 2); // переводим в 10 ССЧ
    if(!ok)
    {
        return false;
    }
    number16 = QString::number(num10, 16);
    return true;
}

QStringList MmkCalculator::getTruthTable(QString &function2)
{
    int razryad = function2.count(); // поличество разрядов в исходном числе
    if(razryad == 0)
    {
        return QStringList();
    }
    // заносим в таблицу нашу функцию
    // определяем, сколько переменных нужно, чтобы описать функцию
    QStringList funcList = function2.split("", SPLITTER); // разбиваем число в 2й ССЧ на символы
//    int rowsTot = static_cast<int>(razryad); // получаем количество строк в таблице
    double stepen = log2(razryad); // получаем, в какую степень над овозвести число 2, чтобы получить такую длину числа
    if(abs(stepen - static_cast<double>(static_cast<int>(stepen))) > 0.0) // если степень не целая
    {
        stepen = static_cast<double>(static_cast<int>(stepen)); // откидываем дробную часть
        stepen += 1.0; // прибавляем 1
    }
    int colsTot = static_cast<int>(stepen) + 1;// количство столбцов равно степени в которую надо возвезти 2, чтобы получить количество строк + 1 для функции
    QStringList sschTableList;
    for (int i=0; i<razryad; i++)
    {
        QString num = QString::number(i, 2); // перевод из 10 в 2
        while (num.count() < colsTot-1) // пока количество разрядов меньше, чем количество колонок -1, т.к. последняя колонка это значение функции
        {
            num = "0" + num;
        }
        sschTableList.append(num);
    }
    for (int i=0; i<sschTableList.size(); i++)
    {
        sschTableList[i] += funcList[i]; // добавляем значение функции к элементу таблицы истинности
    }
    return sschTableList;
}

QStringList MmkCalculator::getValuesTable(QString &ch2, QStringList truthTable, MmkData::MmkType &type)
{
    QStringList funcList = ch2.split("", SPLITTER); // разбиваем число в 2й ССЧ на символы

    // составляем таблицу значений
    QStringList values;
    for (int i=0; i<truthTable.size(); i++)
    {
        if(funcList[i].toInt() == type)
        {
            int valueSize = truthTable[i].size();   // кол-во символов в строке
            QString value = truthTable[i].remove(valueSize-1, 1);  // удаляем последнее значение, т.к. оно является значением функции
            values.append(value);  // добавляем значение функции к списку подходящих
        }
    }
    return values;
}

void MmkCalculator::createGlue(QStringList numbersList, QStringList &skleykiList, bool *wasSkleyka)
{
    bool wasSkleykaTemp = false;
    QStringList listFunc = numbersList; // объявляем для удобства
    int sizeFunc = listFunc.size(); // получаем размер списка чисел
    // первичные проверки
    if(sizeFunc <= 0)
    {
        return;
    }
    int razryad = listFunc[0].count(); // получаем количество символов в одном числе
    if(razryad <= 0)
    {
        return;
    }
    for (int i=0; i<sizeFunc; i++)
    {
        if(numbersList[i].count() != razryad) // если где-то есть пустоты, значит данные ошибочны
        {
            return;
        }
    }
    // разбиваем на разряды для поиска тех, которые можно склеить
    QString **elementsOnes = nullptr;
    try
    {
        elementsOnes = new QString *[sizeFunc]; // создаём двумерный массив для хранения разрядов
    }
    catch (std::exception &e)
    {
        return;
    }
    for (int i=0; i<sizeFunc; i++)
    {
        try
        {
            elementsOnes[i] = new QString [razryad];
        }
        catch (std::exception &e)
        {
            return;
        }
    }
    for (int i=0; i<sizeFunc; i++)
    {
        QString chislo = listFunc[i]; // получаем число
        QStringList chisloList = chislo.split("", SPLITTER);  // разбиваем число на символы
        int size = chisloList.size(); // получаем размер числа (количество символов)
        if(razryad != size) // если количество разрядов не совпадает, проверяем на всякий случай
        {
            for(int row=0; row<sizeFunc; row++)
            {
                delete [] elementsOnes[row];
            }
            delete [] elementsOnes;
            return;
        }
        for (int j=0; j<razryad; j++)
        {
            elementsOnes[i][j] = chisloList[j];   // сохраняем каждое число по символам
        }
    }
    // проверяем строки и создаем склейки
    QStringList skleyki1; // будем сохранять склейки
    bool *proshliSkleiku = nullptr;
    try
    {
        proshliSkleiku = new bool [sizeFunc]; // создаем массив, который будет хрнатиь информацию о том, прошли ли склейку элементы, размер равен количеству элементов
    }
    catch (std::exception &e)
    {
        return;
    }
    for (int i=0; i<sizeFunc; i++)
    {
        proshliSkleiku[i] = false; // задаём, что никто не прошел склейку
    }
    for (int i=0; i<sizeFunc; i++) // проходим по каждому значению
    {
        for (int j=0; j<i; j++) // проходим по каждому разряду каждого значения
        {
            bool *ok = nullptr;
            try
            {
                ok = new bool [razryad];
            }
            catch (std::exception &e)
            {
                return;
            }
            int count = 0; // сохраняет количество элементов, которые отличаются
            bool correctCompare = true; // флаг корректности сравнения. Некореектное, когда различающиеся символы, это X и число
            for (int k=0; k<razryad; k++)
            {
                ok[k] = (elementsOnes[i][k] == elementsOnes[j][k]); // сохраняет, равны-ли два элемента
                count += static_cast<int>(!ok[k]); // прибавляем 0 или 1 в зависимости от того, равны элеменнты, или нет
                if((elementsOnes[i][k] != elementsOnes[j][k]) && (elementsOnes[i][k] == "X" || elementsOnes[j][k] == "X")) // если различающиеся разряды это Х
                {
                    correctCompare = false;
                    break;
                }
            }

            if(count == 1 && (!proshliSkleiku[i] || !proshliSkleiku[j]) && correctCompare) // если разница только в одном разряде и если хотя бы один не проходил склейку до этого
            {
                proshliSkleiku[i] = true; // задаем, что 2 элемента прошли склейку
                proshliSkleiku[j] = true;
                QString skleyka; // для формирования склейки по символам
                for (int g=0; g<razryad; g++) // проверяем каждый разряд
                {
                    if(ok[g]) // если разряды совпадают
                    {
                        skleyka.append(elementsOnes[i][g]); // добавляем константу в склейку
                    }
                    else
                    {
                        skleyka.append("X"); // добавляем Х где разряд меняется
                    }
                }
                skleyki1.append(skleyka); // добавляем склейку в список
            }
            delete [] ok;
            ok = nullptr;
        }
    }

    for(int row=0; row<sizeFunc; row++)
    {
        delete [] elementsOnes[row];
    }
    delete [] elementsOnes;

    if(skleyki1.isEmpty()) // если не было склеек
    {
        skleykiList = numbersList; // новая склейка = старая склейка
        wasSkleykaTemp = false; // ставим, что не было склейки
        if(wasSkleyka != nullptr)
        {
            *wasSkleyka = wasSkleykaTemp;
        }
        delete [] proshliSkleiku;
        return; // выходим из функции, т.к. дальше обрабатывать нет смысла
    }
    wasSkleykaTemp = true; // ставим, склейка произошла
    for (int i=0; i<sizeFunc; i++)
    {
        if(!proshliSkleiku[i]) // если какой-то элемент не прошёл склейку
        {
            skleyki1.append(listFunc[i]); // добавляем число как константу
        }
    }

    delete [] proshliSkleiku;

    // удаляем повторяющиеся
    int skleyki1Size = skleyki1.size();
    for (int i=0; i < skleyki1Size; i++)
    {
        bool povtor = false; // определяет, повторяется ли элемент
        for (int j=0; j<i; j++)
        {
            if(skleyki1[i] == skleyki1[j])
            {
                povtor = true; // повторяется
            }
        }
        if(!povtor)
        {
            skleykiList.append(skleyki1[i]); // если элемент списка не повторяется, то добавляем его
        }
    }

    if(wasSkleyka != nullptr)
    {
        *wasSkleyka = wasSkleykaTemp;
    }
}

void MmkCalculator::createAllGlues(Glue lastGlue, Glue &nextGlue)
{
    QStringList passedGluing;   // список прошедших склейку
    QStringList gluesList;      // список созданных склеек
    for(const QString &value1 : lastGlue.getGluePart())    // перебираем все значения
    {
        for(const QString &value2 : lastGlue.getGluePart())    // перебираем все значения для сравнения
        {
            if(value1 != value2)    // если значения разные
            {
                bool ok = false;
                QStringList out;
                createGlue(QStringList() << value1 << value2, out, &ok);    // пробуем создать склейку
                if(ok)  // если склейка создана
                {
                    gluesList.append(out);    // добавляем получившуюся склейку в список
                    passedGluing.append(value1);    // добавляем значения, прошедшие склейку
                    passedGluing.append(value2);    // добавляем значения, прошедшие склейку
                }
            }
        }
    }

    // удаляем дубликаты
    passedGluing.removeDuplicates();
    gluesList.removeDuplicates();

    // ищем значения, которые не прошли склейку
    QStringList constPart;      // константная часть
    for(const QString &value : lastGlue.getGluePart())    // перебираем все значения
    {
        if(!passedGluing.contains(value))   // если значение не прошло склейку
        {
            constPart.append(value);  // добавляем значение без склейки
        }
    }

    // добавляем константные значения из прошлоой склейки
    constPart.append(lastGlue.getConstPart());

    // удаляем дубликаты в списке не прошедших склейку
    constPart.removeDuplicates();

    // записываем результат
    nextGlue.setGluePart(gluesList);    // элементы, прошедшие склейку
    nextGlue.setConstPart(constPart);   // элементы не прошедшие склейку
}

bool MmkCalculator::checkPokritie(QString &value, QString &skleyka)
{
    for (int k=0; k<skleyka; k++)      // проходим по каждому символу
    {
        if(skleyka[k] != value[k] && skleyka[k].toUpper() != "X")   // сравниваем каждый символ, если символ отличается и он не Х
        {
            return false;
            break;
        }
    }
    return true;
}

CoverageMap MmkCalculator::calculateCoverageMap(QStringList &valuesList, QStringList &gluesList)
{
    CoverageMap coverageMap;   // карты покрытия

    coverageMap.setHorizontalHeaders(valuesList);     // значения
    coverageMap.setVerticalHeaders(gluesList);        // склейки

    QVector<QVector<CoverageMapItem>> coating;    // карта покрытия (строим двумерный массив покрытия значений)
    for (int i=0; i<coverageMap.getVerticalHeaders().size(); i++)
    {
        QVector<CoverageMapItem> line;   // строка карты покрытия
        QString verticalValue = coverageMap.getVerticalHeaders().at(i);    // значение склеек, с которым сравниваем
        for (int j=0; j<coverageMap.getHorizontalHeaders().size(); j++)
        {
            QString horizontalValue = coverageMap.getHorizontalHeaders().at(j);    // значение, с которым сравниваем склейку

            bool correctValue = checkPokritie(horizontalValue, verticalValue);  // проверяем, покрывается ли значение склейкой

            CoverageMapItem mapItem;    // создаем ячейку карты
            mapItem.setValue(horizontalValue);      // покрываемое значение
            mapItem.setGlue(verticalValue);     // используемая склейка
            mapItem.setCoating(correctValue);   // ставим, покрывается или нет

            line.append(mapItem);  // добавляем, элемент карты
        }
        coating.append(line);  // добавляем строку карты
    }

    // ищем ядра
    for (int j=0; j<coverageMap.getHorizontalHeaders().size(); j++)
    {
        int plusCount = 0;  // кол-во перекрытий в данной колонке
        int position = -1;  // позиция строки, в которой найдено последнее перекрытие
        for (int i=0; i<coverageMap.getVerticalHeaders().size(); i++)
        {
            if(coating.at(i).at(j).isCoating())  // если значение покрыто
            {
                position = i;
                plusCount++;
            }
        }

        if(plusCount == 1)  // если в столбце только одно перекрытие
        {
            coating[position][j].setCore(true);     // ставим, что это значение является ядром
        }
    }

    coverageMap.setCoating(coating);  // карта покрытия

    return coverageMap;
}

QStringList MmkCalculator::getCoreDnf(CoverageMap &coverageMap)
{
    // ищем значения, которые составляют ядровую ДНФ
    QStringList coreValues; // список значений, которые являются ядрами

    for (int i=0; i<coverageMap.getVerticalHeaders().size(); i++)
    {
        for (int j=0; j<coverageMap.getHorizontalHeaders().size(); j++)
        {
            CoverageMapItem mapItem = coverageMap.getCoating().at(i).at(j);    // ячейка карты
            if(mapItem.isCore())    // если ячейка является ядром
            {
                coreValues.append(mapItem.getGlue());   // доабвляем значение склейки в таблицу
            }
        }
    }

    coreValues.removeDuplicates();  // удаляем дубликаты

    return coreValues;
}

QList<QStringList> MmkCalculator::getVariativePart(QStringList &coreDnf, CoverageMap &coverageMap)
{
    // ищем значения, которые не покрыты ядрами
    QStringList variativeValues = coverageMap.getHorizontalHeaders();    // список значений, которые не покрыты ядрами

    // идём по всем ядровым значениям и удаляем покрытые ими
    for (int i=0; i<coverageMap.getVerticalHeaders().size(); i++)
    {
        if(coreDnf.contains(coverageMap.getVerticalHeaders().at(i)))      // если это значение является ядром
        {
            for (int j=0; j<coverageMap.getHorizontalHeaders().size(); j++)
            {
                if(coverageMap.getCoating().at(i).at(j).isCoating())  // если значение покрывается ядром
                {
                    variativeValues.removeAll(coverageMap.getHorizontalHeaders().at(j));   // убираем значение из списка непокрытых
                }
            }
        }
    }

    if(variativeValues.size() == 0)     // если нет вариативной части
    {
        return QList<QStringList>();    // возвращаем пустой список
    }

    // составим список склеек, которые могут покрыть непокрытые значения
    QStringList variativeSkleyki;
    QStringList skleykiList = coverageMap.getVerticalHeaders();     // список доступных для использования склеек
    for (QString &core : coreDnf)
    {
        skleykiList.removeAll(core);    // удаляем ядровые значения из списка - их нельзя применять
    }

    // ищём склейки, которые могут покрыть недостающие значения
    for (QString &value : variativeValues)   // берем все непокрытые значения
    {
        for(QString skleyka : coverageMap.getVerticalHeaders())    // берём каждую склейку
        {
            if(checkPokritie(value, skleyka))   // если непокрытое знаячение покрывается данной склейкой
            {
                variativeSkleyki.append(skleyka);   // добавляем склейку в список
            }
        }
    }
    variativeSkleyki.removeDuplicates();    // удаляем дубликаты

    /* Берем все значения
     * Составляем map со списком склеек, которые могут покрыть это значение
     * На основе каждого значения формируем списки вариативной части
     */

    // cоставляем map со списком склеек, которые могут покрыть это значение
    QMap<QString, QStringList> variativeValuesPokritieVariants;     // вовзможные варианты покрытия значений

    for (QString &variativeValue : variativeValues)   // берем все непокрытые значения
    {
        QStringList pokritieVariants;   // список вариантов склеек для покрытия этого значения
        for(QString &skleyka : variativeSkleyki)    // берём каждую вариативную склейку
        {
            if(checkPokritie(variativeValue, skleyka))   // если данное значение покрывается склейкой
            {
                pokritieVariants.append(skleyka);   // доабвляем склейку к списку покрывающих
            }
        }
        variativeValuesPokritieVariants.insert(variativeValue, pokritieVariants);   // добавляем значение и варианты его покрытия
    }

    // составляем списки возможных вариантов значений
    QList<QStringList> variativeVariantsList;   // список состоящий из всех возможных вариантов вариативной части

    for(QString &variativeValue : variativeValuesPokritieVariants.keys())   // делаем перебор всех значений непокрытой вариативной части
    {
        if(variativeVariantsList.isEmpty())     // если сейчас в списке нет ни одного значения
        {
            // добавляем все склейки, покрывающие первое значение
            for(const QString &pokritieVariant : variativeValuesPokritieVariants.value(variativeValue))     // перебор всех склеек, покрывающих данное значение
            {
                variativeVariantsList.append(QStringList() << pokritieVariant);     // добавляем каждую склейку как отдельный вариант
            }
        }
        else    // если в списке уже есть варианты
        {
            // копия нужна для того, чтобы не мешать основному списку добавлением элементовв него
            QList<QStringList> variativeVariantsListCopy = variativeVariantsList;   // копия списка, состоящего из всех возможных вариантов вариативной части

            for(QStringList &variativeVariants : variativeVariantsListCopy)      // перебираем все уже имеющиеся наборы
            {
                bool valueClosedInThisList = false;     // флаг покрытия этого вариативного значения хоть одной склейкой из текущего набора
                for(QString &variativeVariant : variativeVariants)   // перебираем все склейки текущего варианта
                {
                    if(checkPokritie(variativeValue, variativeVariant))     // если значение покрывается
                    {
                        valueClosedInThisList = true;   // ставим, что значение покрыто
                        break;
                    }
                }

                // если значение уже покрыто, то ничего нек делаем
                // если значение не покрыто, то надо удаляем из списка вариантов текущий, доабвляем в него все возможные варианты склейи и возвращаем обратно

                if(!valueClosedInThisList)      // если значение не покрыто
                {
                    QStringList variativeVariantsCopy = variativeVariants;      // берем текущий список
                    variativeVariantsList.removeOne(variativeVariantsCopy);     // удаляем текущий набор

                    for(const QString &skleyka : variativeValuesPokritieVariants.value(variativeValue))    // делаем перебор всех значений, которые покрывают данное значение
                    {
                        QStringList variativeVariantsVariant = variativeVariantsCopy;   // берем текущий список
                        variativeVariantsVariant.append(skleyka);   // добавляем в него текущую склейку, покрывающую это значение
                        variativeVariantsList.append(variativeVariantsVariant);     // добавляем обратно в список вариантов, но уже с новой склейкой
                    }
                }
            }
        }
    }

    // удаляем дубликаты
    auto removeDuplicates = [&](QList<QStringList>& list)
    {
        // создаем список индексов элементов, которые будут удалены
        QList<int> indexesToRemove;

        // проходим по всем парам списков внутри QList
        for (int i = 0; i < list.size(); i++)
        {
            for (int j = i + 1; j < list.size(); j++)
            {
                // проверяем, содержатся ли все элементы текущих списков друг в друге
                if (checkLists(list[i], list[j]))
                {
                    indexesToRemove.append(j);
                }
            }
        }

        // удаляем списки по индексам
        std::sort(indexesToRemove.begin(), indexesToRemove.end(), std::greater<int>());
        for (int index : indexesToRemove)
        {
            list.removeAt(index);
        }
    };

    removeDuplicates(variativeVariantsList);    // удаляем дубликаты в списке

    return variativeVariantsList;
}

QList<QStringList> MmkCalculator::getOptimalVariativePart(QList<QStringList> allVariativePart)
{
    // ищем только самые минимальные варианты
    // сначала отсеиваем по кол-ву склеек, оставляем только с минимальным кол-вом

    int minSkleykiCount = INT_MAX;
    for (const QStringList &variativeList : qAsConst(allVariativePart))   // ищем минимальный размер
    {
        int size = variativeList.size();
        if(size < minSkleykiCount)
        {
            minSkleykiCount = size;
        }
    }

    // отсеиваем по кол-ву склеек
    for(int i=0; i<allVariativePart.size(); i++)
    {
        if(allVariativePart[i].size() > minSkleykiCount)   // если размер списка склеек больше минимального
        {
            allVariativePart.removeAt(i);  // удаляем из списка
            i--;
        }
    }

    return allVariativePart;
}

QList<QStringList> MmkCalculator::getResultDnf(QStringList &coreDnf, QList<QStringList> &optimalVariativePart)
{
    QList<QStringList> variants;    // список вариантов

    if(optimalVariativePart.isEmpty())      // если вариативная часть пустая
    {
        variants.append(coreDnf);   // добавляем только ядровую
    }
    else    // если есть вариативная часть
    {
        for (const QStringList &variativeList : qAsConst(optimalVariativePart))     // собираем все варианты
        {
            variants.append(coreDnf + variativeList);   // доабвляем ядровую часть к вариативной
        }
    }

    return variants;
}

bool MmkCalculator::checkLists(const QStringList &list1, const QStringList &list2)
{
    // перебираем все элементы первого списка
    for (const QString& item : list1)
    {
        // проверяем, содержится ли текущий элемент во втором списке
        if (!list2.contains(item))
        {
            return false; // Если элемент не найден, возвращаем false
        }
    }

    // перебираем все элементы второго списка
    for (const QString& item : list2)
    {
        // проверяем, содержится ли текущий элемент в первом списке
        if (!list1.contains(item))
        {
            return false; // Если элемент не найден, возвращаем false
        }
    }

    // если все элементы обоих списков содержатся в каждом из них, возвращаем true
    return true;
}

