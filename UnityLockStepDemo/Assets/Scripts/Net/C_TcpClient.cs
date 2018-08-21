using System;
using System.Collections.Concurrent;
using System.Net.Sockets;
using System.Threading;
using Mina.Core.Buffer;
using System.Net;

namespace TVR
{
    public class C_TcpClient : TcpClient
    {
        //接受soket数据的大小
        public static readonly int READ_SOCKET_BUFFER_SIZE = 1024 * 128;
        //发送soket数据的大小
        public static readonly int SEND_SOCKET_BUFFER_SIZE = 1024 * 10;
        //心跳的超时时间（单位s）
        public static readonly float PINGPONG_OVER_TIME = 5f;

        protected TcpClient m_mySocket;
        //socket是否已经设置
        protected volatile bool m_isSocketReady = false;
        protected volatile bool m_isConnecting = false;
        protected byte[] m_getData = new byte[READ_SOCKET_BUFFER_SIZE];
        protected IoBuffer m_readBuffer = ByteBuffer.Allocate(READ_SOCKET_BUFFER_SIZE);

        protected int m_currentSendBufferWritePosition = 0;
        protected byte[] m_sendData = new byte[SEND_SOCKET_BUFFER_SIZE];
        protected IoBuffer m_sendBuffer = ByteBuffer.Allocate(SEND_SOCKET_BUFFER_SIZE);

        protected System.Collections.Generic.Queue<MessageBody> m_recievedMsgs = new System.Collections.Generic.Queue<MessageBody>();
        protected System.Collections.Generic.Queue<MessageBody> m_sendMsgs = new System.Collections.Generic.Queue<MessageBody>();

        //测试连线使用
        byte[] testByte = new byte[1];

        /// <summary>
        /// for Client
        /// </summary>
        protected Timer m_getMsgThread;

        /// <summary>
        /// for Server
        /// </summary>
        //当前连线IP
        protected string m_CurHost = "Host";
        protected Int32 m_Port = 6000;

        protected string m_clientID;

        public string ClientID
        {
            get { return m_clientID; }
        }

        public bool IsConnected
        {
            get
            {
                return m_isSocketReady;
            }
        }

        public bool IsConnecting
        {
            get
            {
                return m_isConnecting;
            }
        }

        /// <summary>
        /// 是否连线状态
        /// </summary>
        /// <returns></returns>
        public bool DetectIsConnected()
        {
            bool bReturn = true;

            try
            {
                //lock (this)
                //{
                //检查读是否可读
                if (m_mySocket.Client.Poll(0, SelectMode.SelectRead))
                {
                    if (m_mySocket.Client.Receive(testByte, SocketFlags.Peek) == 0)
                    {
                        return false;
                    }
                }
                //}
            }
            catch
            {
                return false;
            }

            return bReturn;
        }

        public void ConnectSocket(string hostName, int port)
        {
            try
            {
                if (m_mySocket == null)
                {
                    m_mySocket = new TcpClient();
                }

                m_mySocket.BeginConnect(hostName, port, ConnectCallBack, null);
                m_isConnecting = true;
            }
            catch (Exception ex)
            {
                m_isConnecting = false;
                // TODO: 打印日志
                DEBUG.Error("连接服务器失败:{0}", ex.ToString());
            }
        }

        private void ConnectCallBack(IAsyncResult Ar)
        {
            try
            {
                m_isConnecting = false;

                //设置连接数据缓存池大小
                m_mySocket.ReceiveBufferSize = READ_SOCKET_BUFFER_SIZE;
                m_mySocket.SendBufferSize = SEND_SOCKET_BUFFER_SIZE;
                //设置及时模式
                m_mySocket.NoDelay = true;
                m_mySocket.Client.NoDelay = true;

                //结束异步连接请求
                m_mySocket.EndConnect(Ar);

                //uint IOC_IN = 0x80000000;
                //uint IOC_VENDOR = 0x18000000;
                //uint SIO_UDP_CONNRESET = IOC_IN | IOC_VENDOR | 12;
                //m_mySocket.Client.IOControl((int)SIO_UDP_CONNRESET, new byte[] { Convert.ToByte(false) }, null);

                m_isSocketReady = true;

                m_getMsgThread = new Timer(GetMsg);
                m_getMsgThread.Change(TimeSpan.FromMilliseconds(0), TimeSpan.FromMilliseconds(1));

                //获得当前链接地址
                m_CurHost = GetConnectIp();
                //当前链接端口
                m_Port = GetConnectPort();
            }
            catch (Exception ex)
            {
                // TODO: 打印日志
                DEBUG.Error("客户端启动消息接收线程失败:{0}", ex.ToString());
            }
        }

        public void CloseSocket()
        {
            if (m_getMsgThread != null)
            {
                m_getMsgThread.Dispose();
                m_getMsgThread = null;
            }

            if (m_mySocket != null)
            {
                try
                {
                    m_mySocket.Client.Shutdown(SocketShutdown.Both);
                }
                catch (Exception ex)
                {
                    // TODO: 打印日志
                    DEBUG.Error("断开连接失败:{0}", ex.ToString());
                }
                m_mySocket.Close();
            }
            m_mySocket = null;
            m_isSocketReady = false;
        }

