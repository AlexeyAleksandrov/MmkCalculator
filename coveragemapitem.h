#ifndef COVERAGEMAPITEM_H
#define COVERAGEMAPITEM_H

#include <QString>



class CoverageMapItem
{
public:
    CoverageMapItem();

    const QString &getValue() const;
    void setValue(const QString &newValue);
    const QString &getGlue() const;
    void setGlue(const QString &newGlue);
    bool isCoating() const;
    void setCoating(bool newCoating);
    bool isCore() const;
    void setCore(bool newCore);

private:
    QString value;  // значение, на пересечении с которым находится ячейка
    QString glue;   // склейка, на пересечении с которым находится ячейка
    bool coating = false;   // перекрытие ячейки
    bool core = false;  // является ли данное значение ядром
};

#endif // COVERAGEMAPITEM_H
