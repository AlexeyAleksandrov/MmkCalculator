#include "logiceditor.h"
#include "qdebug.h"

#define AUTOINPUT_MKNF_SYMVOL "+" /**< Символ операции МКНФ */
#define AUTOINPUT_MDNF_SYMVOL "*" /**< Символ операции МДНФ */

LogicEditor::LogicEditor(QTableWidget *parent)
{
    setTableWidget(parent);
    allowSymvolsList << "(" << ")" << "+" << "v" << "*" << "^" << "=" << "!";

    /*!
       * \brief Конструктор класса LogicEditor
       *
       * \param parent Указатель на родительский QTableWidget
       */

      // Устанавливаем родительский QTableWidget
}


int LogicEditor::getNumberEmptyItem()
{
    if(!formulaTableWidget)
        return -1; // если указатель пустой

    /*!
         * \brief Получить номер пустой ячейки
         *
         * Возвращает номер пустой ячейки в таблице формулы,
         * в которую можно внести инверсию или символ операции
         *
         * \return Номер пустой ячейки или -1, если все заняты
         */

    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
    if(rows < 2)
        return -1; // если строчек меньше 2, т.е. некуда писать инверсии
    if(cols < 2)
        return -1; // еслт столбцов меньше 2, то выходим
    int emptyItemNumber = 0; // создаем переменную, в которую будем сохранять номер столбца, который свободен
    while (emptyItemNumber < cols)
    {
        if(formulaTableWidget->item(rows-1, emptyItemNumber) == nullptr)  // если элемент пустой
        {
            formulaTableWidget->setItem(rows-1, emptyItemNumber, new QTableWidgetItem("")); // выделяем память и возвращаем его номер
            return emptyItemNumber;
        }
        if(formulaTableWidget->item(rows-1, emptyItemNumber)->text() == "") // если ничего не написано
        {
            return emptyItemNumber;
        }
        emptyItemNumber++;
    }
    return -1; // если же все занято, то возвращаем ошибку
}

void LogicEditor::scrollToLastSymvol(QTableWidget *tableWidgetInput)
{
    if(tableWidgetInput == nullptr) // если указатель пустой
    {
        return;
    }

    /*!
        * \brief Прокрутить таблицу до последнего символа
        *
        * Прокручивает таблицу до последнего символа
        *
        * \param tableWidgetInput Указатель на таблицу
        */

    int rows = tableWidgetInput->rowCount(); // поулчаем количество строк
    int cols = tableWidgetInput->columnCount(); // получаем количество столбцов
    if(!rows || !cols) // если нет строк или нет столбцов
    {
        return;
    }
    tableWidgetInput->scrollToItem(tableWidgetInput->item(rows-1, cols-1)); // передвигнаем к нижнему правому item
}

bool LogicEditor::isEditChoseItem() const
{
    /*!
        * \brief Получить значение флага editChoseItem
        *
        * Возвращает значение флага editChoseItem
        *
        * \return Значение флага editChoseItem
        */
    return editChoseItem;
}

int LogicEditor::insertColumnAftreSelected(QTableWidget *tableWidgetInput)
{
    /*!
     * \brief Вставить столбец после выбранного
     *
     * Вставляет новый столбец в таблицу после выбранного столбца.
     * Если нет выбранного столбца, то добавляет столбец в конец таблицы.
     *
     * \param tableWidgetInput Указатель на таблицу
     * \return Номер вставленного столбца или -1, если произошла ошибка
     */

    int c = -1; // переменная для хранения номера выделенного столбца
    for (int i=0; i<tableWidgetInput->rowCount(); i++)
    {
        for (int j=0; j<tableWidgetInput->columnCount(); j++)
        {
           if(tableWidgetInput->item(i, j)->isSelected())
           {
               c = j;
           }
        }
    }
    if(c >= 0) // если найден выделенный столбец
    {
        c = c + 1; // новый столбец будет стоять справа от выделенного, поэтому +1
    }
    else
    {
        c = tableWidgetInput->columnCount(); // если не выбрано ни одного столбца, то добавляем в самый конец
    }
    tableWidgetInput->insertColumn(c); // добавляем столбец
    for (int i=0; i<tableWidgetInput->rowCount(); i++)
    {
        tableWidgetInput->setItem(i, c, new QTableWidgetItem("")); // выделяем память
    }
    for (int i=0; i<tableWidgetInput->rowCount(); i++)
    {
        for (int j=0; j<tableWidgetInput->columnCount(); j++)
        {
            tableWidgetInput->item(i, j)->setSelected(false); // снимаем выделение со всех элементов
        }
    }
    if(c != tableWidgetInput->columnCount()-1) // если был не последний столбец, то ставим выделение, чтобы сохранить позицию курсора
    {
        for (int i=0; i<tableWidgetInput->rowCount(); i++)
        {
            tableWidgetInput->item(i, c)->setSelected(true); // ставим выделение для только что добавленного элемента
        }
    }
    tableWidgetInput->scrollToItem(tableWidgetInput->item(tableWidgetInput->rowCount()-1, c)); // прокручиваем до выделенного элемента
    return c; // вызвращаем номер вставленного столбца
}

