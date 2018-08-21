////////////////////////////////////////////////////////////////////////
//
// 作者：Kevin
// 时间：
// 功能：
//
///////////////////////////////////////////////////////////////////////


using System.Collections;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System;

namespace TVR
{
    public class TCPClient : ISingleton<TCPClient>
    {
        public string host = "192.168.1.102";
        public int port = 10086;

        C_TcpClient tcpClient = new C_TcpClient();

        public bool Connected
        {
            get
            {
                if (tcpClient == null)
                    return false;

                return tcpClient.IsConnected;
            }
        }

        protected override void InitInstance()
        {
            OnConnect();
        }

        protected override void ClearInstance()
        {
            Disconnection();
        }

        public void Fini()
        {
            ClearInstance();
        }

        public void Update()
        {
            if (tcpClient != null)
                tcpClient.Update();
        }

        public void OnConnect()
        {
            tcpClient.ConnectSocket(host, port);
        }


        public void Disconnection()
        {
            if (tcpClient != null)
                tcpClient.CloseSocket();
        }

        public void Send(MessageBody body)
        {
            tcpClient.SendMsg(body);
        }
    }

} // end of namespace TVR
