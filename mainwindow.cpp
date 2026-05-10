#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Commands.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QFileDialog>

#define NOM_FICHIER_SAUVEGARDE "etudiants.csv"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QAction *undoAction;
    QAction *redoAction;
    ui->setupUi(this);
    m_currentPage = 0;
    m_undoStack = new QUndoStack(this);

    // stretching table
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    undoAction = m_undoStack->createUndoAction(this, "Annuler");
    undoAction->setShortcut(QKeySequence("Ctrl+C"));
    ui->menuEdition->addAction(undoAction);

    redoAction = m_undoStack->createRedoAction(this, "Rétablir");
    redoAction->setShortcut(QKeySequence("Ctrl+Y"));
    ui->menuEdition->addAction(redoAction);


    connect(ui->btnAjouter, &QPushButton::clicked, this, &MainWindow::onAjouterClicked);
    connect(ui->btnRechercher, &QPushButton::clicked, this, &MainWindow::onRechercherClicked);
    connect(ui->btnPrecedent, &QPushButton::clicked, this, &MainWindow::onPrecedentClicked);
    connect(ui->btnSuivant, &QPushButton::clicked, this, &MainWindow::onSuivantClicked);

    loadFromFile();
    updateTable();

    ui->table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table, &QTableWidget::customContextMenuRequested, this, &MainWindow::onCustomContextMenu);

    connect(ui->actionOuvrir, &QAction::triggered, this, &MainWindow::onActionOuvrirTriggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAjouterClicked()
{
    QString nom;
    QString prenom;
    QString sexe;
    int age;
    bool okAge;
    Etudiant e;
    AddCommand *cmd;

    nom = ui->nomEdit->text().trimmed();
    prenom = ui->prenomEdit->text().trimmed();
    age = ui->ageEdit->text().toInt(&okAge);
    sexe = ui->sexeComboBox->currentText();

    if (!Etudiant::estNomValide(nom) || !Etudiant::estNomValide(prenom) || sexe.isEmpty() || !okAge || !Etudiant::estAgeValide(age)) 
    {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs correctement.");
        return;
    }

    e.setNom(nom);
    e.setPrenom(prenom);
    e.setAge(age);
    e.setSexe(sexe);

    cmd = new AddCommand();
    cmd->setEtudiant(e);
    cmd->setMainWindow(this);
    m_undoStack->push(cmd);

    // Clear fields 
    ui->nomEdit->clear();
    ui->prenomEdit->clear();
    ui->ageEdit->clear();
    ui->sexeComboBox->setCurrentIndex(0);
}

void MainWindow::onRechercherClicked()
{
    m_currentSearch = ui->searchEdit->text().trimmed().toLower();
    m_currentPage = 0;
    updateTable();
}

void MainWindow::onUndoTriggered()
{
    m_undoStack->undo();
}

void MainWindow::onPrecedentClicked()
{
    if (m_currentPage > 0) 
    {
        m_currentPage--;
        updateTable();
    }
}

void MainWindow::onSuivantClicked()
{
    int totalItems;
    int maxPage;

    totalItems = m_currentSearch.isEmpty() ? m_etudiants.size() : m_filteredIndices.size();
    maxPage = (totalItems - 1) / ITEMS_PER_PAGE;
    if (m_currentPage < maxPage) 
    {
        m_currentPage++;
        updateTable();
    }
}

void MainWindow::updateTable()
{
    int i;
    int totalItems;
    int maxPage;
    int startIndex;
    int endIndex;
    int row;
    int globalIndex;

    m_filteredIndices.clear();

    // Filter 
    for (i = 0; i < m_etudiants.size(); ++i) 
    {
        if (m_currentSearch.isEmpty()) 
        {
            m_filteredIndices.append(i);
        } 
        else 
        {
            if (m_etudiants[i].getNom().toLower().contains(m_currentSearch) || m_etudiants[i].getPrenom().toLower().contains(m_currentSearch)) 
            {
                m_filteredIndices.append(i);
            }
        }
    }

    totalItems = m_filteredIndices.size();
    if (totalItems == 0) 
    {
        m_currentPage = 0;
        ui->pageLabel->setText("Page 1/1");
        ui->table->setRowCount(0);
        return;
    }

    maxPage = (totalItems - 1) / ITEMS_PER_PAGE;
    if (m_currentPage > maxPage) 
    {
        m_currentPage = maxPage;
    }

    ui->pageLabel->setText(QString("Page %1/%2").arg(m_currentPage + 1).arg(maxPage + 1));

    startIndex = m_currentPage * ITEMS_PER_PAGE;
    endIndex = qMin(startIndex + ITEMS_PER_PAGE, totalItems);

    ui->table->setRowCount(endIndex - startIndex);

    for (row = 0; row < (endIndex - startIndex); ++row) 
    {
        globalIndex = m_filteredIndices[startIndex + row];

        ui->table->setItem(row, 0, new QTableWidgetItem(m_etudiants[globalIndex].getNom()));
        ui->table->setItem(row, 1, new QTableWidgetItem(m_etudiants[globalIndex].getPrenom()));
        ui->table->setItem(row, 2, new QTableWidgetItem(QString::number(m_etudiants[globalIndex].getAge())));
        ui->table->setItem(row, 3, new QTableWidgetItem(m_etudiants[globalIndex].getSexe()));

    }
}

void MainWindow::onModifierEtudiantClicked()
{
    QPushButton *btn;
    int globalIndex;

    btn = qobject_cast<QPushButton*>(sender());
    if (btn) 
    {
        globalIndex = btn->property("globalIndex").toInt();
        modifierEtudiant(globalIndex);
    }
}

void MainWindow::onSupprimerEtudiantClicked()
{
    QPushButton *btn;
    int globalIndex;

    btn = qobject_cast<QPushButton*>(sender());
    if (btn) 
    {
        globalIndex = btn->property("globalIndex").toInt();
        supprimerEtudiant(globalIndex);
    }
}

void MainWindow::modifierEtudiant(int globalIndex)
{
    Etudiant oldE;
    QString newNom;
    QString newPrenom;
    int newAge;
    QString newSexe;
    Etudiant newE;
    EditCommand *cmd;
    bool ok;
    QStringList sexeItems;
    int currentSexeIndex;

    if(globalIndex < 0 || globalIndex >= m_etudiants.size()) return;
    
    oldE = m_etudiants[globalIndex];
    newNom = QInputDialog::getText(this, "Modifier", "Nom:", QLineEdit::Normal, oldE.getNom());
    if(newNom.isEmpty()) return; // Cancelled 
    
    newPrenom = QInputDialog::getText(this, "Modifier", "Prénom:", QLineEdit::Normal, oldE.getPrenom());
    if(newPrenom.isEmpty()) return;

    newAge = QInputDialog::getInt(this, "Modifier", "Age:", oldE.getAge(), 0, 150, 1, &ok);
    if(!ok) return;

    sexeItems << "masculin";
    sexeItems << "feminin";

    currentSexeIndex = sexeItems.indexOf(oldE.getSexe());
    if  (currentSexeIndex == -1) currentSexeIndex = 0;

    newSexe = QInputDialog::getItem(this, "Modifier", "Sexe:", sexeItems, currentSexeIndex, false, &ok);
    if (!ok || newSexe.isEmpty()) return;


    newE.setNom(newNom);
    newE.setPrenom(newPrenom);
    newE.setAge(newAge);
    newE.setSexe(newSexe);

    cmd = new EditCommand();
    cmd->setMainWindow(this);
    cmd->setIndex(globalIndex);
    cmd->setNewEtudiant(newE);
    m_undoStack->push(cmd);
}

void MainWindow::supprimerEtudiant(int globalIndex)
{
    RemoveCommand *cmd;

    cmd = new RemoveCommand();
    cmd->setMainWindow(this);
    cmd->setIndex(globalIndex);
    m_undoStack->push(cmd);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn;

    if (m_undoStack->canUndo()) 
    { 
        // which means modifications occurred 
        resBtn = QMessageBox::question( this, "Quitter",
                                        tr("Voulez vous enregistrer les modifications ?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes) 
        {
            saveToFile();
            event->accept();
        } 
        else if (resBtn == QMessageBox::No) 
        {
            event->accept(); // Revert to initial state (modifications are unsaved, hence "abandoned") 
        } 
        else 
        {
            event->ignore(); // Cancel quit 
        }
    } 
    else 
    {
        event->accept();
    }
}

void MainWindow::saveToFile()
{
    QFile file;
    int i;

    file.setFileName(NOM_FICHIER_SAUVEGARDE);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out;
        
        out.setDevice(&file);
        for (i = 0; i < m_etudiants.size(); i++) 
        {
            out << m_etudiants[i].getNom() << "," << m_etudiants[i].getPrenom() << "," << m_etudiants[i].getAge() << "," << m_etudiants[i].getSexe() << "\n";
        }
        file.close();
    }
}

void MainWindow::loadFromFile()
{
    QFile file;
    QString line;
    Etudiant e;

    file.setFileName(NOM_FICHIER_SAUVEGARDE);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QTextStream in;
        
        in.setDevice(&file);
        while (!in.atEnd()) 
        {
            line = in.readLine();
            e = Etudiant::fromString(line);
            if (Etudiant::estNomValide(e.getNom())) 
            {
                m_etudiants.append(e);
            }
        }
        file.close();
        
 
        m_undoStack->setClean();
    }
}

int MainWindow::getEtudiantsCount() const
{
    return m_etudiants.size();
}

Etudiant MainWindow::getEtudiant(int index) const
{
    return m_etudiants.at(index);
}

void MainWindow::insererEtudiant(int index, const Etudiant &e)
{
    m_etudiants.insert(index, e);
    updateTable();
}

void MainWindow::ajouterEtudiant(const Etudiant &e)
{
    m_etudiants.append(e);
    updateTable();
}

void MainWindow::supprimerEtudiantAt(int index)
{
    m_etudiants.removeAt(index);
    updateTable();
}

void MainWindow::modifierEtudiantAt(int index, const Etudiant &e)
{
    m_etudiants[index] = e;
    updateTable();
}


void::MainWindow::onCustomContextMenu(const QPoint &pos)
{
    QTableWidgetItem *item;
    int row;
    int startIndex;
    int globalIndex;
    QAction *actionModif;
    QAction *actionSuppr;
    QAction *selecledAction;
    QMenu menu(this);
    
    
    item = ui->table->itemAt(pos);
    if (!item) return;
    
    row = item->row();

    startIndex = m_currentPage * ITEMS_PER_PAGE;
    globalIndex = m_filteredIndices[startIndex + row];
    
    actionModif = menu.addAction("Modifier");
    actionSuppr = menu.addAction("Supprimer");

    selecledAction = menu.exec(ui->table->mapToGlobal(pos));

    if (selecledAction == actionModif)
    {
        modifierEtudiant(globalIndex);
    }
    else if (selecledAction == actionSuppr)
    {
        supprimerEtudiant(globalIndex);
    }
}

void MainWindow::onActionOuvrirTriggered()
{
    QString fileName;
    QFile file;
    QTextStream in;
    QString firstLine;
    QStringList parts;

    fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "", "fichiers csv (*.csv)");

    if (fileName.isEmpty()) 
    {
        return;
    }

    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le fichier.");
        return;
    }

    in.setDevice(&file);
    firstLine = in.readLine();
    file.close();

    
    parts = firstLine.split(',');
    if (!firstLine.isEmpty() && parts.size() < 4) 
    {
        QMessageBox::critical(this, "Erreur de format", "Le fichier CSV ne respecte pas le format attendu (Nom,Prenom,Age,Sexe).");
        return;
    }

    //Le format est validé, on remplace les données actuelles
    m_etudiants.clear();
    m_undoStack->clear(); // On réinitialise l'historique car le contexte a changé 

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QTextStream inStream;
        QString line;
        Etudiant e;

        inStream.setDevice(&file);
        
        while (!inStream.atEnd()) 
        {
            line = inStream.readLine();
            e = Etudiant::fromString(line);
            if (Etudiant::estNomValide(e.getNom())) 
            {
                m_etudiants.append(e);
            }
        }
        file.close();
        
        m_currentPage = 0;
        updateTable();
        
        QMessageBox::information(this, "Succès", "Fichier chargé avec succès !");
    }
}