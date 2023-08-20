#ifndef LOGICEDITOR_H
#define LOGICEDITOR_H

#define COLUMN_WIDTH 12     // 20
#define ROW_HEIGH_INV 3     // 5
#define ROW_HEIGH_VAR 20    // 35
#define START_ROW_COUNT 2

#include <QObject>
#include <QTableWidget>
#include <QTableWidgetItem>

class LogicEditor
{
public:
    LogicEditor(QTableWidget *parent);

    void addSymvol(QString symvol, bool inversiya = false); // фобавление символа (кроме инверсии)
    void formatTable(); // форматирует таблицу (устанавливает ширину и высоту строк и столбцов
    void clearTable();  // функция очистки таблицы
    void deleteLastSymvol(); // удаление последнего символа
    void addInv(); // добавляет инверсию
    void addInvLine(); // функция добавления строки инверсии *
    void delInvLine(); // функция удаления строки инверсии *
    QString getFormulaText(); // возвращает текстовый вид формулы
    bool isAllowSymvol(QString symvol); // возвращает, является-ли символ разрешённым
//    int getColChosedItem(); // возвращает номер столбца выделенного элемента. Вернёт -1, если выбрано более 1 элемента. Вернёт номер последнего столбца, если не выбрано ничего
    int insertColumnAftreSelected(QTableWidget *tableWidgetInput); // вставляет столбец после выбранного
    bool deleteSelectedColumns(QTableWidget *tableWidgetInput); // удаляет выделенные столбцы
    void getSelectedItemNumber(QTableWidget *tableWidgetInput, int &itemNumber, int *count = nullptr);

    void setTableWidget(QTableWidget *value); // получает указатель на tableWidget, в котором будет строиться выражение
    void setFormula(QString virazhenie); // устанавливает выражение в tableWidget, нет добавления инверсии над скобкой

    void setAutoInput(bool value);
    void setAutoInputType(int value); // тип автоввода (МДНФ/МКНФ)

    void setEditChoseItem(bool value);
    bool isEditChoseItem() const;

signals:
    void onStatusChanged(); // вызывается при каждом действии

private:
    QTableWidget *formulaTableWidget = nullptr; // указатель на главный tableWidget
    QStringList allowSymvolsList; // список разрешённых символов
    int getNumberEmptyItem(); // функция, возвращающающая номер пустой ячейки
    void scrollToLastSymvol(QTableWidget *tableWidgetInput); // прокручивает tableWidget до крайнего нижнего правого item
    bool autoInput = false; // флаг автоматического ввода для МКНФ/МДНФ
    int autoInputType = 0; // тип автоматического ввода (0 - МКНФ/ 1 - МДНФ)
    bool editChoseItem = false; // флаг, для редактирования выбранного мышкой элемента (true), или последнего (false)


};

#endif // LOGICEDITOR_H
