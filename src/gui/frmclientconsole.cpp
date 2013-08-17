#include "frmclientconsole.h"
#include "ui_frmclientconsole.h"

FrmClientConsole::FrmClientConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmClientConsole)
{
    ui->setupUi(this);
}

FrmClientConsole::~FrmClientConsole()
{
    delete ui;
}

void FrmClientConsole::AddLine(const QString& line, const QString& color)
{
    //we need to strip html and what-not
    QString style = color.isEmpty() ? tr("") : tr(" style=\"color: %1\"").arg(color);
    ui->txtConsole->append(tr("<pre")+style+tr(">") + line + tr("</pre>"));
}

QLineEdit *FrmClientConsole::getEditSend() const
{
    return ui->edtSend;
}
