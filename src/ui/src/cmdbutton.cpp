#include "cmdbutton.h"

#include <QDebug>
#include <QMouseEvent>
#include <QLineEdit>
#include <QInputDialog>

CmdButton::CmdButton(int prefix, QString cmdLine) : m_prefix(prefix), m_cmdLine(cmdLine)
{
    update();
    connect(this, &CmdButton::clicked, this, &CmdButton::runCmd);
    connect(this, &CmdButton::rightClicked, this, &CmdButton::editCmd);
}

void CmdButton::update() {
    setText(QString("&" + QString::number(m_prefix) + ": ") + m_cmdLine);
}

void CmdButton::runCmd()
{
    // qInfo() << __func__ << " " << m_cmdLine;
    if (m_cmdLine != "") {
        if (m_cmdLine == "^C") { // QQQ TODO: create a command dialog which can send Ctrl-C without hi-jacking the string "^C"
            emit execute(QString::fromLatin1("\x03"));
        } else {
            emit execute(m_cmdLine);
        }
    }
}

void CmdButton::editCmd()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Command") + QString::number(m_prefix),
                                         tr("Command:"), QLineEdit::Normal,
                                         m_cmdLine, &ok);

    if (ok) {
        m_cmdLine = text;
        setText(QString("&" + QString::number(m_prefix) + ": ") + m_cmdLine);
    }
}

void CmdButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton) {
        emit rightClicked();
    }
    else if (e->button()==Qt::LeftButton) {
        emit clicked();
    }
}