void LogicEditor::setEditChoseItem(bool value)
{
    /*!
     * \brief Установить состояние флага editChoseItem
     *
     * Устанавливает состояние флага editChoseItem.
     *
     * \param value Значение флага
     */
    editChoseItem = value;
}

void LogicEditor::setAutoInputType(int value)
{
    /*!
     * \brief Установить тип автоматического ввода
     *
     * Устанавливает тип автоматического ввода.
     *
     * \param value Тип автоматического ввода
     */
    autoInputType = value;
}

void LogicEditor::setAutoInput(bool value)
{
    /*!
    * \brief Установить флаг автоматического ввода
    *
    * Устанавливает состояние флага автоматического ввода.
    *
    * \param value Значение флага
    */
    autoInput = value;
}

/**
 * @brief Проверяет, является ли символ разрешенным символом.
 *
 * Функция проверяет, является ли переданный символ symvol разрешенным символом.
 *
 * @param symvol Строка - символ для проверки.
 *
 * @return true, если символ является разрешенным.
 * @return false, если символ не является разрешенным или formulaTableWidget равен nullptr.
 */
bool LogicEditor::isAllowSymvol(QString symvol)
{
    if(formulaTableWidget == nullptr)
    {
        return false;
    }
    int size = allowSymvolsList.size(); // получаем количество элементов в списке разрешенных символов
    symvol = symvol.toLower(); // переводим всё в нижний регистр
    if(((symvol >= "a") && (symvol <= "d")) || (symvol == "!")) // если символ от A до D
    {
        return true;
    }
    for (int i=0; i<size; i++)
    {
        if(symvol == allowSymvolsList[i]) // проверяем совпадаение с хотя бы одним символом
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Удаляет выделенные столбцы в таблице.
 *
 * Функция удаляет выделенные столбцы в переданной таблице tableWidgetInput.
 *
 * @param tableWidgetInput Указатель на QTableWidget - таблицу, в которой нужно удалить столбцы.
 *
 * @return true, если был удален хотя бы один столбец.
 * @return false, если не было выделено ни одного столбца.
 */
bool LogicEditor::deleteSelectedColumns(QTableWidget *tableWidgetInput)
{
    int c = -1; // переменная для хранения номера выделенного столбца
    for (int i=0; i<tableWidgetInput->rowCount(); i++)
    {
        for (int j=tableWidgetInput->columnCount()-1; j>=0; j--)
        {
           if(tableWidgetInput->item(i, j)->isSelected())
           {
                c = j;
                tableWidgetInput->removeColumn(j);
           }
        }
    }
    if(c >= 0) // выделяем изначально выделенный столбец
    {
        int colNum = c-1; // номер стобца будет равен предыдущему, перед тем, который удален
        for (int i=0; i<tableWidgetInput->rowCount(); i++)
        {
            if(tableWidgetInput->item(i, colNum) != nullptr)
            {
                tableWidgetInput->item(i, colNum)->setSelected(true); // ставим выделение для первого выделенного элемента
            }
        }
    }
    else // если изначально не было выделено ни одного столбца
    {
        int colsDel = tableWidgetInput->columnCount()-1;
        for (int i=0; i<tableWidgetInput->rowCount(); i++)
        {
            delete tableWidgetInput->item(i, colsDel); // удаляем память ячеек
        }
        tableWidgetInput->removeColumn(tableWidgetInput->columnCount()-1); // удаляем последний столбец
    }
    return (c != -1); // если с == -1, то значит не было выделено ни одного элемента и возвращаем false, иначе был удален хотя бы один элемент и возвращаем true
}

/**
 * @brief Возвращает номер выделенного столбца и количество выделенных столбцов в таблице.
 *
 * Функция возвращает номер выделенного столбца в переданной таблице tableWidgetInput
 * и подсчитывает количество выделенных столбцов, записывая его в переменную count,
 * если count не равно nullptr.
 *
 * @param tableWidgetInput Указатель на QTableWidget - таблицу, из которой нужно получить номер выделенного столбца и количество выделенных столбцов.
 * @param itemNumber Ссылка на int - переменную, в которую будет записан номер выделенного столбца.
 * @param count Указатель на int - переменную, в которую будет записано количество выделенных столбцов (если не равно nullptr).
 */
void LogicEditor::getSelectedItemNumber(QTableWidget *tableWidgetInput, int &itemNumber, int *count)
{
    itemNumber = -1; // переменная для хранения номера выделенного столбца
    if(count != nullptr)
    {
        count = 0;
    }
    for (int i=0; i<tableWidgetInput->rowCount(); i++)
    {
        for (int j=0; j<tableWidgetInput->columnCount(); j++)
        {
           if(tableWidgetInput->item(i, j)->isSelected())
           {
               itemNumber = j;
               if(count != nullptr)
               {
                   count++;
               }
           }
        }
    }
    if(itemNumber == -1)
    {
        itemNumber = tableWidgetInput->columnCount()-1; // если не выбрано ни одного столбца, то добавляем в самый конец
    }
}
//int logicEditor::getColChosedItem()
//{
//    if(formulaTableWidget == nullptr)
//    {
//        return -1;
//    }
//    int rows = formulaTableWidget->rowCount(); // получаем количество строк
//    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
//    if((rows < 2) || (cols == 0))
//    {
//        return -1;
//    }
//    int number = -1; // будет хранить номер выбранного столбца
//    for (int i = 0; i < cols; ++i)
//    {
//        bool selected = false;
//        for (int j=0; j<rows; j++)
//        {
//            if(formulaTableWidget->item(j, i) != nullptr) // если элемент существует
//            {
//                if(formulaTableWidget->item(j, i)->isSelected()) // если элемент выбран
//                {
//                    selected = true; // устанавливаем флаг
//                    break;
//                }
//            }
//        }
//        if(selected) // если найден выбранный элемент
//        {
//            if(number != -1) // если сохранённый номер не -1, значит найден выделено 2 столбца
//            {
//                return -1;
//            }
//            else
//            {
//                number = i; // сохраняем номер столбца
//            }
//        }
//    }
//    if(number == -1) // если не было найдено ни одного выделенного элемента
//    {
//        number = cols-1; // приравниваем к последнему
//    }
//    return number;
//}

/**
 * @brief Добавляет символ в таблицу.
 *
 * Функция добавляет заданный символ в формулу в таблице formulaTableWidget.
 *
 * @param symvol Строка - добавляемый символ.
 * @param inversiya Флаг, указывающий, нужна ли инверсия над символом.
 */
void LogicEditor::addSymvol(QString symvol, bool inversiya)
{
    if(formulaTableWidget == nullptr)
    {
        return;
    }
    if(symvol == "")
    {
        return;
    }
    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
    if(rows < 2)
    {
        return; // если строчек меньше 2, т.е. некуда писать инверсии
    }
    int colNumber = -1; // номер столбца ,в который будет добавлен символ
    if(editChoseItem) // если включено редактирование выбранного элемента
    {
        colNumber = insertColumnAftreSelected(formulaTableWidget);
    }
    else
    {
        cols++; // увеличиваем количество колонок
        formulaTableWidget->setColumnCount(cols); // увеливаем количество колонок на 1, чтобы добавить символ
        rows = formulaTableWidget->rowCount(); // снова получаем количество строк, т.к. оно могло поменяться из-за рекурсии, вызванной автоматическим вводом
        cols = formulaTableWidget->columnCount(); // снова получаем количество столбцов
        colNumber = cols-1;
        qDebug() << "Номер столбюца, куда будет вставлен символ: " << colNumber << "количество столбцов: " << cols;
    }
    qDebug() << "переходим к добавлению символа";
    qDebug() << "строк: " << formulaTableWidget->rowCount();
    qDebug() << "столбцов: " << formulaTableWidget->columnCount();
    qDebug() << "номер строки, в которую ставим значение:" << rows-1;
    qDebug() << "номер столбца, в который ставим значение: " << colNumber;
    qDebug() << "символ: " << symvol;
    qDebug() << "инверсия: " << inversiya;
    if(colNumber == cols)
    {
        formulaTableWidget->setColumnCount(cols+1);
    }
    QTableWidgetItem *symvolItem = new QTableWidgetItem();
    symvolItem->setText(symvol);    // задаем текст ячейки
    symvolItem->setBackground(Qt::white); // устанавливаем цвет ячейки
    symvolItem->setForeground(Qt::black); // устанавливаем цвет текста
    formulaTableWidget->setItem(rows-1, colNumber, symvolItem); // добавляем символ
    if(inversiya) // если нужна инверсия над символом
    {
        formulaTableWidget->setItem(rows-2, colNumber, new QTableWidgetItem("")); // выделяем память под ячейку
        formulaTableWidget->item(rows-2, colNumber)->setBackground(Qt::black); // устанавливаем чёрный цвет для инверсии
    }
    formatTable(); // форматируем таблицу
    scrollToLastSymvol(formulaTableWidget); // перемещаемся к последнему символу
    emit onStatusChanged(); // отправляем сигнал, что что-то изменилось
}

/**
   * @brief Форматирует таблицу в редакторе логики.
   *
   * Если таблица не существует или не содержит ни одной строки, то создает 5 строк.
   * Если таблица содержит строки, но не содержит столбцов, то возвращает управление.
   * Для каждой ячейки таблицы:
   *   - Если ячейка пустая, выделяет память, устанавливает белый фон и шрифт Segoe UI размером 13.
   *   - Если ячейка не пустая, удаляет текст ячейки.
   * Устанавливает ширину столбцов и высоту строк.
   */
void LogicEditor::formatTable()
{
    if(formulaTableWidget == nullptr)
    {
        return;
    }
    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем количяество столбцов
    if(!rows)
    {
        formulaTableWidget->setRowCount(START_ROW_COUNT); // устаналиваем 5 строк
    }
    if(!cols) // если нет строк или столбцов
    {
        return;
    }
    for (int i=0; i<rows-1; i++)
    {
        for (int j=0; j<cols; j++)
        {
            if(formulaTableWidget->item(i, j) == nullptr) // если ячейка пустая
            {
                formulaTableWidget->setItem(i, j, new QTableWidgetItem("")); // выделяем память
                formulaTableWidget->item(i, j)->setBackground(Qt::white); // устанавливаем белый фон
                formulaTableWidget->item(i, j)->setFont(QFont("Segoe UI", 13, 100, 0));
            }
            else
            {
                formulaTableWidget->item(i, j)->setText(""); // просто удаляем текст
            }
            formulaTableWidget->setColumnWidth(j, COLUMN_WIDTH); // устаналиваем ширину
        }
        formulaTableWidget->setRowHeight(i, ROW_HEIGH_INV); // устанавливаем высоту
    }
    formulaTableWidget->setRowHeight(rows-1, ROW_HEIGH_VAR); // устанавливаем высоту
}

/**
    @brief Очищает таблицу редактора логики.
    Если указатель formulaTableWidget равен nullptr, ничего не делает.
    Иначе производит следующие действия:
    Получает количество строк и столбцов таблицы.
    Перебирает все ячейки таблицы и устанавливает пустую строку в каждую ячейку.
    Освобождает память, выделенную для каждой ячейки таблицы.
    Устанавливает количество строк и столбцов таблицы в 0.
    Форматирует таблицу заново.
    Отправляет сигнал о изменении статуса.
    */
void LogicEditor::clearTable()
{
    if(formulaTableWidget == nullptr)
    {
        return;
    }
    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            formulaTableWidget->setItem(i, j, new QTableWidgetItem(""));
            delete formulaTableWidget->item(i, j); // удаляем память
        }
    }
    formulaTableWidget->setRowCount(0); // устанавливаем 0 строк
    formulaTableWidget->setColumnCount(0); // устанавливаем 0 столбцов
    formatTable(); // так как при очистке форматирование сбросилоась, форматируем заново
    emit onStatusChanged(); // отправляем сигнал, что что-то изменилось
}

