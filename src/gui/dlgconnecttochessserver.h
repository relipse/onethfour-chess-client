#ifndef DLGCONNECTTOCHESSSERVER_H
#define DLGCONNECTTOCHESSSERVER_H

#include <QDialog>

namespace Ui {
class DlgConnectToChessServer;
}

class DlgConnectToChessServer : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgConnectToChessServer(QWidget *parent = 0);
    ~DlgConnectToChessServer();
    
public:
    Ui::DlgConnectToChessServer *ui;
};

#endif // DLGCONNECTTOCHESSSERVER_H
