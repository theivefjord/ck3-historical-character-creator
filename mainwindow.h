#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "characterdata.h"
#include "charactertoken.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QTextStream>
#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString currentFile;
    QMap<QString, QString> fieldValues;
    QGraphicsScene *familyTreeScene;
    QList<CharacterData*> characters;
    QMap<QString, CharacterData*> charactersById;
    QMap<QString, CharacterToken*> tokensById;
    QMap<QString, QGraphicsPathItem*> parentLines;
    QMap<QString, QGraphicsPathItem*> spouseLines;

    QString formatCharacter(CharacterData *character, int characterIndex);
    void addCharacterToScene(CharacterData *character);
    void exampleCharacterSelection(CharacterData *initialCharacter);
    void updateDnaIfChecked(CharacterData *c);
    void initializeCharacterSheet();

private slots:
    void openFile();
    void saveFileAs();
    void saveFile();
    void closeFile();
    void reloadFile();
    // remember to make a death reason selection input after the death date thingy
    void on_FieldEdited(const QString &text);
    void on_addCharacter_clicked();
    void on_female_checkBox_stateChanged(int arg1);
    void on_rtraits_checkBox_stateChanged(int arg1);
    void on_traitButton_clicked();
    void updateAllCharacterText();
    void drawSpouseLine(QString char1Id, QString char2Id);
    void drawParentLine(QString char1Id, QString char2Id);
    void updateAllLines();
    void on_dna_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_AGOT_checkBox_checkStateChanged(const Qt::CheckState &arg1);
    void on_actionChange_character_index_triggered();
    void on_actionToggle_DNA_field_toggled(bool arg1);
    void on_actionToggle_Death_date_field_toggled(bool arg1);
    void on_actionReload_Text_not_file_triggered();
    void on_openFileEdit_modificationChanged(bool arg1);
    void on_removeSpousesRequested(QString characterId);
    void on_removeParentsRequested(QString characterId);
};

#endif // MAINWINDOW_H
