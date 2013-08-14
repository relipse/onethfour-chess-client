#include "dlgconnecttochessserver.h"
#include "ui_dlgconnecttochessserver.h"

DlgConnectToChessServer::DlgConnectToChessServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgConnectToChessServer)
{
    ui->setupUi(this);
}

DlgConnectToChessServer::~DlgConnectToChessServer()
{
    delete ui;
}
