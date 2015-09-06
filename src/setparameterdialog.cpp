#include "setparameterdialog.h"
#include "ui_setparameterdialog.h"

#include <string>

SetParameterDialog::SetParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetParameterDialog)
{
    ui->setupUi(this);
}

SetParameterDialog::~SetParameterDialog()
{
    delete ui;
}

QString SetParameterDialog::getSeqPath()
{
    return ui->lineEdit_SequencePath->text();
}

QString SetParameterDialog::getFileFormat()
{
    return ui->lineEdit_FileFormat->text();
}

QString SetParameterDialog::getOutputPath()
{
    return ui->lineEdit_OutputPath->text();
}

QString SetParameterDialog::getStartFrame()
{
    return ui->lineEdit_StartFrame->text();
}

void SetParameterDialog::setSeqPath(const QString& str)
{
    ui->lineEdit_SequencePath->setText(str);
}

void SetParameterDialog::setFileFormat(const QString& str)
{
    ui->lineEdit_FileFormat->setText(str);
}

void SetParameterDialog::setOutputPath(const QString& str)
{
    ui->lineEdit_OutputPath->setText(str);
}

void SetParameterDialog::setStartFrame(const QString& str)
{
    ui->lineEdit_StartFrame->setText(str);
}
