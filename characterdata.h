#ifndef CHARACTERDATA_H
#define CHARACTERDATA_H

#include <QString>
#include <QStringList>

class CharacterData
{
public:
    CharacterData();
    QString id;
    QString name;
    QString gender;
    QString dynasty;
    QString dna;
    QString religion;
    QString culture;
    bool disallowRandomTraits;
    QStringList traits;
    QString birth;
    QString death;
    QString fatherId;
    QString motherId;
    QStringList spouseIds;
    QStringList childrenIds;
    int characterNumber;
};

#endif // CHARACTERDATA_H
