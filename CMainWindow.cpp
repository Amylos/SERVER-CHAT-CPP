#include "CMainWindow.h"
#include "ui_CMainWindow.h"

#include<iostream>

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CMainWindow)
    , m_pServeur{}
{
    ui->setupUi(this);
    m_pServeur = new CServeur(this);

    ui->le_serveurPort->setText("20000");

    if(m_pServeur) MessageLog("Serveur Created");
}

CMainWindow::~CMainWindow()
{
    delete ui;
}


void CMainWindow::on_pb_connectDisconnect_clicked(){

    if(m_pServeur->IsConnected()){
       if(m_pServeur->Disconnect()== 0){
            ui->pb_connectDisconnect->setText("Connect");
       }
    }
    else{
        if(m_pServeur->Connect(ui->le_serveurPort->text()) == 0){
            ui->pb_connectDisconnect->setText("Disconnect");
        }
    }
}


void CMainWindow::on_pb_clearChat_clicked(){

    ui->lw_chat->clear();
}

void CMainWindow::on_pb_clearLog_clicked(){

    ui->lw_log->clear();
}

void CMainWindow::MessageLog(const QString& msg){

    ui->lw_log->addItem(msg);
}

void CMainWindow::MessageChat(const QString& msg){

    ui->lw_chat->addItem(msg);
}
