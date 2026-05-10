#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoStack>
#include <QVector>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include "Etudiant.h"
#include <QLabel>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define ITEMS_PER_PAGE 10

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateTable();

    int getEtudiantsCount() const;
    Etudiant getEtudiant(int index) const;
    void insererEtudiant(int index, const Etudiant &e);
    void ajouterEtudiant(const Etudiant &e);
    void supprimerEtudiantAt(int index);
    void modifierEtudiantAt(int index, const Etudiant &e);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAjouterClicked();
    void onRechercherClicked();
    void onUndoTriggered();
    void onPrecedentClicked();
    void onSuivantClicked();

    void onModifierEtudiantClicked();
    void onSupprimerEtudiantClicked();

    void onCustomContextMenu(const QPoint &pos);


    void onActionOuvrirTriggered();

private:
    void modifierEtudiant(int globalIndex);
    void supprimerEtudiant(int globalIndex);
    void loadFromFile();
    void saveToFile();

    QVector<Etudiant> m_etudiants;
    QVector<int> m_filteredIndices;
    QString m_currentSearch;

    static const QString NOM_FICHIER_SAUVEGARDE;

    QUndoStack *m_undoStack;

    int m_currentPage;

    Ui::MainWindow *ui;
};

#endif 
