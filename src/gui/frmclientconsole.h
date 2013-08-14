#ifndef FRMCLIENTCONSOLE_H
#define FRMCLIENTCONSOLE_H

#include <QWidget>
#include <QLineEdit>

namespace Ui {
class FrmClientConsole;
}

class FrmClientConsole : public QWidget
{
    Q_OBJECT
    
public:
    explicit FrmClientConsole(QWidget *parent = 0);
    ~FrmClientConsole();
    
    void AddLine(const QString &line, const QString &color = "");
    QLineEdit *getEditSend() const;
private:
    Ui::FrmClientConsole *ui;
};

#endif // FRMCLIENTCONSOLE_H
