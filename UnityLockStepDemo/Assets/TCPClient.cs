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

namespace TVR
{
    public class TCPClient : ISingletonBehaviour<TCPClient>
    {
        public string host = "192.168.1.102";
        public int port = 10010;
        private Socket tcpSocket;

        byte[] m_buffer_recv = new byte[8192];
        private int m_lastPos = 0;

        private Thread m_thread = null;

        System.Collections.Generic.List<MessageBody> m_messageList = new System.Collections.Generic.List<MessageBody>();

        private void Start()
        {
            OnConnect();
        }

        private void Update()
        {
            lock(((ICollection)m_messageList).SyncRoot)
            {
                if(m_messageList.Count > 0)
                {
                    OnMessageProc(m_messageList[0]);
                    m_messageList.RemoveAt(0);
                }
            }
        }

        private void Recv(Socket sock)
        {
            if (!sock.Connected)
                return;

            //接收消息
            int nLenRecv = sock.Receive(m_buffer_recv, 0, m_buffer_recv.Length, SocketFlags.None);
            m_lastPos += nLenRecv;
            if (nLenRecv <= 0)
            {
                //Debug.Log("Recv Disconnection From Server");
                Disconnection();
                return;
            }

            while (true)
            {
                if (m_lastPos < MessageBody.HEADER_SIZE)
                    break;

                int len = GetPackageLength();
                if (len < 0)
                    break;

                MessageBody body = new MessageBody();
                body.size = System.BitConverter.ToUInt32(m_buffer_recv, 0);
                body.type = System.BitConverter.ToUInt16(m_buffer_recv, sizeof(System.UInt32));
                body.data = new byte[body.size - MessageBody.HEADER_TYPE_BYTES];
                System.Array.Copy(m_buffer_recv, sizeof(System.UInt32) + sizeof(System.UInt16), body.data, 0, body.size - MessageBody.HEADER_TYPE_BYTES);

                m_lastPos = m_lastPos - ((int)body.size + MessageBody.HEADER_LEN_BYTES);

                m_messageList.Add(body);

                bool isFinish = false;
                if (m_lastPos <= 0)
                    isFinish = true;

                if (isFinish)
                    break;
            }
        }

        public void OnConnect()
        {
            //创建socket
            tcpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            //连接服务器
            tcpSocket.Connect(IPAddress.Parse(host), port);
            UnityEngine.Debug.Log("连接服务器");

            m_thread = new Thread(() => OnRun(tcpSocket));
            m_thread.Start();

        }

        void OnRun(Socket sock)
        {
            Recv(sock);
        }

        public void Disconnection()
        {
            if (tcpSocket == null)
                return;

            if (m_thread != null)
            {
                m_thread.Abort();
                m_thread = null;
            }

            tcpSocket.Close();
        }

        static byte[] tmp = new byte[1024 * 128];
        int GetPackageLength()
        {
            if (m_lastPos < MessageBody.HEADER_SIZE)
                return -1;

            int len = -1;
            System.Array.Copy(m_buffer_recv, tmp, MessageBody.HEADER_LEN_BYTES);
            len = System.BitConverter.ToInt32(tmp, 0);
            if (m_lastPos >= len + MessageBody.HEADER_LEN_BYTES)
                return len;

            return -1;
        }

        public void Send(MessageBody body)
        {
            byte[] bytes = body.Serialize();
            tcpSocket.Send(bytes);
        }

        void OnMessageProc(MessageBody body)
        {
            MessageCenter.Instance.DoEvent(body.type, body);
        }

    }

} // end of namespace TVR
