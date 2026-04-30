#include "Etudiant.h"

Etudiant::Etudiant() 
{
    m_age = 0;
}



QString Etudiant::getNom() const 
{ 
    return m_nom; 
}
void Etudiant::setNom(const QString& nom) 
{ 
    m_nom = nom; 
}

QString Etudiant::getPrenom() const 
{ 
    return m_prenom; 
}
void Etudiant::setPrenom(const QString& prenom) 
{ 
    m_prenom = prenom; 
}

int Etudiant::getAge() const 
{ 
    return m_age; 
}
void Etudiant::setAge(int age) 
{ 
    m_age = age; 
}

QString Etudiant::getSexe() const 
{ 
    return m_sexe; 
}
void Etudiant::setSexe(const QString& sexe) 
{ 
    m_sexe = sexe; 
}
