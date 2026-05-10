#include "Commands.h"
#include "mainwindow.h"

AddCommand::AddCommand() : QUndoCommand()
{
    m_index = -1;
    m_mw = nullptr;
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
    if (m_index >= 0 && m_mw && m_index < m_mw->getEtudiantsCount()) 
    {
        m_mw->supprimerEtudiantAt(m_index);
    }
}

void AddCommand::redo()
{
    if (m_mw) 
    {
        if (m_index == -1) 
        {
            m_mw->ajouterEtudiant(m_etudiant);
            m_index = m_mw->getEtudiantsCount() - 1;
        } 
        else 
        {
            m_mw->insererEtudiant(m_index, m_etudiant);
        }
    }
}


RemoveCommand::RemoveCommand() : QUndoCommand()
{
    m_index = -1;
    m_mw = nullptr;
}

void RemoveCommand::setIndex(int index) 
{ 
    m_index = index; 
    if (m_index >= 0 && m_mw && m_index < m_mw->getEtudiantsCount()) 
    {
        m_etudiant = m_mw->getEtudiant(m_index);
    }
}
void RemoveCommand::setMainWindow(MainWindow *mw) 
{ 
    m_mw = mw; 
}

void RemoveCommand::undo()
{
    if (m_mw)
    {
        m_mw->insererEtudiant(m_index, m_etudiant);
    } 
        
}

void RemoveCommand::redo()
{
    if (m_mw)
    {
        m_mw->supprimerEtudiantAt(m_index);
    } 
        
}


EditCommand::EditCommand() : QUndoCommand()
{
    m_index = -1;
    m_mw = nullptr;
}

void EditCommand::setIndex(int index) 
{ 
    m_index = index; 
    if (m_index >= 0 && m_mw && m_index < m_mw->getEtudiantsCount()) 
    {
        m_oldEtudiant = m_mw->getEtudiant(m_index);
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
    if (m_mw)
    {
        m_mw->modifierEtudiantAt(m_index, m_oldEtudiant);
    } 
        
}

void EditCommand::redo()
{
    if (m_mw)
    {
        m_mw->modifierEtudiantAt(m_index, m_newEtudiant);
    } 
}
