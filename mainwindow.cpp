#include "mainwindow.h"
#include "charactertoken.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QRegularExpression"
#include "QScrollBar"
#include "characterdata.h"
#include "QValidator"

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
    // datevalidator stuff
    QRegularExpression re("^\\d+\\.(1[0-2]|[1-9])\\.(30|[12]\\d|[1-9])$");
    QRegularExpressionValidator *dateValidator = new QRegularExpressionValidator(re, this);

    connect(ui->nameField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->dnaField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->religionField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->cultureField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->dynField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    connect(ui->bdayField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    ui->bdayField->setValidator(dateValidator);
    connect(ui->deathdayField, &QLineEdit::textEdited, this, &MainWindow::on_FieldEdited);
    ui->deathdayField->setValidator(dateValidator);
    // create example character
    CharacterData *initialCharacter = new CharacterData();
    exampleCharacterSelection(initialCharacter);

    // family tree stuff
    familyTreeScene = new QGraphicsScene(this);
    ui->familyTreeView->setScene(familyTreeScene);
    addCharacterToScene(initialCharacter);

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

void MainWindow::exampleCharacterSelection(CharacterData *initialCharacter)
{
    int randNum = rand() % 5;
    if (randNum == 1 || 2 || 3 || 4){
        // Set initial text in line edits
        ui->nameField->setText("Henry");
        ui->dynField->setText("normandy");
        ui->religionField->setText("catholic");
        ui->cultureField->setText("english");
        ui->dnaField->setText("normandy_1");
        ui->bdayField->setText("1068.8.20");
        ui->deathdayField->setText("1135.12.1");


        initialCharacter->characterNumber = 1;
        initialCharacter->id = "character_1";
        initialCharacter->name = "Henry";
        initialCharacter->dynasty = "normandy";
        initialCharacter->religion = "catholic";
        initialCharacter->culture = "english";
        initialCharacter->dna = "normandy_1";
        initialCharacter->birth = "1068.8.20";
        initialCharacter->death = "1135.12.1";
        characters.append(initialCharacter);
        // Update text output
        QString updatedText;
        for (int i = 0; i < characters.size(); ++i) {
            updatedText += formatCharacter(characters[i], i + 1);
        }
        ui->openFileEdit_newchars->setPlainText(updatedText);
    }
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

QString MainWindow::formatCharacter(CharacterData *character, int characterIndex)
{
    QString formattedText;

    if (!character->dynasty.isEmpty())
    {
        formattedText += QString("%1_%2 = {\n").arg(character->dynasty).arg(characterIndex);
    }
    else
    {
        formattedText += QString("character_%1 = {\n").arg(character->dynasty);
    }

    if (!character->name.isEmpty())
        formattedText += QString ("    name = \%1\n").arg(character->name);
    if (!character->dna.isEmpty())
        formattedText += QString ("    dna = \%1\n").arg(character->dna);
    if (!character->dynasty.isEmpty())
        formattedText += QString("    dynasty = \%1\n").arg(character->dynasty);
    if (!character->culture.isEmpty())
        formattedText += QString ("    culture = \%1\n").arg(character->culture);
    if (!character->religion.isEmpty())
        formattedText += QString ("    religion = \%1\n").arg(character->religion);

    if (character->gender == "female")
        formattedText += QString ("    female = yes\n");

    if (!character->fatherId.isEmpty())
        formattedText += QString ("    father = \%1\n").arg(character->fatherId);

    if (!character->motherId.isEmpty())
        formattedText += QString ("    mother = \%1\n").arg(character->motherId);

    if (character->disallowRandomTraits == true)
    {
        formattedText += QString("    disallow_random_traits = yes\n");
    }

    for (const QString &trait : character->traits)
    {
        if (trait.contains("i_e_")){
            QStringList traitParts = trait.split('_');
            QString updatedTrait = "education_intrigue_" + traitParts[2];
            formattedText += QString("    trait = %1\n").arg(updatedTrait);
        }
        else if (trait.contains("d_e_")){
            QStringList traitParts = trait.split('_');
            QString updatedTrait = "education_diplomacy_" + traitParts[2];
            formattedText += QString("    trait = %1\n").arg(updatedTrait);
        }
        else if (trait.contains("s_e_")){
            QStringList traitParts = trait.split('_');
            QString updatedTrait = "education_stewardship_" + traitParts[2];
            formattedText += QString("    trait = %1\n").arg(updatedTrait);
        }
        else if (trait.contains("m_e_")){
            QStringList traitParts = trait.split('_');
            QString updatedTrait = "education_martial_" + traitParts[2];
            formattedText += QString("    trait = %1\n").arg(updatedTrait);
        }
        else if (trait.contains("l_e_")){
            QStringList traitParts = trait.split('_');
            QString updatedTrait = "education_learning_" + traitParts[2];
            formattedText += QString("    trait = %1\n").arg(updatedTrait);
        }
        else
            formattedText += QString("    trait = %1\n").arg(trait);
    }

    if (!character->birth.isEmpty())
        formattedText += QString ("    %1 = { birth = yes }\n").arg(character->birth);

    for (const QString &spouseId : character->spouseIds)
    {
        QStringList birthParts = character->birth.split('.');
        int birthyearPlus16 = 0;
        //QString earliestMarryDate = "x.x.x";
        if (birthParts.size() == 3){
            int birthYear = birthParts[0].toInt();
            birthyearPlus16 = birthYear + 16;
        }
        QString earliestMarryDate = QString::number(birthyearPlus16) + "." + birthParts[1] + "." + birthParts[2];
        formattedText += QString("    %2 = { add_spouse = %1 }\n").arg(spouseId).arg(earliestMarryDate);
    }

    if (!character->death.isEmpty())
        formattedText += QString ("    %1 = { death = yes }\n").arg(character->death);


    formattedText += "}\n\n";
    return formattedText;
}

void MainWindow::on_FieldEdited(const QString &text)
{
    //CharacterData *activeChar = characters.last();
    if (characters.isEmpty())
        return;

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    if (!lineEdit)
        return;

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

    if (key.isEmpty())
        return;

    CharacterData *activeChar = characters.last();
    if (key == "name")
        activeChar->name = text;
    else if (key == "dna")
        activeChar->dna = text;
    else if (key == "religion")
        activeChar->religion = text;
    else if (key == "culture")
        activeChar->culture = text;
    else if (key == "dynasty")
        activeChar->dynasty = text;
    else if (key == "birth")
        activeChar->birth = text;
    else if (key == "death")
        activeChar->death = text;

    QScrollBar *scrollBar = ui->openFileEdit_newchars->verticalScrollBar();
    int scrollPosition = scrollBar->value();

    QString updatedText;
    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], i+1);
    }
    ui->openFileEdit_newchars->setPlainText(updatedText);
    scrollBar->setValue(scrollPosition);
}

