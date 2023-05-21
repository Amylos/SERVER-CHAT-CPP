#include <CServeur.h>
#include <iostream>
#include <CMainWindow.h>


using namespace std;

CServeur::CServeur(CMainWindow*pMainWindow):
    m_sock_error(),
    m_uStatus{ST_ALL_CLEARED},
    /* Serveur Configuration */
    m_sin{(int)INVALID_SOCKET},
    m_sock{},
    m_recsize{sizeof m_sin},
     /* Client Configuration */
    m_csin{(int)INVALID_SOCKET},
    m_csock{},
    m_crecsize{sizeof m_csin},
    m_pMainWindow(pMainWindow)
{
    Start();
}

CServeur::~CServeur(){
    Disconnect();
    if(mBitsSet(m_uStatus,ST_WSADATA_INITIATED)){
            WSACleanup();
        mBitsClr(m_uStatus,ST_WSADATA_INITIATED);
    }
}


void CServeur::Start(){
    WSADATA WSAData;
    int error = WSAStartup(MAKEWORD(2,2),&WSAData);
    if(error){
        mBitsSet(m_uStatus,ST_WSADATA_ERROR);
    }
    else{
         mBitsSet(m_uStatus,ST_WSADATA_INITIATED);
    }
}

int CServeur::Disconnect(){
    if(mIsBitsClr(m_uStatus, ST_LINKED)) return (int)INVALID_SOCKET;
    mBitsClr(m_uStatus,ST_LINKED);
    m_pMainWindow->MessageLog(QString("DISCONNECTED "));

    shutdown(m_csock, 2);
    closesocket(m_csock);
    closesocket(m_sock);


    m_pAcceptThread[0]->join();
    delete m_pAcceptThread[0]; m_pAcceptThread[0]=nullptr;

    m_pListenThread->join();
    delete m_pListenThread; m_pListenThread=nullptr;



    WSACleanup();

    m_sock=INVALID_SOCKET;
    return 0;
}

int CServeur::Connect(const QString& serverPort){
    Start();
    if(mIsBitsSet(m_uStatus,ST_WSADATA_INITIATED)){
        /**** CREATION DU SOCKET *****/

        m_sock = socket(AF_INET,SOCK_STREAM,0);

        if(m_sock != INVALID_SOCKET){
            m_pMainWindow->MessageLog(QString("The socket ") + QString::number(m_sock) + QString(" is now opened in TCP/IP"));
            cout << "The socket " << m_sock << " is now opened in TCP/IP" << endl;

            m_sin.sin_addr.s_addr = htonl(INADDR_ANY);	/* Adresse IP autorisée */
            m_sin.sin_family = AF_INET;	/* Type de connexion */
            m_sin.sin_port = htons(serverPort.toShort()); /* Port */
            m_sock_error = bind(m_sock,(SOCKADDR*)&m_sin,m_recsize);
            mBitsSet(m_uStatus,ST_LINKED);
        }
        else{
            m_pMainWindow->MessageLog(QString(" Invalid socket "));
            cout << "Invalid socket" << endl;
            return (int)INVALID_SOCKET;
        }

        m_pListenThread = new thread(ListenThread,this);



        return 0;

    }
    return (int)INVALID_SOCKET;
}



/*static*/ int CServeur::ListenThread(CServeur* pServeur){

    if(mIsBitsSet(pServeur->m_uStatus,ST_LINKED)){
        pServeur->m_pMainWindow->MessageLog(QString("CONNECTED "));
        if(pServeur->m_sock_error != SOCKET_ERROR){
            pServeur->m_pMainWindow->MessageLog(QString("Listen on port ") + QString::number(pServeur->m_sin.sin_port));
            cout << "Listen on port " << pServeur->m_sin.sin_port << endl;
            pServeur->m_sock_error = listen(pServeur->m_sock,5);

//            while(mIsBitsSet(pServeur->m_uStatus,ST_LINKED)){
//                pServeur->m_sock_error = listen(pServeur->m_sock,5);

//                 Si nouveau client je crée un nouveau thread

//                int k = 0;
//                if(k<10){
//                    for(k; k<10;k++){
                        pServeur->m_pAcceptThread[0] = new thread(AcceptThread,pServeur);
//                    }
//                 }
//            }


        }
    }
    return 0;
}



