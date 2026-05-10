#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QVector>
#include "Etudiant.h"

class MainWindow;

class AddCommand : public QUndoCommand
{
public:
    AddCommand();
    void setEtudiant(const Etudiant &etudiant);
    void setMainWindow(MainWindow *mw);
    void undo() override;
    void redo() override;
private:
    Etudiant m_etudiant;
    int m_index;
    MainWindow *m_mw;
};

class RemoveCommand : public QUndoCommand
{
public:
    RemoveCommand();
    void setIndex(int index);
    void setMainWindow(MainWindow *mw);
    void undo() override;
    void redo() override;
private:
    Etudiant m_etudiant;
    int m_index;
    MainWindow *m_mw;
};

class EditCommand : public QUndoCommand
{
public:
    EditCommand();
    void setIndex(int index);
    void setNewEtudiant(const Etudiant &newEtudiant);
    void setMainWindow(MainWindow *mw);
    void undo() override;
    void redo() override;
private:
    Etudiant m_oldEtudiant;
    Etudiant m_newEtudiant;
    int m_index;
    MainWindow *m_mw;
};

#endif 
