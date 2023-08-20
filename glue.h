#ifndef GLUE_H
#define GLUE_H

#include <QStringList>



class Glue  // склейка
{
public:
    Glue();

    const QStringList &getGluePart() const;
    void setGluePart(const QStringList &newGluePart);
    const QStringList &getConstPart() const;
    void setConstPart(const QStringList &newConstPart);

    const QStringList getAllGluePart() const;   // получить оющий список значений

private:
    QStringList gluePart;   // результат склейки
    QStringList constPart;  // константная часть (не применялась в склейке)
};

#endif // GLUE_H
