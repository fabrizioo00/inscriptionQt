#include "Etudiant.h"
#include <QStringList>


Etudiant::Etudiant() 
{
    m_age = 0;
}



QString Etudiant::getNom() const 
{ 
    return (m_nom); 
}
void Etudiant::setNom(const QString& nom) 
{ 
    m_nom = nom; 
}

QString Etudiant::getPrenom() const 
{ 
    return (m_prenom); 
}
void Etudiant::setPrenom(const QString& prenom) 
{ 
    m_prenom = prenom; 
}

int Etudiant::getAge() const 
{ 
    return (m_age); 
}
void Etudiant::setAge(int age) 
{ 
    m_age = age; 
}

QString Etudiant::getSexe() const 
{ 
    return (m_sexe); 
}
void Etudiant::setSexe(const QString& sexe) 
{ 
    m_sexe = sexe; 
}

bool Etudiant::estAgeValide(int age)
{
    return (age > 0 && age <= 150);
}

bool Etudiant::estNomValide(const QString& nom)
{
    return (!nom.trimmed().isEmpty());
}

Etudiant Etudiant::fromString(const QString& ligneCsv)
{
    Etudiant e;
    QStringList parts = ligneCsv.split(',');
    if (parts.size() >= 2) 
    {
        e.setNom(parts[0]);
        e.setPrenom(parts[1]);
        if (parts.size() >= 4) 
        {
            e.setAge(parts[2].toInt());
            e.setSexe(parts[3]);
        }
    }
    return (e);
}
