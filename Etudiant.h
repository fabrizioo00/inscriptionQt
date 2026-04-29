#ifndef ETUDIANT_H
#define ETUDIANT_H
#include <QString>

class Etudiant 
{
public:
    /* Le constructeur par défaut est nécessaire pour que QList fonctionne correctement. */
    Etudiant();

    QString getNom() const;
    void setNom(const QString& nom);

    QString getPrenom() const;
    void setPrenom(const QString& prenom);

private:
    QString m_nom;
    QString m_prenom;
};

#endif 
