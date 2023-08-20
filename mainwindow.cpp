#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_f16->setText("7D73");
    ui->radioButton_MDNF->setChecked(true);

    logicEditor = new LogicEditor(ui->tableWidget_formula);     // инициируем редактор формул
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_calculate_clicked()
{
    QString f16 = ui->lineEdit_f16->text();     // 16е значение функции
    if(f16.isEmpty())   // пустая строка
    {
        QMessageBox::warning(this, "Error", "Введите функцию!");
        return;
    }

    if(f16.size() != 4)     // если не 4х значная функция
    {
        QMessageBox::warning(this, "Error", "Введите корректную функцию!");
        return;
    }

    int type = -1;      // тип минимизации
    if(ui->radioButton_MDNF->isChecked())
    {
        type = 1;
    }
    else if(ui->radioButton_MKNF->isChecked())
    {
        type = 0;
    }

    if(type == -1)  // не выбран тип минимизации
    {
        QMessageBox::warning(this, "Error", "Выберите тип минимизации!");
        return;
    }

    MmkCalculator mk;
    MmkData mnf;
    bool ok = mk.calculateFunction(f16, MmkData::MmkType(type), mnf);     // считаем минимизацию

    if(!ok)     // если невозможно рассчитать
    {
        QMessageBox::warning(this, "Error", "Функцию нельзя минимизировать данным способом!");
        return;
    }

    ui->lineEdit_f2->setText(addSpaces(mnf.getFunc2()));       // значение в 2 ссч

    setQStringListToTableWidget(ui->tableWidget_tt, mnf.getTruthTable());       // таблица истинности

    // горизонтальные заголовки таблицы истинности
    QStringList ttHorizontalHeaders;
    for(int i=0; i<ui->tableWidget_tt->columnCount()-1; i++)    // буквенные переменные
    {
        ttHorizontalHeaders.append(QString('A'+i));     // символ А со смещением
    }
    ttHorizontalHeaders.append("F");    // значение функции

    // вертикальные заголовки таблицы истинности
    QStringList ttVerticalHeaders;
    for(int i=0; i<ui->tableWidget_tt->rowCount(); i++)     // значения в 16 ССЧ
    {
        ttVerticalHeaders.append(QString::number(i, 16).toUpper());     // считаем значение в 16 ССЧ
    }

    ui->tableWidget_tt->setHorizontalHeaderLabels(ttHorizontalHeaders);     // горизонтальные заголовки
    ui->tableWidget_tt->setVerticalHeaderLabels(ttVerticalHeaders);     // вертикальные заголовки

    setQStringListToTableWidget(ui->tableWidget_values, mnf.getValuesTable());  // таблица со значениями

    // склейки
    setQStringListToTableWidget(ui->tableWidget_glue1, mnf.getGlueList().at(0).getGluePart());
    setQStringListToTableWidget(ui->tableWidget_glue2, mnf.getGlueList().at(1).getGluePart());
    setQStringListToTableWidget(ui->tableWidget_glue3, mnf.getGlueList().at(2).getGluePart());

    // выводим карту покрытия
    CoverageMap coverageMap = mnf.getCoverageMap();     // получаем рассчитанную карту покрытия
    ui->tableWidget_coverageMap->setRowCount(coverageMap.getVerticalHeaders().size());
    ui->tableWidget_coverageMap->setColumnCount(coverageMap.getHorizontalHeaders().size());
    ui->tableWidget_coverageMap->setVerticalHeaderLabels(coverageMap.getVerticalHeaders());
    ui->tableWidget_coverageMap->setHorizontalHeaderLabels(coverageMap.getHorizontalHeaders());

    for(int i=0; i<coverageMap.getCoating().size(); i++)     // строки
    {
        for(int j=0; j<coverageMap.getCoating().at(i).size(); j++)   // столбцы
        {
            QString text;   // текст ячейки
            if(coverageMap.getCoating().at(i).at(j).isCoating())
            {
                text = "+";
            }

            QTableWidgetItem *item = ui->tableWidget_coverageMap->item(i, j);   // ячейка
            if(item == nullptr)
            {
                item = new QTableWidgetItem;
                ui->tableWidget_coverageMap->setItem(i, j, item);
            }

            item->setText(text);
        }
    }

    // выделяем ядра функции
    QStringList cores = mnf.getCoreDnf();   // получаем ядровую ДНФ
    for(int i=0; i<coverageMap.getCoating().size(); i++)     // строки
    {
        QString value = coverageMap.getVerticalHeaders().at(i);      // значение в столбце (склейка)
        if(cores.contains(value))       // если значение является ядровым
        {
            for(int j=0; j<coverageMap.getCoating().at(i).size(); j++)   // столбцы
            {
                QTableWidgetItem *item = ui->tableWidget_coverageMap->item(i, j);   // ячейка
                if(item == nullptr)
                {
                    item = new QTableWidgetItem;
                    ui->tableWidget_coverageMap->setItem(i, j, item);
                }
            }
        }
    }

    // выравниваем
    ui->tableWidget_coverageMap->resizeColumnsToContents();
    ui->tableWidget_coverageMap->resizeRowsToContents();
    centerTextInTable(ui->tableWidget_coverageMap);

    // получаем формулы
    QList<QStringList> results = mnf.getResults();      // паолучаем набор значений ДНФ
    formulas.clear();   // очищаем старые формулы
    for (const QStringList &variant : results)     // перебираем все результаты
    {
        formulas.append(mk.getFormulaByResult(variant, mnf.getMmkType()));      // добавляем формулу в список, преобразовывая ДНФ в формулу
    }

    // задаем формулы в ComboBox
    ui->comboBox_formulas->clear();     // удаляем старые значения
    ui->comboBox_formulas->addItems(formulas);      // добавляем новые формулы

    if(!formulas.isEmpty())     // если есть варианты
    {
        applyFormula(0);    // применеям первую формулу
    }

    ui->label_resultsCount->setNum(formulas.size());    // задаем кол-во решений

//    qDebug() << "Ядровая: " << mk.getFormulaByResult(mnf.getCoreDnf(), mnf.getMmkType());
//    qDebug() << "Вариативная: " << mk.getFormulaByResult(mnf.getOptimalVariativePart().at(0), mnf.getMmkType());
}