/**
    @brief Удаляет последний символ в таблице редактора логики.
    Если указатель formulaTableWidget равен nullptr, ничего не делает.
    Иначе производит следующие действия:
    Получает количество строк и столбцов таблицы.
    Если количество столбцов равно 0, ничего не делает.
    Если режим редактирования выбранных включен:
     - Удаляет выделенные столбцы из таблицы.
    В противном случае:
     - Устанавливает пустую строку в последнюю ячейку каждой строки.
     - Освобождает память, выделенную для последней ячейки каждой строки.
     - Уменьшает количество столбцов таблицы на 1.
    Форматирует таблицу заново.
    Отправляет сигнал о изменении статуса.
*/
void LogicEditor::deleteLastSymvol()
{
    if(formulaTableWidget == nullptr)
    {
        return;
    }
    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
    if(cols == 0)
    {
        return;
    }
    if(editChoseItem) // если редактирование выбранных
    {
        deleteSelectedColumns(formulaTableWidget); // удаляем выделенные столбцы
    }
    else
    {
        for (int i = 0; i < rows; ++i)
        {
            formulaTableWidget->setItem(i, cols-1, new QTableWidgetItem(""));
            delete formulaTableWidget->item(i, cols-1); // удаляем память
        }
        formulaTableWidget->setColumnCount(cols-1); // уменьшаем на 1 колонку
    }
    formatTable(); // так как при очистке форматирование сбросилоась, форматируем заново
    emit onStatusChanged(); // отправляем сигнал, что что-то изменилось
}

