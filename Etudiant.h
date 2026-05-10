#ifndef ETUDIANT_H
#define ETUDIANT_H
#include <QString>

class Etudiant 
{
public:

    Etudiant();

    QString getNom() const;
    void setNom(const QString& nom);

    QString getPrenom() const;
    void setPrenom(const QString& prenom);

    int getAge() const;
    void setAge(int age);

    QString getSexe() const;
    void setSexe(const QString& sexe);

    static bool estAgeValide(int age);
    static bool estNomValide(const QString& nom);
    static Etudiant fromString(const QString& ligneCsv);

private:
    QString m_nom;
    QString m_prenom;
    int m_age;
    QString m_sexe;
};

#endif 
