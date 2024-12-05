#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTextStream>

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
    QList<QMap<QString, QVariant>> characters;
    QString formatCharacter(const QMap<QString, QVariant> &character, const QStringList &fieldOrder, int characterIndex);
    const QStringList fieldOrder = {
        "name", "dynasty", "dna", "female", "culture", "religion", "disallow_random_traits", "birth", "death"
    };


private slots:
    void openFile();
    void saveFileAs();
    void saveFile();
    void closeFile();
    void reloadFile();
    // remember to make a death reason selection input after the death date thingy
    //void on_nameField_textEdited(const QString &text);
    void on_FieldEdited(const QString &text);
    void on_addCharacter_clicked();
    void on_female_checkBox_stateChanged(int arg1);
    void on_rtraits_checkBox_stateChanged(int arg1);
    void on_traitButton_clicked();
};

#endif // MAINWINDOW_H