/**
    @brief Добавляет или удаляет инверсию для выделенных ячеек в таблице формул.
    Если таблица формул не инициализирована, функция ничего не делает.
    @details Функция проверяет количество строк и колонок в таблице формул. Если
    количество строк меньше 2 или количество колонок равно 0, функция завершает свою работу.
    Затем происходит итерация по ячейкам таблицы формул. Если ячейка существует и
    выделена, проверяется текущий цвет фона. Если цвет черный, это означает,
    что в ячейке уже есть инверсия, и она удаляется, устанавливая белый цвет фона.
    Если цвет не черный, то инверсия отсутствует, и она добавляется, устанавливая
    цвет фона ячейки в черный.
    По окончании процесса изменения цветов в ячейках, функция отправляет сигнал
    onStatusChanged() для указания, что произошли изменения.
*/
void LogicEditor::addInv()
{
    if(formulaTableWidget == nullptr)
    {
        return;
    }
    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
    if((rows < 2) || (cols == 0))
    {
        return;
    }
    for (int i = 0; i < rows-1; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if(formulaTableWidget->item(i, j) != nullptr) // если ячейка существует
            {
                if(formulaTableWidget->item(i, j)->isSelected()) // если выделен
                {
                    if(formulaTableWidget->item(i, j)->background() == Qt::black) // если текщий цвет черный, значит удаляем инверсию
                    {
                        formulaTableWidget->item(i, j)->setBackground(Qt::white); // удаляем инверсию ставя белый цвет
                    }
                    else
                    {
                        formulaTableWidget->item(i, j)->setBackground(Qt::black); // ставим инверсию - черный цвет
                    }
                }
            }
        }
    }
    emit onStatusChanged(); // отправляем сигнал, что что-то изменилось
}