        /// <summary>
        /// 获得当前TCP的IP
        /// </summary>
        /// <returns>IP</returns>
        public string GetConnectIp()
        {
            string strReturn = "0.0.0.0";

            if (m_isSocketReady == true)
            {
                IPEndPoint endPoint = (IPEndPoint)(m_mySocket.Client.RemoteEndPoint);
                strReturn = endPoint.Address.ToString();
            }

            return strReturn;
        }

        /// <summary>
        /// 获得当前链接的端口号
        /// </summary>
        /// <returns>端口号</returns>
        public int GetConnectPort()
        {
            int RevPort = -1;

            if (m_isSocketReady == true)
            {
                IPEndPoint endPoint = (IPEndPoint)(m_mySocket.Client.RemoteEndPoint);
                RevPort = endPoint.Port;
            }

            return RevPort;
        }

        public void Update()
        {
            if (m_mySocket == null)
                return;

            if (!m_isSocketReady)
                return;

            try
            {
                SendMsg();

                HandleRecieveMsg();
            }
            catch (Exception ex)
            {
                DEBUG.Error("处理消息失败:{0}", ex.ToString());
            }
        }

        public void GetMsg(object o)
        {
            while (m_isSocketReady)
            {
                int recieveLen = 0;
                try
                {
                    if (m_mySocket.Available > 0)
                    {
                        recieveLen = m_mySocket.Client.Receive(m_getData, 0, (int)m_mySocket.ReceiveBufferSize, SocketFlags.None);
                    }
                    else
                    {
                        System.Threading.Thread.Sleep(1);
                        break;
                    }
                }
                catch (Exception e)
                {
                    DEBUG.Error(e.Message);
                    break;
                }

                m_readBuffer.Put(m_getData, 0, recieveLen);
                Array.Clear(m_getData, 0, m_getData.Length);
                m_readBuffer.Flip();
                int len = GetPacketLength(m_readBuffer);

                if (len < 0)
                {
                    break;
                }

                MessageBody msg = new MessageBody();
                CPacket.PopPacketLength(m_readBuffer);
                msg.type = (ushort)CPacket.PopPacketType(m_readBuffer);
                byte[] data = new byte[len - CPacket.HEADER_TYPE_BYTES];
                m_readBuffer.Get(data, 0, len - CPacket.HEADER_TYPE_BYTES);
                msg.data = data;

                lock(this)
                {
                    m_recievedMsgs.Enqueue(msg);
                }

                bool isFinish = false;
                if (m_readBuffer.HasRemaining)
                {
                    m_readBuffer.Compact();
                }
                else
                {
                    m_readBuffer.Clear();
                    isFinish = true;
                }

                if (isFinish)
                    break;

            }
        }

        private int GetPacketLength(IoBuffer buff)
        {
            if (buff.Remaining < CPacket.HEADER_TOTAL_BYTES)
            {
                return -1;
            }
            int _len = CPacket.PeekPacketLength(buff);
            if (buff.Remaining >= _len + CPacket.HEADER_LENGTH_BYTES)
            {
                return _len;
            }
            return -1;
        }

        public void SendMsg()
        {
            //lock (this)
            {
                while (m_sendMsgs.Count > 0)
                {
                    MessageBody msg = m_sendMsgs.Dequeue();
                    if (msg != null)
                    {
                        m_sendBuffer.Clear();
                        Array.Clear(m_sendData, 0, m_sendData.Length);

                        int len = CPacket.HEADER_TYPE_BYTES + msg.data.Length;
                        m_sendBuffer.PutInt32(len);
                        m_sendBuffer.PutInt16((short)msg.type);
                        m_sendBuffer.Put(msg.data);
                        m_sendBuffer.Flip();

                        m_sendBuffer.Get(m_sendData, 0, len + CPacket.HEADER_LENGTH_BYTES);
                        m_currentSendBufferWritePosition = len + CPacket.HEADER_LENGTH_BYTES;
                        SendMsg(m_sendData);
                    }
                }
            }
        }

        private void SendMsg(byte[] msg)
        {
            if (m_mySocket == null)
                return;

            if (!m_isSocketReady)
                return;

            //lock (this)
            {
                if (m_mySocket.Client != null && m_mySocket.Client.Connected)
                {
                    try
                    {
                        m_mySocket.Client.Send(msg, 0, m_currentSendBufferWritePosition, SocketFlags.None);
                        m_currentSendBufferWritePosition = 0;
                    }
                    catch (Exception ex)
                    {
                        m_currentSendBufferWritePosition = 0;
                        DEBUG.Error("SendMsg Error: {0}", ex.Message.ToString());
                    }
                }
            }
        }

        public void SendMsg(MessageBody msg)
        {
            if (m_mySocket == null)
                return;

            if (!m_isSocketReady)
                return;

            m_sendMsgs.Enqueue(msg);
        }

        private void HandleRecieveMsg()
        {
            lock(this)
            {
                while (m_recievedMsgs.Count > 0)
                {
                    MessageBody msg = m_recievedMsgs.Dequeue();

                    if (msg != null)
                        OnMessageProc(msg);
                }
            }
        }

        void OnMessageProc(MessageBody body)
        {
            if (body == null)
                return;

            MessageCenter.Instance.DoEvent(body.type, body);
        }
    }
}
