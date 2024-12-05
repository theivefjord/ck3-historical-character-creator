#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QRegularExpression"
#include "QScrollBar"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeFile);
    connect(ui->actionReload_File, &QAction::triggered, this, &MainWindow::reloadFile);
    currentFile = "";
    setWindowTitle(tr("Untitled"));
    connect(ui->nameField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->dnaField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->religionField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->cultureField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->dynField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->bdayField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->deathdayField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    characters.append(QMap<QString, QVariant>());
    // trait buttons
    QList<QPushButton*> traitButtons = ui->traitTabs->findChildren<QPushButton*>();
    for (QPushButton *button : traitButtons)
    {
        connect(button, &QPushButton::clicked, this, &MainWindow::on_traitButton_clicked);
    }
    // setup trait tabs
    QSizePolicy sp_retain = ui->traitTabs->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->traitTabs->setSizePolicy(sp_retain);
    ui->traitTabs->setVisible(false);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            ui->openFileEdit->setPlainText(in.readAll());
            file.close();
            currentFile = fileName;
            setWindowTitle(fileName);
        }
    }
}
void MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << ui->openFileEdit->toPlainText();
            file.close();
        }
    }
}
void MainWindow::saveFile()
{
    if (currentFile.isEmpty())
    {
        saveFileAs();
    }
    else
    {
        QFile file(currentFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << ui->openFileEdit->toPlainText();
            file.close();
        }
    }
}
void MainWindow::closeFile()
{
    if (ui->openFileEdit->document()->isModified())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved changes. Do you want to save them before closing?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
        );

        if (reply == QMessageBox::Yes)
        {
            saveFile();
        }
        else if (reply == QMessageBox::Cancel)
        {
            return;
        }
    }

    ui->openFileEdit->clear();
    currentFile = "";
    ui->openFileEdit->document()->setModified(false);
    setWindowTitle(tr("Untitled"));
}
void MainWindow::reloadFile()
{
    if (ui->openFileEdit->document()->isModified())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved changes. Do you want to save them before closing?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if (reply == QMessageBox::Yes)
        {
            saveFile();
        }
        else if (reply == QMessageBox::Cancel)
        {
            return;
        }
    }
    if (!currentFile.isEmpty())
    {
        QFile file(currentFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            ui->openFileEdit->setPlainText(in.readAll());
            file.close();
            ui->openFileEdit->document()->setModified(false);
            statusBar()->showMessage(tr("File reloaded successfully"), 3000);
            // make the program blink or something here for feedback otherwise you'll feel unsure
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::formatCharacter(const QMap<QString, QVariant> &character, const QStringList &fieldOrder, int characterIndex)
{
    QString formattedText;

    if (character.contains("dynasty"))
    {
        QString dynasty = character["dynasty"].toString();
        formattedText += QString("%1_%2 = {\n").arg(dynasty).arg(characterIndex);
    }

    for (const QString &orderedKey : fieldOrder)
    {
        if (orderedKey == "birth" && character.contains(orderedKey) && !character[orderedKey].toString().isEmpty())
        {
            formattedText += QString("    %1 = {\n        birth = yes\n    }\n").arg(character[orderedKey].toString());
        }
        else if (orderedKey == "death" && character.contains(orderedKey) && !character[orderedKey].toString().isEmpty())
        {
            formattedText += QString("    %1 = { death = yes }\n").arg(character[orderedKey].toString());
        }
        else if (character.contains(orderedKey) && !character[orderedKey].toString().isEmpty())
        {
            formattedText += QString("    %1 = %2\n").arg(orderedKey, character[orderedKey].toString());
        }
    }

    if (character.contains("traits"))
    {
        QStringList traits = character["traits"].toStringList();
        for (const QString &trait : traits)
        {
            formattedText += QString("  trait = %1\n").arg(trait);
        }
    }
    formattedText += "}\n\n";
    return formattedText;
}

void MainWindow::on_FieldEdited(const QString &text)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    QString key;

    if (lineEdit == ui->nameField)
        key = "name";
    else if (lineEdit == ui->dnaField)
        key = "dna";
    else if (lineEdit == ui->religionField)
        key = "religion";
    else if (lineEdit == ui->cultureField)
        key = "culture";
    else if (lineEdit == ui->dynField)
        key = "dynasty";
    else if (lineEdit == ui->bdayField)
        key = "birth";
    else if (lineEdit == ui->deathdayField)
        key = "death";

    if (!key.isEmpty() && !characters.isEmpty())
    {
        // scrollbar stuff
        QScrollBar *scrollBar = ui->openFileEdit_newchars->verticalScrollBar();
        int scrollPosition = scrollBar->value();

        characters[characters.size() - 1][key] = text;

        QString updatedText;

        for (int i = 0; i < characters.size(); ++i)
        {
            updatedText += formatCharacter(characters[i], fieldOrder, i + 1);
        }

        ui->openFileEdit_newchars->setPlainText(updatedText);

        scrollBar->setValue(scrollPosition);
    }
}

void MainWindow::on_traitButton_clicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button || characters.isEmpty())
        return;

    QString traitName = button->objectName();
    if (traitName.isEmpty())
        return;

    QMap<QString, QVariant> &activeCharacter = characters.last();

    QStringList traits = activeCharacter.value("traits").toStringList();

    if (button->isChecked())
    {
        if (!traits.contains(traitName))
            traits.append(traitName);
    }
    else
    {
        traits.removeAll(traitName);
    }

    activeCharacter["traits"] = traits;

    QString updatedText;
    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], fieldOrder, i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
}

