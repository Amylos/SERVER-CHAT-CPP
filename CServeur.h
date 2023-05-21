#ifndef CSERVEUR_H
#define CSERVEUR_H


#include<QString>
#include<thread>

#include <winsock2.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#define Cleanup() WSACleanup()
typedef int socklen_t;


#define mBitsSet(f,m)		((f)|=(m))
#define mBitsClr(f,m)		((f)&=(~(m)))
#define mBitsTgl(f,m)		((f)^=(m))
#define mBitsMsk(f,m)		((f)& (m))
#define mIsBitsSet(f,m)		(((f)&(m))==(m))
#define mIsBitsClr(f,m)		(((~(f))&(m))==(m))

class CMainWindow;

class CServeur {
private:
    enum e_statusMasks:uint32_t{
        ST_ALL_CLEARED       = 0x00000000,
        ST_LINKED            = 0x00000001,
        ST_WSADATA_ERROR     = 0x8000000,
        ST_WSADATA_INITIATED = 0x40000000,
    };
private:
    int 			m_sock_error;
    uint32_t		m_uStatus;
    /* SOCKET et contexte d'adressage du Serveur */
    SOCKADDR_IN 	m_sin;
    SOCKET 			m_sock;
    socklen_t 		m_recsize;
    /* SOCKET et contexte d'adressage du client */
    SOCKADDR_IN 	m_csin;
    SOCKET 			m_csock;
    socklen_t 		m_crecsize;

    CMainWindow*    m_pMainWindow;

    std::thread*    m_pListenThread;
    std::thread*    m_pAcceptThread[10];

public:
    CServeur(CMainWindow*pMainWindow);
    ~CServeur();

    void Start();

    int Connect(const QString& serverPort);

    int Run();

    int Disconnect();

    int IsConnected()const;

    int Send(const char*message, unsigned long len,const char*Id);
    int Receive(char*buffer, unsigned long len);



//    int static A(int a);    int static B(int b);

    int static ListenThread(CServeur*pServeur);
    int static AcceptThread(CServeur*pServeur);
};



#endif // CSERVEUR_H
