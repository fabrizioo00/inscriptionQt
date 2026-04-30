#include "Commands.h"
#include "mainwindow.h"

AddCommand::AddCommand(QUndoCommand *parent)
    : QUndoCommand(parent), m_etudiants(nullptr), m_index(-1), m_mw(nullptr)
{
    setText("Ajouter etudiant");
}

void AddCommand::setEtudiants(QList<Etudiant> *etudiants) 
{ 
    m_etudiants = etudiants; 
}
void AddCommand::setEtudiant(const Etudiant &etudiant) 
{ 
    m_etudiant = etudiant; 
}
void AddCommand::setMainWindow(MainWindow *mw) 
{ 
    m_mw = mw; 
}

void AddCommand::undo()
{
    if (m_index >= 0 && m_index < m_etudiants->size()) 
    {
        m_etudiants->removeAt(m_index);
        if (m_mw) m_mw->updateTable();
    }
}

void AddCommand::redo()
{
    if (m_index == -1) 
    {
        m_etudiants->append(m_etudiant);
        m_index = m_etudiants->size() - 1;
    } 
    else 
    {
        m_etudiants->insert(m_index, m_etudiant);
    }
    if (m_mw) m_mw->updateTable();
}


RemoveCommand::RemoveCommand(QUndoCommand *parent)
    : QUndoCommand(parent), m_etudiants(nullptr), m_index(-1), m_mw(nullptr)
{
    setText("Supprimer etudiant");
}

void RemoveCommand::setEtudiants(QList<Etudiant> *etudiants) 
{ 
    m_etudiants = etudiants; 
    if (m_index >= 0 && m_etudiants && m_index < m_etudiants->size()) 
    {
        m_etudiant = m_etudiants->at(m_index);
    }
}
void RemoveCommand::setIndex(int index) 
{ 
    m_index = index; 
    if (m_index >= 0 && m_etudiants && m_index < m_etudiants->size()) 
    {
        m_etudiant = m_etudiants->at(m_index);
    }
}
void RemoveCommand::setMainWindow(MainWindow *mw) { m_mw = mw; }

void RemoveCommand::undo()
{
    m_etudiants->insert(m_index, m_etudiant);
    if (m_mw) m_mw->updateTable();
}

void RemoveCommand::redo()
{
    m_etudiants->removeAt(m_index);
    if (m_mw) m_mw->updateTable();
}


EditCommand::EditCommand(QUndoCommand *parent)
    : QUndoCommand(parent), m_etudiants(nullptr), m_index(-1), m_mw(nullptr)
{
    setText("Modifier etudiant");
}

void EditCommand::setEtudiants(QList<Etudiant> *etudiants) 
{ 
    m_etudiants = etudiants; 
    if (m_index >= 0 && m_etudiants && m_index < m_etudiants->size()) 
    {
        m_oldEtudiant = m_etudiants->at(m_index);
    }
}
void EditCommand::setIndex(int index) 
{ 
    m_index = index; 
    if (m_index >= 0 && m_etudiants && m_index < m_etudiants->size()) 
    {
        m_oldEtudiant = m_etudiants->at(m_index);
    }
}
void EditCommand::setNewEtudiant(const Etudiant &newEtudiant) 
{ 
    m_newEtudiant = newEtudiant; 
}
void EditCommand::setMainWindow(MainWindow *mw) 
{ 
    m_mw = mw; 
}

void EditCommand::undo()
{
    (*m_etudiants)[m_index] = m_oldEtudiant;
    if (m_mw) m_mw->updateTable();
}

void EditCommand::redo()
{
    (*m_etudiants)[m_index] = m_newEtudiant;
    if (m_mw) m_mw->updateTable();
}