void MainWindow::on_addCharacter_clicked()
{
    QMap<QString, QVariant> currentCharacter;
    currentCharacter["name"] = ui->nameField->text();
    currentCharacter["dna"] = ui->dnaField->text();
    currentCharacter["father"] = "";
    currentCharacter["mother"] = "";
    currentCharacter["religion"] = ui->religionField->text();
    currentCharacter["culture"] = ui->cultureField->text();
    currentCharacter["dynasty"] = ui->dynField->text();
    currentCharacter["birth"] = ui->bdayField->text();
    currentCharacter["death"] = ui->deathdayField->text();

    characters.append(currentCharacter);

    //ui->nameField->clear();
    //ui->dnaField->clear();
    //ui->religionField->clear();
    //ui->cultureField->clear();
    //ui->dynField->clear();
    //ui->bdayField->clear();
    //ui->deathdayField->clear();
    ui->female_checkBox->setChecked(false);
    ui->rtraits_checkBox->setChecked(false);

    QString updatedText;

    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], fieldOrder, i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
    ui->openFileEdit_newchars->verticalScrollBar()->setValue(ui->openFileEdit_newchars->verticalScrollBar()->maximum());
}

void MainWindow::on_female_checkBox_stateChanged(int state)
{
    if (characters.isEmpty())
        return;

    QMap<QString, QVariant> &activeCharacter = characters[characters.size() - 1];

    if (state == Qt::Checked)
    {
        activeCharacter["female"] = "yes";
    }
    else
    {
        activeCharacter.remove("female");
    }

    QString updatedText;

    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], fieldOrder, i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
    ui->openFileEdit_newchars->verticalScrollBar()->setValue(ui->openFileEdit_newchars->verticalScrollBar()->maximum());
}


void MainWindow::on_rtraits_checkBox_stateChanged(int state)
{
    if (characters.isEmpty())
        return;

    QMap<QString, QVariant> &activeCharacter = characters[characters.size() - 1];

    if (state == Qt::Checked)
    {
        activeCharacter["disallow_random_traits"] = "yes";
        ui->traitTabs->setVisible(true);
    }
    else
    {
        activeCharacter.remove("disallow_random_traits");
        ui->traitTabs->setVisible(false);
    }

    QString updatedText;

    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], fieldOrder, i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
    ui->openFileEdit_newchars->verticalScrollBar()->setValue(ui->openFileEdit_newchars->verticalScrollBar()->maximum());
}

