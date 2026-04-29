#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoStack>
#include <QList>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include "Etudiant.h"
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateTable();

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

private:
    void modifierEtudiant(int globalIndex);
    void supprimerEtudiant(int globalIndex);
    void loadFromFile();
    void saveToFile();

    void initializeUi();

    QList<Etudiant> m_etudiants;
    QList<int> m_filteredIndices; /* indices in m_etudiants that match search */
    QString m_currentSearch;

    QUndoStack *m_undoStack;

    int m_currentPage;
    const int ITEMS_PER_PAGE = 10;

    /* UI elements */
    QLineEdit *m_nomEdit;
    QLineEdit *m_prenomEdit;

    QLineEdit *m_searchEdit;

    QTableWidget *m_table;
    QLabel *m_pageLabel;
    
    /* Original UI pointer just to be safe with deletion, though we won't use setupUi from it */
    class Ui_MainWindow *ui_designer = nullptr; 
};

#endif /* MAINWINDOW_H */
