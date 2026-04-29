#include "Etudiant.h"

Etudiant::Etudiant() 
{

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
