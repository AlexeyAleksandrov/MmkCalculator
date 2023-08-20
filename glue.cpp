#include "glue.h"

Glue::Glue()
{

}

const QStringList &Glue::getGluePart() const
{
    return gluePart;
}

void Glue::setGluePart(const QStringList &newGluePart)
{
    gluePart = newGluePart;
}

const QStringList &Glue::getConstPart() const
{
    return constPart;
}

void Glue::setConstPart(const QStringList &newConstPart)
{
    constPart = newConstPart;
}

const QStringList Glue::getAllGluePart() const
{
    return gluePart + constPart;
}
