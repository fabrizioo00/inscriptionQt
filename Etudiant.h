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

    int getAge() const;
    void setAge(int age);

    QString getSexe() const;
    void setSexe(const QString& sexe);

private:
    QString m_nom;
    QString m_prenom;
    int m_age;
    QString m_sexe;
};

#endif 