void LogicEditor::addInvLine()
{
//    if(formulaTableWidget == nullptr)
//    {
//        return;
//    }
//    int rows = formulaTableWidget->rowCount(); // получаем количество строк
//    formulaTableWidget->setRowCount(rows+1); // добавляем строку
//    rows++;
//    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
//    for (int i = rows-2; i >= 0; --i) // идём в обратную сторону
//    {
//        for (int j = 0; j < cols; ++j)
//        {
//            if(formulaTableWidget->item(i, j) != nullptr) // если ячейка существует
//            {
//                formulaTableWidget->setItem()
//            }
//        }
    //    }
}

/**

@brief Возвращает текстовое представление формулы.
@details Если таблица формул пуста или содержит недостаточное количество строк и столбцов, возвращается строка "ERROR".
@return Текстовое представление формулы.
*/
QString LogicEditor::getFormulaText()
{
    if(formulaTableWidget == nullptr)
    {
        return "ERROR";
    }
    int rows = formulaTableWidget->rowCount(); // получаем количество строк
    int cols = formulaTableWidget->columnCount(); // получаем колдичество колонок
    if((cols == 0) || (rows < 2))
    {
        return "ERROR";
    }
    QString formula;
//    for (int i = 0; i < rows; ++i)
//    {
        for (int j = 0; j < cols; ++j)
        {
            QTableWidgetItem *item = formulaTableWidget->item(rows-1, j); // получаем ячейку
            if(item != nullptr) // если ячейка существует
            {
                QString symvol = item->text(); // получаем текст
                if(symvol != "")
                {
                    if(isAllowSymvol(symvol)) // если символ подходит
                    {
                        if(formulaTableWidget->item(rows-2, j)->background() == Qt::black) // если сверху стоит инверсия
                        {
                            formula.append("!");
                        }
                        symvol.replace("v", "+");
                        symvol.replace("^", "*");
                        symvol = symvol.toLower(); // переводим символ в нижний регистр
                        formula.append(symvol);
                    }
                }
            }
        }
//    }
    return formula;
}