void MainWindow::on_traitButton_clicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button || characters.isEmpty())
        return;

    QString traitName = button->objectName();
    if (traitName.isEmpty())
        return;

    CharacterData *activeCharacter = characters.last();

    QStringList traits = activeCharacter->traits;

    if (button->isChecked())
    {
        if (!traits.contains(traitName))
            traits.append(traitName);
    }
    else
    {
        traits.removeAll(traitName);
    }

    activeCharacter->traits = traits;

    QString updatedText;
    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
}

void MainWindow::on_addCharacter_clicked()
{
    if (characters.isEmpty())
        return;

    CharacterData *currentCharacter = characters.last();

    // first get the characterNumber
    int charIndex = characters.size();
    currentCharacter->characterNumber = charIndex;
    currentCharacter->name = ui->nameField->text();
    currentCharacter->dna = ui->dnaField->text();
    currentCharacter->fatherId = "";
    currentCharacter->motherId = "";
    currentCharacter->religion = ui->religionField->text();
    currentCharacter->culture = ui->cultureField->text();
    currentCharacter->dynasty = ui->dynField->text();
    currentCharacter->birth = ui->bdayField->text();
    currentCharacter->death = ui->deathdayField->text();

    // store old id before changing it
    QString oldId = currentCharacter->id;

    // assign the id for spouse/father/mother etc
    if (!currentCharacter->dynasty.isEmpty())
        currentCharacter->id = currentCharacter->dynasty + "_" + QString::number(currentCharacter->characterNumber);
    else
        currentCharacter->id = "character_" + QString::number(currentCharacter->characterNumber);

    // Remove the old entry from tokensById
    if (tokensById.contains(oldId)) {
        // remove old id
        CharacterToken *token = tokensById.take(oldId);
        tokensById[currentCharacter->id] = token; // Reinsert with new id
    } else {
        qDebug() << "if this prints something's wrong with the tokens";
    }

    charactersById[currentCharacter->id] = currentCharacter;





    CharacterData *newCharacter = new CharacterData();
    int newCharIndex = characters.size() + 1;
    newCharacter->characterNumber = newCharIndex;
    newCharacter->name = ui->nameField->text();
    newCharacter->dna = ui->dnaField->text();
    newCharacter->fatherId = "";
    newCharacter->motherId = "";
    newCharacter->religion = ui->religionField->text();
    newCharacter->culture = ui->cultureField->text();
    newCharacter->dynasty = ui->dynField->text();
    newCharacter->birth = ui->bdayField->text();
    newCharacter->death = ui->deathdayField->text();

    // assign the id for spouse/father/mother etc
    if (!newCharacter->dynasty.isEmpty())
        newCharacter->id = newCharacter->dynasty + "_" + QString::number(newCharacter->characterNumber);
    else
        newCharacter->id = "character_" + QString::number(newCharacter->characterNumber);

    characters.append(newCharacter);
    addCharacterToScene(newCharacter);
    // update the displayed text
    QString updatedText;
    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], i + 1);
    }
    ui->openFileEdit_newchars->setPlainText(updatedText);
    ui->openFileEdit_newchars->verticalScrollBar()->setValue(ui->openFileEdit_newchars->verticalScrollBar()->maximum());
    ui->female_checkBox->setChecked(false);
    ui->rtraits_checkBox->setChecked(false);
}

