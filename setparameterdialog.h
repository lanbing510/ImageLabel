#ifndef SETPARAMETERDIALOG_H
#define SETPARAMETERDIALOG_H

#include <QDialog>

namespace Ui {
class SetParameterDialog;
}

class SetParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetParameterDialog(QWidget *parent = 0);
    ~SetParameterDialog();

    QString getSeqPath();
    QString getFileFormat();
    QString getOutputPath();
    QString getStartFrame();

    void setSeqPath(const QString& str);
    void setFileFormat(const QString& str);
    void setOutputPath(const QString& str);
    void setStartFrame(const QString& str);
private:
    Ui::SetParameterDialog *ui;
};

#endif // SETPARAMETERDIALOG_H