int CServeur::AcceptThread(CServeur*pServeur){

    if(pServeur->m_sock_error != SOCKET_ERROR){ // Création thread

        pServeur->m_csock = accept(pServeur->m_sock, (SOCKADDR*)&pServeur->m_csin,&pServeur->m_crecsize);
        cout << "Client is connected with the socket " <<  pServeur->m_csock << " from :" <<  inet_ntoa(pServeur->m_csin.sin_addr) << " " <<  htons(pServeur->m_csin.sin_port) << endl;

        int res=1;
        int cpt=0;
        char buffrecv[BUFSIZ];
        char buffsend[BUFSIZ];
        char buffId[BUFSIZ];

        pServeur->Receive(buffId, sizeof buffId);
        cout << buffId << " is connected" << endl;
        pServeur->m_pMainWindow->MessageChat(QString(buffId) + QString(" is connected") );

        pServeur->m_pMainWindow->MessageLog(QString(buffId) + QString(" is connected") );

        while(res){
           switch(pServeur->Receive(buffrecv, sizeof buffrecv)){
           case SOCKET_ERROR:
               break;
           case 0:
           case 1:
               res = 0;
               break;
           default:
               if(mIsBitsClr(pServeur->m_uStatus,ST_LINKED)){
                   res=0;
                   break;
               }

               strcpy(buffsend,buffrecv);
               cpt++;
               cout << buffrecv << cpt << endl;
               pServeur->Send(buffsend, sizeof buffsend,buffId);
               break;
            }
        }
        pServeur->m_pMainWindow->MessageLog(QString(buffId) + QString(" is disconnected"));

        pServeur->Send(buffsend, sizeof buffsend,buffId);
    }
    return 0;
}


int CServeur::Receive(char* buffer,unsigned long len){
    m_sock_error=recv(m_csock, buffer, len, 0);
    m_pMainWindow->MessageLog(QString("DATA RECEIVED : ") + QString(buffer));
    cout << " DATA RECEIVED : " << buffer << endl;

    return m_sock_error;
}

int CServeur::Send(const char*buffer,unsigned long len,const char*Id){
    m_sock_error=send(m_csock, buffer, len, 0);
    m_pMainWindow->MessageLog(QString("DATA SENT : ") + QString(Id) + QString(buffer));
    m_pMainWindow->MessageChat(QString(Id) + QString(" : ") + QString(buffer));
    cout << " DATA SENT : " << buffer << endl;

    return m_sock_error;
}

int CServeur::IsConnected()const{
   return mIsBitsSet(m_uStatus,ST_LINKED);
}








/****** TRASH *****/

//            pServeur->m_sock_error = listen(pServeur->m_sock,5);
//            pServeur->m_pMainWindow->MessageLog(QString("Listen on port ") + QString::number(pServeur->m_sin.sin_port));
//            cout << "Listen on port " << pServeur->m_sin.sin_port << endl;
//            // Si nouveau client je crée un nouveau thread
//            pServeur->m_pAcceptThread = new thread(AcceptThread,pServeur);

//        if(mIsBitsSet(m_uStatus,ST_LINKED)){
//            m_pMainWindow->MessageLog(QString("CONNECTED "));
//            if(m_sock_error != SOCKET_ERROR){
//                m_sock_error = listen(m_sock,5);
//                m_pMainWindow->MessageLog(QString("Listen on port ") + QString::number(m_sin.sin_port));
//                cout << "Listen on port " << m_sin.sin_port << endl;
//                m_pAcceptThread = new thread(AcceptThread,this);
//                if(m_sock_error != SOCKET_ERROR){ // Création thread

//                    m_csock = accept(m_sock, (SOCKADDR*)&m_csin,&m_crecsize);
//                    cout << "Client is connected with the socket " <<  m_csock << " from :" <<  inet_ntoa(m_csin.sin_addr) << " " <<  htons(m_csin.sin_port) << endl;

//                    int res=1;
//                    int cpt=0;
//                    char buffrecv[BUFSIZ];
//                    char buffsend[BUFSIZ];

//                    Receive(buffrecv, sizeof buffrecv);
//                    cout << buffrecv << endl;

//                    strcpy(buffsend,buffrecv);

//                    Send(buffsend, sizeof buffsend);
//                    cout << buffsend << endl;

