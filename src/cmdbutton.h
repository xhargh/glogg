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
    void update();
public:
    CmdButton(int m_prefix, QString m_cmdLine);
    QString getCmdLine() const { return m_cmdLine; }
    void setCmdLine(QString cmdLine) { m_cmdLine = cmdLine; update(); }

private slots:
    void runCmd();
    void editCmd();
    void mousePressEvent(QMouseEvent *e);
signals:
    void execute(QString cmd);
    void rightClicked();
};

#endif // CMDBUTTON_H
