#ifndef CMDBUTTON_H
#define CMDBUTTON_H

#include <QObject>
#include <QToolButton>

class CmdButton : public QToolButton
{
    Q_OBJECT
private:
    int m_prefix;
    QString m_cmdLine;
public:
    CmdButton(int m_prefix, QString m_cmdLine);

private slots:
    void runCmd();
    void editCmd();
    void mousePressEvent(QMouseEvent *e);
signals:
    void execute(QString cmd);
    void rightClicked();
};

#endif // CMDBUTTON_H
