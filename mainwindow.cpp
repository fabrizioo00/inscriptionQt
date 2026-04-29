#include "mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_currentPage(0)
{
    m_undoStack = new QUndoStack(this);
    initializeUi();
    loadFromFile();
    updateTable();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeUi()
{
    QMenu *editMenu;
    QAction *undoAction;
    QPushButton *btnUndoCorner;
    QWidget *central;
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QGroupBox *grpInscription;
    QFormLayout *formInscription;
    QPushButton *btnAjouter;
    QGroupBox *grpRecherche;
    QVBoxLayout *vboxRecherche;
    QPushButton *btnRechercher;
    QGroupBox *grpAffichage;
    QVBoxLayout *vboxAffichage;
    QHBoxLayout *paginationLayout;
    QPushButton *btnPrecedent;
    QPushButton *btnSuivant;

    setWindowTitle("Gestion des Inscriptions");
    resize(900, 600);

    // Menu 
    editMenu = menuBar()->addMenu("Edit");
    undoAction = m_undoStack->createUndoAction(this, "Annuler");
    // Change shortcut to Ctrl+C
    undoAction->setShortcut(QKeySequence("Ctrl+C"));
    editMenu->addAction(undoAction);

    btnUndoCorner = new QPushButton("o", this);
    menuBar()->setCornerWidget(btnUndoCorner, Qt::TopRightCorner);
    connect(btnUndoCorner, &QPushButton::clicked, m_undoStack, &QUndoStack::undo);

    // Central widget 
    central = new QWidget(this);
    setCentralWidget(central);
    mainLayout = new QVBoxLayout(central);

    // Top part - Split Left (Inscription) and Right (Recherche) 
    topLayout = new QHBoxLayout();
    
    // Left Group: Inscription 
    grpInscription = new QGroupBox("Inscription");
    formInscription = new QFormLayout(grpInscription);
    m_nomEdit = new QLineEdit();
    m_prenomEdit = new QLineEdit();
    formInscription->addRow("Nom:", m_nomEdit);
    formInscription->addRow("Prenom:", m_prenomEdit);
    
    btnAjouter = new QPushButton("Ajouter l'étudiant");
    formInscription->addRow(btnAjouter);
    connect(btnAjouter, &QPushButton::clicked, this, &MainWindow::onAjouterClicked);

    // Right Group: Recherche 
    grpRecherche = new QGroupBox("Recherche");
    vboxRecherche = new QVBoxLayout(grpRecherche);
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Rechercher par nom ou prénom...");
    btnRechercher = new QPushButton("Rechercher");
    
    vboxRecherche->addWidget(m_searchEdit);
    vboxRecherche->addWidget(btnRechercher);
    vboxRecherche->addStretch();

    connect(btnRechercher, &QPushButton::clicked, this, &MainWindow::onRechercherClicked);

    topLayout->addWidget(grpInscription);
    topLayout->addWidget(grpRecherche);

    // Bottom part - Affichage 
    grpAffichage = new QGroupBox("Affichage");
    vboxAffichage = new QVBoxLayout(grpAffichage);

    m_table = new QTableWidget();
    m_table->setColumnCount(3); // Nom, Prenom, Actions 
    m_table->setHorizontalHeaderLabels({"Nom", "Prénom", "Actions"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    vboxAffichage->addWidget(m_table);

    // Pagination controls 
    paginationLayout = new QHBoxLayout();
    btnPrecedent = new QPushButton("Précédent");
    m_pageLabel = new QLabel("Page 1");
    btnSuivant = new QPushButton("Suivant");

    paginationLayout->addStretch();
    paginationLayout->addWidget(btnPrecedent);
    paginationLayout->addWidget(m_pageLabel);
    paginationLayout->addWidget(btnSuivant);
    paginationLayout->addStretch();

    vboxAffichage->addLayout(paginationLayout);

    connect(btnPrecedent, &QPushButton::clicked, this, &MainWindow::onPrecedentClicked);
    connect(btnSuivant, &QPushButton::clicked, this, &MainWindow::onSuivantClicked);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(grpAffichage);
}

void MainWindow::onAjouterClicked()
{
    QString nom;
    QString prenom;
    Etudiant e;
    AddCommand *cmd;

    nom = m_nomEdit->text().trimmed();
    prenom = m_prenomEdit->text().trimmed();

    if (nom.isEmpty() || prenom.isEmpty()) 
    {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    e.setNom(nom);
    e.setPrenom(prenom);

    cmd = new AddCommand();
    cmd->setEtudiants(&m_etudiants);
    cmd->setEtudiant(e);
    cmd->setMainWindow(this);
    m_undoStack->push(cmd);

    // Clear fields 
    m_nomEdit->clear();
    m_prenomEdit->clear();
}

void MainWindow::onRechercherClicked()
{
    m_currentSearch = m_searchEdit->text().trimmed().toLower();
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
            const Etudiant &e = m_etudiants[i];
            if (e.getNom().toLower().contains(m_currentSearch) || e.getPrenom().toLower().contains(m_currentSearch)) 
            {
                m_filteredIndices.append(i);
            }
        }
    }

    totalItems = m_filteredIndices.size();
    if (totalItems == 0) 
    {
        m_currentPage = 0;
        m_pageLabel->setText("Page 1/1");
        m_table->setRowCount(0);
        return;
    }

    maxPage = (totalItems - 1) / ITEMS_PER_PAGE;
    if (m_currentPage > maxPage) 
    {
        m_currentPage = maxPage;
    }

    m_pageLabel->setText(QString("Page %1/%2").arg(m_currentPage + 1).arg(maxPage + 1));

    startIndex = m_currentPage * ITEMS_PER_PAGE;
    endIndex = qMin(startIndex + ITEMS_PER_PAGE, totalItems);

    m_table->setRowCount(endIndex - startIndex);

    for (row = 0; row < (endIndex - startIndex); ++row) 
    {
        int globalIndex;
        QWidget *actionWidget;
        QHBoxLayout *actionLayout;
        QPushButton *btnModif;
        QPushButton *btnSuppr;

        globalIndex = m_filteredIndices[startIndex + row];
        // Block scope declaration of e 
        {
            const Etudiant &e = m_etudiants[globalIndex];

            m_table->setItem(row, 0, new QTableWidgetItem(e.getNom()));
            m_table->setItem(row, 1, new QTableWidgetItem(e.getPrenom()));
        }

        // Create widget for Actions 
        actionWidget = new QWidget();
        actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0,0,0,0);
        
        btnModif = new QPushButton("Modifier");
        btnSuppr = new QPushButton("Supprimer");
        
        actionLayout->addWidget(btnModif);
        actionLayout->addWidget(btnSuppr);
        
        m_table->setCellWidget(row, 2, actionWidget);

        btnModif->setProperty("globalIndex", globalIndex);
        btnSuppr->setProperty("globalIndex", globalIndex);

        connect(btnModif, &QPushButton::clicked, this, &MainWindow::onModifierEtudiantClicked);
        connect(btnSuppr, &QPushButton::clicked, this, &MainWindow::onSupprimerEtudiantClicked);
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
    Etudiant newE;
    EditCommand *cmd;

    if(globalIndex < 0 || globalIndex >= m_etudiants.size()) return;
    
    oldE = m_etudiants[globalIndex];
    newNom = QInputDialog::getText(this, "Modifier", "Nom:", QLineEdit::Normal, oldE.getNom());
    if(newNom.isEmpty()) return; // Cancelled 
    
    newPrenom = QInputDialog::getText(this, "Modifier", "Prénom:", QLineEdit::Normal, oldE.getPrenom());
    if(newPrenom.isEmpty()) return;

    newE.setNom(newNom);
    newE.setPrenom(newPrenom);

    cmd = new EditCommand();
    cmd->setEtudiants(&m_etudiants);
    cmd->setIndex(globalIndex);
    cmd->setNewEtudiant(newE);
    cmd->setMainWindow(this);
    m_undoStack->push(cmd);
}

void MainWindow::supprimerEtudiant(int globalIndex)
{
    RemoveCommand *cmd;

    cmd = new RemoveCommand();
    cmd->setEtudiants(&m_etudiants);
    cmd->setIndex(globalIndex);
    cmd->setMainWindow(this);
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
    QFile file("etudiants.csv");
    int i;

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) 
    {
        QTextStream out(&file);
        for (i = 0; i < m_etudiants.size(); ++i) 
        {
            const Etudiant &e = m_etudiants[i];
            out << e.getNom() << "," << e.getPrenom() << "\n";
        }
        file.close();
    }
}

void MainWindow::loadFromFile()
{
    QFile file("etudiants.csv");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {

        QTextStream in(&file);
        while (!in.atEnd()) 
        {
            QString line;
            QStringList parts;

            line = in.readLine();
            parts = line.split(',');
            if (parts.size() >= 2) 
            {
                Etudiant e;
                e.setNom(parts[0]);
                e.setPrenom(parts[1]);
                m_etudiants.append(e);
            }
        }
        file.close();
        
        // Mark starting point to avoid saying dirty if we just loaded 
        m_undoStack->setClean();
    }
}
