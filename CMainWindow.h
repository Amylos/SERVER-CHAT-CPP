#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <CServeur.h>

QT_BEGIN_NAMESPACE
namespace Ui { class CMainWindow; }
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::CMainWindow *ui;
    CServeur*       m_pServeur;

public:
    CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

    void MessageLog(const QString& msg);
    void MessageChat(const QString& msg);

private slots:
    void on_pb_connectDisconnect_clicked();

    void on_pb_clearChat_clicked();

    void on_pb_clearLog_clicked();



};
#endif // CMAINWINDOW_H