//                    while(res){
//                       switch(Receive(buffrecv, sizeof buffrecv)){
//                       case SOCKET_ERROR:
//                           break;
//                       case 1:
//                           res = 0;
//                           break;
//                       default:
//                           strcpy(buffsend,buffrecv);
//                           cpt++;
//                           cout << buffrecv << cpt << endl;
//                           m_pMainWindow->MessageChat(QString(buffsend));
//                           Send(buffsend, sizeof buffsend);
//                           break;
//                        }
//                    }
//                }
//                else{
//                   cout << " Listen error " << endl;
//                }
//            }
//        }


//            if(pServeur->m_sock_error != SOCKET_ERROR){ // Création thread
//                  pServeur->m_pAcceptThread = new thread(AcceptThread,pServeur);
//                pServeur->m_csock = accept(pServeur->m_sock, (SOCKADDR*)&pServeur->m_csin,&pServeur->m_crecsize);
//                cout << "Client is connected with the socket " <<  pServeur->m_csock << " from :" <<  inet_ntoa(pServeur->m_csin.sin_addr) << " " <<  htons(pServeur->m_csin.sin_port) << endl;

//                int res=1;
//                int cpt=0;
//                char buffrecv[BUFSIZ];
//                char buffsend[BUFSIZ];

//                pServeur->Receive(buffrecv, sizeof buffrecv);
//                cout << buffrecv << endl;

//                strcpy(buffsend,buffrecv);

//                pServeur->Send(buffsend, sizeof buffsend);
//                cout << buffsend << endl;

//                while(res){
//                   switch(pServeur->Receive(buffrecv, sizeof buffrecv)){
//                   case SOCKET_ERROR:
//                       break;
//                   case 1:
//                       res = 0;
//                       break;
//                   default:
//                       strcpy(buffsend,buffrecv);
//                       cpt++;
//                       cout << buffrecv << cpt << endl;
//                       pServeur->Send(buffsend, sizeof buffsend);
//                       break;
//                    }
//                }
//            }
//            else{
//               cout << " Listen error " << endl;
//            }


//int CServeur::Run(){

//    if(mIsBitsSet(m_uStatus,ST_LINKED)){
//        m_pMainWindow->MessageLog(QString("CONNECTED "));
//        if(m_sock_error != SOCKET_ERROR){
//           m_sock_error = listen(m_sock,5);
//            m_pMainWindow->MessageLog(QString("Listen on port ") + QString::number(m_sin.sin_port));
//            cout << "Listen on port " << m_sin.sin_port << endl;

//            if(m_sock_error != SOCKET_ERROR){ // Création thread

////            thread a(A,5);
////            thread b(B,5);
////            thread a(Accept,this,0);
////            thread b(Accept,this,1);
////                m_csock = accept(m_sock, (SOCKADDR*)&m_csin,&m_crecsize);

//                cout << "Client is connected with the socket " <<  m_csock << " from :" <<  inet_ntoa(m_csin.sin_addr) << " " <<  htons(m_csin.sin_port) << endl;

//                int res=1;

//                char buf[BUFSIZ];
//                char bufS[BUFSIZ];
//                int cpt=0;

//                while(res){
//                   switch(res=recv(m_csock, buf, sizeof buf, 0)){
//                   case SOCKET_ERROR:
//                       break;
//                   case 1:
//                       res = 0;
//                       break;
//                   default:
//                       cpt++;
//                       cout << buf << cpt << endl;
//                       m_pMainWindow->MessageChat(QString(bufS));
//                       m_sock_error=send(m_csock, bufS, strlen(bufS)+1, 0);
//                       break;
//                    }
//                }
//                cout << "QUITTER" << endl;
//            }
//            else{
//                cout << " Listen error " << endl;
//            }
//        }
//    }
//    else{
//        m_pMainWindow->MessageLog(QString("DISCONNECTED "));
//        return (int)INVALID_SOCKET;
//    }
//    return 0;
//}




//int CServeur::A(int a){

//    a = 1;
//    while(a <40000){
//        a++;
//        cout << "Thread A " << a << endl;
//    }
//    return 0;
//}

//int CServeur::B(int b){
//    b = 1;
//    while(b > -40000){
//        b--;
//        cout << "Thread B " << b << endl;
//    }
//    return 0;
//}
