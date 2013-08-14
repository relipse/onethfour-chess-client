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
    QString style = color.isEmpty() ? tr("") : tr(" style=\"color: %1\"").arg(color);
    ui->txtConsole->append(tr("<div")+style+tr(">") + line + tr("</div>"));
}

QLineEdit *FrmClientConsole::getEditSend() const
{
    return ui->edtSend;
}