void MainWindow::applyFormula(int i)
{
    logicEditor->clearTable();      // очищаем таблицу
    if(i >= 0 && i<formulas.size())     // проверяем границы
    {
        logicEditor->setFormula(formulas[i]);   // задаем формулу
        centerTextInTable(ui->tableWidget_formula);     // центрируем текст
    }
}

void MainWindow::setQStringListToTableWidget(QTableWidget *table, const QStringList &list)
{
    // Устанавливаем количество строк в таблице
    table->setRowCount(list.size());

    // Для каждого элемента списка
    for (int i = 0; i < list.size(); ++i)
    {
        QString item = list.at(i);
        int itemLength = item.length();

        // Устанавливаем количество столбцов для текущей строки
        table->setColumnCount(itemLength);

        // Разбиваем элемент списка на символы и заполняем ячейки таблицы
        for (int j = 0; j < itemLength; ++j)
        {
            QTableWidgetItem* cellItem = new QTableWidgetItem;
            cellItem->setText(QString(item[j]));
            table->setItem(i, j, cellItem);
        }
    }

    // выравниваем
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    centerTextInTable(table);
}

void MainWindow::centerTextInTable(QTableWidget *table)
{
    if(table == nullptr)
    {
        return;
    }
    int rows = table->rowCount(); // получили количество строк
    int cols = table->columnCount(); // получаем количество столбцов
    if(rows == 0 || cols == 0)
    {
        return;
    }
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            if(table->item(i, j) != nullptr)
            {
                table->item(i, j)->setTextAlignment(Qt::AlignCenter); // выравниаем все элементы по центру
            }
        }
    }
}

QString MainWindow::addSpaces(const QString &input)
{
    QString result;

    // Для каждого символа во входной строке
    for (int i = 0; i < input.size(); ++i)
    {
        // Добавляем текущий символ к результату
        result += input[i];

        // Если текущий индекс делится на 4 без остатка и это не последний символ
        if ((i + 1) % 4 == 0 && i != input.size() - 1)
        {
            // Добавляем пробел
            result += " ";
        }
    }

    return result;
}

void MainWindow::adjustTableSize(QTableWidget *tableWidget)
{
    // Вызываем метод updateGeometry(), чтобы убедиться, что размеры таблицы корректно обновились
    tableWidget->updateGeometry();

    int rowCount = tableWidget->rowCount();
    int columnCount = tableWidget->columnCount();

    // Включаем режим масштабирования таблицы
    tableWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // Отключаем горизонтальную и вертикальную прокрутку
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Вычисляем ширину и высоту содержимого таблицы
    int totalWidth = tableWidget->verticalHeader()->width();
    int totalHeight = tableWidget->horizontalHeader()->height();

    for (int i = 0; i < columnCount; ++i)
    {
        totalWidth += tableWidget->columnWidth(i);
    }

    for (int i = 0; i < rowCount; ++i)
    {
        totalHeight += tableWidget->rowHeight(i);
    }

    // Вычисляем ширину и высоту видимой области таблицы
    int viewportWidth = tableWidget->viewport()->width();
    int viewportHeight = tableWidget->viewport()->height();

    // Изменяем ширину и высоту таблицы с учетом видимой области
    tableWidget->setFixedSize(qMin(totalWidth, viewportWidth), qMin(totalHeight, viewportHeight));
//    QRect geom = tableWidget->geometry();
//    geom.setWidth(qMin(totalWidth, viewportWidth));
//    geom.setHeight(qMin(totalHeight, viewportHeight));
//    tableWidget->setGeometry(geom);

    // Восстанавливаем режим масштабирования и политику прокрутки таблицы
    tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}


void MainWindow::on_comboBox_formulas_currentIndexChanged(int index)
{
    applyFormula(index);    // применяем формулу
}