void MainWindow::on_female_checkBox_stateChanged(int state)
{
    if (characters.isEmpty())
        return;

    CharacterData *activeCharacter = characters.last();

    if (state == Qt::Checked)
    {
        activeCharacter->gender = "female";
    }
    else
    {
        activeCharacter->gender = "";
    }

    QString updatedText;

    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
    ui->openFileEdit_newchars->verticalScrollBar()->setValue(ui->openFileEdit_newchars->verticalScrollBar()->maximum());
}


void MainWindow::on_rtraits_checkBox_stateChanged(int state)
{
    if (characters.isEmpty())
        return;

    CharacterData *activeCharacter = characters.last();

    if (state == Qt::Checked)
    {
        activeCharacter->disallowRandomTraits = true;
        ui->traitTabs->setVisible(true);
    }
    else
    {
        activeCharacter->disallowRandomTraits = false;
        ui->traitTabs->setVisible(false);
    }

    QString updatedText;

    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], i + 1);
    }

    ui->openFileEdit_newchars->setPlainText(updatedText);
    ui->openFileEdit_newchars->verticalScrollBar()->setValue(ui->openFileEdit_newchars->verticalScrollBar()->maximum());
}
void MainWindow::updateAllCharacterText()
{
    QString updatedText;
    for (int i = 0; i < characters.size(); ++i)
    {
        updatedText += formatCharacter(characters[i], characters[i]->characterNumber);
    }
    ui->openFileEdit_newchars->setPlainText(updatedText);
}

void MainWindow::addCharacterToScene(CharacterData *character)
{
    CharacterToken *token = new CharacterToken(character);
    // connect token signal to slot in MainWindow
    connect(token, &CharacterToken::relationshipsChanged, this, &MainWindow::updateAllCharacterText);
    connect(token, &CharacterToken::spousesSet, this, &MainWindow::drawSpouseLine);
    connect(token, &CharacterToken::parentSet, this, &MainWindow::drawParentLine);

    // start at base position
    int x = 0, y = 0;
    token->setPos(x, y);
    familyTreeScene->addItem(token);

    while (!token->collidingItems().isEmpty()) {
        x += 20;
        token->setPos(x, y);

        if (x > 500){
            x = 0;
            y += 80;
            token->setPos(x, y);
        }
    }

    tokensById[character->id] = token;
}

void MainWindow::drawParentLine(QString char1Id, QString char2Id)
{

}

void MainWindow::drawSpouseLine(QString char1Id, QString char2Id)
{
    //qDebug() << "drawSpouseLine called with:" << char1Id << char2Id;

    //if (!tokensById.contains(char1Id) || !tokensById.contains(char2Id))
    //{
    //    qDebug() << "one of the tokens is not in tokensbyid.";
    //    return;
    //}

    CharacterToken *token1 = tokensById[char1Id];
    CharacterToken *token2 = tokensById[char2Id];

    // get positions
    QPointF pos2 = token2->scenePos();
    token1->setPos(pos2.x() - 120, pos2.y()); // align tokens for line
    QPointF pos1 = token1->scenePos();

    //qDebug() << "pos1:" << pos1 << "pos2" << pos2;


    // create a line item between them
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(pos1.x() + 100, pos1.y() + 25, pos2.x(), pos2.y() + 25));
    line->setPen(QPen(Qt::white, 3));
    // add line
    familyTreeScene->addItem(line);

    QGraphicsItemGroup *spouseGroup = familyTreeScene->createItemGroup({token1, token2, line});
    spouseGroup->setFlag(QGraphicsItem::ItemIsMovable, true);
    //spouseGroup->setFlag(QGraphicsItem::ItemIsSelectable, true);
    token1->setFlag(QGraphicsItem::ItemIsMovable, false);
    token2->setFlag(QGraphicsItem::ItemIsMovable, false);

}