/**
@brief Устанавливает таблицу формул.
@details Если передается нулевой указатель, функция ничего не выполняет.
@param value Указатель на QTableWidget.
*/
void LogicEditor::setTableWidget(QTableWidget *value)
{
    if(value == nullptr)
    {
        return;
    }
    formulaTableWidget = value;
    formulaTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // блоируем ввод
    formatTable();
}

/**
@brief Устанавливает формулу.
@details Удаляет пробелы из выражения и проверяет наличие символов.
Если выражение пустое, функция ничего не выполняет. Переводит символы в верхний регистр,
если они находятся в нижнем регистре.
Добавляет символы в таблицу формул и форматирует таблицу.
Отправляет сигнал onStatusChanged.
@param virazhenie Формула в виде строки.
*/
void LogicEditor::setFormula(QString virazhenie)
{
    virazhenie.remove(" "); // удаляем пробелы
    if(virazhenie.count() == 0) // если в выражении нет символов
    {
        return;
    }
    int count = virazhenie.count(); // получаем сколько символов выражении
    bool inversiya = false; // флаг для инверсии символа
    for (int i=0; i<count; i++)
    {
        QString symvol = virazhenie.at(i); // получаем символы
        if(!isAllowSymvol(symvol)) // если попался неизвестный символ
        {
            continue; // пропускаем этот символ
        }
        if((symvol >= "a") && (symvol <= "z")) // если символ в нижнем регистре
        {
           symvol = symvol.toUpper(); // переводим символ в верхний регистр
        }
        if(symvol == "!") // если это символ инверсии
        {
            inversiya = true; // задаём флаг инверсии
        }
        else // если это символ
        {
            if(symvol == "V") // если символ - дизъюнкция
            {
                symvol = symvol.toLower(); // переводим в нижний регистр
            }
//            symvol.replace("+", "v");
//            symvol.replace("*", "^");
            symvol.replace("v", "+");
            symvol.replace("^", "*");
            addSymvol(symvol, inversiya); // добавляем символ
            inversiya = false; // снимаем флаг инверсии
        }
    }
    formatTable(); // форматируем таблицу
    emit onStatusChanged(); // отправляем сигнал, что что-то изменилось
}

void LogicEditor::onStatusChanged(){} // просто сигнал, его надо было куда-то запихнуть
