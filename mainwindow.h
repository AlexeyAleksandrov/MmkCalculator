#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidget>
#include "mmkcalculator.h"
#include "logiceditor.h"
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_calculate_clicked();
    void applyFormula(int i);   // функция устанавливающая формулу

    void setQStringListToTableWidget(QTableWidget* table, const QStringList& list);     // выводим строки в таблицу
    void centerTextInTable(QTableWidget *table); // центрировать текст в ячейках таблицы
    QString addSpaces(const QString& input);    // добавляет пробел после каждого 4 символа
    void adjustTableSize(QTableWidget* tableWidget);    // функция подгонки размера таблицы к размеру содержимого

    void on_comboBox_formulas_currentIndexChanged(int index);

private:
    QStringList formulas;    // список формул
    LogicEditor *logicEditor = nullptr;    // редактор формул

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
