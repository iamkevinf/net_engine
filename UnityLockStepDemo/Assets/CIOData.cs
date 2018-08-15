using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using UnityEngine;

//*************************************************************************
//文件名(File Name):		CIOData
//作者(Author):			    杨子润
//日期(Create Date):		 2015.11.10
//功能(Function):			简单的封装了一下读写信息的类
//					        用于消息的二进制化,可以简单的使用,用于读取和写入信息
//                          *因为使用byte[] 所以之后也许会修改使用stream,但是因为一直都是使用byte[]所以暂时不修改
//修改记录(RevisionHistory): 
//20160722 添加常用类型的unsafe指针拷贝压入,优化效率和GC
//20160305 添加V2 V3 V4基本类型,同时兼容unity设置
//20160413 添加弹出内容最大限制,添加针对消息头的固定压入弹出方法,防止错误消息造成异常
//20160414 删除消息头的压入弹出方式,还是使用byte直接对比
//*************************************************************************
namespace TVR_Server.Basic
{
    public class CIOData
    {
        public static readonly int DATA_COUNT_MAX = 1024 * 1024 * 10;//最大一次获得数据10M

        protected byte[] _Data;//二进制内容
        protected int _Size;//当前数据大小
        protected int _CurReadIndex;//当前读数据位置
        protected int _CurWritaIndex;//当前写数据位置
        protected IntPtr _ptr;

        static TDicPool<int, CIOData> g_IODataPool;

        static TDicPool<int, CIOData> Pool
        {
            get
            {
                if (g_IODataPool == null)
                {
                    g_IODataPool = new TDicPool<int, CIOData>(100);
                }

                return g_IODataPool;
            }
        }

        static public CIOData GetCIOData()
        {
            return Pool.GetObj(0);
        }

        static public void BackCIOData(CIOData Obj)
        {
            Obj.Clear();
            Pool.BackObj(0, Obj);
        }

        /// <summary>
        /// 创建一个读写资料数据类型
        /// </summary>
        /// <param name="m_Size">数据大小</param>
        public CIOData(int Size)
        {
            _Data = new byte[Size];
            _CurReadIndex = 0;
            _CurWritaIndex = 0;
            _Size = 0;
        }

        /// <summary>
        /// 默认构造函数
        /// </summary>
        public CIOData()
        {
            _Data = new byte[2048];
            _CurReadIndex = 0;
            _CurWritaIndex = 0;
            _Size = 0;
        }
        /// <summary>
        /// 
        /// </summary>
        public void SetData(byte[] data)
        {
            _Data = data;
            _CurReadIndex = 0;
            _CurWritaIndex = 0;
            _Size = data.Length;
        }

        /// <summary>
        /// 外部使用的清理资料
        /// </summary>
        public void Clear()
        {
            Init();
        }

        /// <summary>
        /// 资料全部重置
        /// </summary>
        void Init()
        {
            Array.Clear(_Data, 0, _Data.Length);
            _CurReadIndex = 0;
            _CurWritaIndex = 0;
            _Size = 0;
        }

        /// <summary>
        /// 重置一下资料位置,因为是顺序读写,所以需要重新拷贝一下
        /// </summary>
        void Reset()
        {
            byte[] TmpData = new byte[_Size];
            Array.Copy(_Data, _CurReadIndex, TmpData, 0, _Size);

            Init();
            _CurReadIndex = 0;
            Array.Copy(TmpData, 0, _Data, 0, TmpData.Length);
            _CurWritaIndex = TmpData.Length;
            _Size = _CurWritaIndex;
        }

        /// <summary>
        /// 外部得到资料包大小
        /// </summary>
        public int Size
        {
            get
            {
                return _Size;
            }
        }

        /// <summary>
        /// 扩展一下资料大小
        /// </summary>
        /// <param name="m_Size">扩展大小,如果小于0则为当前大小2倍</param>
        void Expand(int Size = -1)
        {
            if (Size > 0)
            {
                _Size = _Data.Length + Size;
            }
            else
            {
                _Size = _Data.Length * 2;
            }

            //申请新的数组大小
            byte[] TmpData = new byte[_Size];
            //拷贝当前内容到新数组
            _Size = _CurWritaIndex - _CurReadIndex;
            Array.Copy(_Data, _CurReadIndex, TmpData, 0, _Size);
            //设置写入位置
            _CurWritaIndex = _Size;
            _CurReadIndex = 0;
            //替换旧数组
            _Data = TmpData;
        }


        /// <summary>
        /// 检查和扩展资料大小,主要用于写资料检查
        /// </summary>
        /// <param name="WriteSize">写入资料大小</param>
        void CheckAndFixSize(int WriteSize)
        {
            //如果长度够写入退出
            if (WriteSize <= (_Data.Length - _CurWritaIndex))
            {
                return;
            }

            //如果写入长度比本身长度还大,扩展到需要的长度
            if ((WriteSize - _Data.Length) > 0)
            {
                Expand(WriteSize);
                return;
            }

            //如果只是写到后半段不够,直接重置一下
            if ((_CurReadIndex - WriteSize) > 0)
            {
                Reset();
                return;
            }

            //扩展一倍大小
            Expand();
        }

        /// <summary>
        /// 压入一个T型的资料
        /// </summary>
        /// <typeparam name="T">资料的类型</typeparam>
        /// <param name="PushSData">压入资料</param>
        public void PushData<T>(T PushSData) where T : struct
        {
            int pushSize = Marshal.SizeOf(PushSData);

            CheckAndFixSize(pushSize);

            _ptr = Marshal.AllocHGlobal(pushSize);
            Marshal.StructureToPtr(PushSData, _ptr, true);
            Marshal.Copy(_ptr, _Data, _CurWritaIndex, pushSize);
            Marshal.FreeHGlobal(_ptr);

            _CurWritaIndex += pushSize;
            _Size += pushSize;
        }

        /// <summary>
        /// 压入一个byte
        /// </summary>
        /// <param name="Data"></param>
        public void PushData(byte Data)
        {
            CheckAndFixSize(1);
            _Data[_CurWritaIndex] = Data;
            _CurWritaIndex++;
            _Size++;
        }

        /// <summary>
        /// 以下部分为性能优化特化部分
        /// </summary>
        /// <param name="Data"></param>
        /// ********************************************
        public unsafe void PushData(long Data)
        {
            CheckAndFixSize(8);
            fixed (byte* point = &_Data[_CurWritaIndex])
                *((long*)point) = Data;
            _CurWritaIndex += 8;
            _Size += 8;
        }

        public unsafe void PushData(ulong Data)
        {
            CheckAndFixSize(8);
            fixed (byte* point = &_Data[_CurWritaIndex])
                *((ulong*)point) = Data;
            _CurWritaIndex += 8;
            _Size += 8;
        }

        public unsafe void PushData(double Data)
        {
            PushData(*(long*)&Data);
        }

        public unsafe void PushData(int Data)
        {
            CheckAndFixSize(4);
            fixed (byte* point = &_Data[_CurWritaIndex])
                *((int*)point) = Data;
            _CurWritaIndex += 4;
            _Size += 4;
        }

        public void PushData(uint Data)
        {
            PushData((int)Data);
        }

        public unsafe void PushData(float Data)
        {
            PushData(*(int*)&Data);
        }

        public unsafe void PushData(short Data)
        {
            CheckAndFixSize(2);
            fixed (byte* point = &_Data[_CurWritaIndex])
                *((short*)point) = Data;
            _CurWritaIndex += 2;
            _Size += 2;
        }

        public void PushData(ushort Data)
        {
            PushData((short)Data);
        }

        //**************************************************

        /// <summary>
        /// 压入一个byte
        /// </summary>
        /// <param name="Data"></param>
        public void PushByte(byte Data)
        {
            PushData(Data);
        }

        /// <summary>
        /// 获得一个T型的资料
        /// </summary>
        /// <typeparam name="T">资料类型</typeparam>
        /// <param name="PopData">获取资料的引用</param>
        public void GetData<T>(ref T PopData) where T : struct
        {
            int Popsize = Marshal.SizeOf(PopData);

            if (Popsize > _Size)
            {
                Init();
                ShowMsg(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", PopData.GetType(), Popsize));
                throw new IndexOutOfRangeException(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", PopData.GetType(), Popsize));
            }

            _ptr = Marshal.AllocHGlobal(Popsize);
            Marshal.Copy(_Data, _CurReadIndex, _ptr, Popsize);
            PopData = (T)Marshal.PtrToStructure(_ptr, PopData.GetType());
            Marshal.FreeHGlobal(_ptr);

            _CurReadIndex += Popsize;
            _Size -= Popsize;
        }

        /// <summary>
        /// 获得一个byte[]资料,更具传入引用数组的大小决定获得大小,最大不超过当前资料大小
        /// </summary>
        /// <param name="PopData">获得资料byte[]对象应用</param>
        public void GetData(ref byte[] PopData)
        {
            if (PopData.Length > DATA_COUNT_MAX)
            {
                Init();
                ShowMsg(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "GetData", PopData.Length));
                throw new IndexOutOfRangeException(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "GetData", PopData.Length));
            }
            int DataSize = PopData.Length;

            if (_Size < DataSize)
            {
                DataSize = _Size;
            }

            Array.Copy(_Data, _CurReadIndex, PopData, 0, DataSize);
            _CurReadIndex += DataSize;
            _Size -= DataSize;

        }

        public void CopyData(ref byte[] PopData, int OffSet = 0, int GetSize = -1)
        {
            int DataSize = GetSize < 0 ? PopData.Length : GetSize;

            if (_Size < DataSize)
            {
                DataSize = _Size;
            }

            if (PopData.Length < OffSet + DataSize)
            {
                throw new IndexOutOfRangeException(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "CopyData", PopData.Length));
            }

            Array.Copy(_Data, _CurReadIndex, PopData, OffSet, DataSize);
        }

        /// <summary>
        /// 获得一个byte[]资料,更具传入引用数组的大小决定获得大小,最大不超过当前资料大小
        /// </summary>
        /// <param name="PopData">获得资料byte[]对象应用</param>
        public void GetData(CIOData PopData)
        {
            if (PopData == null)
            {
                return;
            }

            GetLimitLenData(PopData, Size);
        }

        /// <summary>
        /// 弹出所有资料
        /// </summary>
        /// <returns></returns>
        public byte[] PopAllData(bool IsClearData = true)
        {
            byte[] Rev = new byte[_Size];
            if (!IsClearData)
            {
                Array.Copy(_Data, _CurReadIndex, Rev, 0, _Size);
            }
            else
            {
                GetData(ref Rev);
            }

            return Rev;
        }

        /// <summary>
        /// 弹出一定长度的资料
        /// </summary>
        /// <param name="Len">资料长度,如果当前资料长度不够,后面为空</param>
        /// <returns></returns>
        public byte[] PopLimitLenData(int Len)
        {
            if (Len > DATA_COUNT_MAX)
            {
                Init();
                ShowMsg(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "LimitData", Len));
                throw new IndexOutOfRangeException(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "LimitData", Len));
            }
            byte[] Rev = new byte[Len];
            GetData(ref Rev);
            return Rev;
        }

        /// <summary>
        /// 获得限制长度的资料
        /// </summary>
        /// <param name="PopData"></param>
        public void GetLimitLenData(byte[] PopData, int Len)
        {
            if (PopData == null)
            {
                return;
            }

            if (Len > DATA_COUNT_MAX)
            {
                Init();
                ShowMsg(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "GetLimitLenData", Len));
                throw new IndexOutOfRangeException(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "GetLimitLenData", Len));
            }

            int DataSize = Len;

            if (_Size < DataSize)
            {
                DataSize = _Size;
            }

            Array.Copy(_Data, _CurReadIndex, PopData, 0, DataSize);
            _CurReadIndex += DataSize;
            _Size -= DataSize;
        }

        /// <summary>
        /// 获得限制长度的资料
        /// </summary>
        /// <param name="PopData"></param>
        public void GetLimitLenData(CIOData PopData, int Len)
        {
            if (PopData == null)
            {
                return;
            }
            if (Len > DATA_COUNT_MAX)
            {
                Init();
                ShowMsg(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "GetLimitLenData", Len));
                throw new IndexOutOfRangeException(string.Format("想要获得一个错误长度的数据类型{0}-size:{1}", "GetLimitLenData", Len));
            }

            PopData.CheckAndFixSize(Len);
            int DataSize = Len;

            if (_Size < DataSize)
            {
                DataSize = _Size;
            }

            Array.Copy(_Data, _CurReadIndex, PopData._Data, PopData._CurWritaIndex, DataSize);
            _CurReadIndex += DataSize;
            _Size -= DataSize;

            PopData._CurWritaIndex += DataSize;
            PopData._Size += DataSize;
        }

        /// <summary>
        /// 压入一个string类型的资料
        /// </summary>
        /// <param name="PushStrData">压入string</param>
        public void PushData(string PushStrData)
        {
            byte[] StrBytes = UTF8Encoding.UTF8.GetBytes(PushStrData);
            int Len = StrBytes.Length;

            PushData(Len);
            PushData(StrBytes);
        }

        /// <summary>
        /// 压入一个byte[]类型的资料
        /// </summary>
        /// <param name="PushBytesData"></param>
        public void PushData(byte[] PushBytesData)
        {
            if (PushBytesData == null)
            {
                return;
            }
            int pushSize = PushBytesData.Length;

            CheckAndFixSize(pushSize);

            Array.Copy(PushBytesData, 0, _Data, _CurWritaIndex, pushSize);

            _CurWritaIndex += pushSize;
            _Size += pushSize;
        }

        /// <summary>
        /// 压入一个byte[]类型的资料,限制长度,支持
        /// </summary>
        /// <param name="PushBytesData">压入资料</param>
        /// <param name="Length">压入长度</param>
        /// <param name="Offset">压入偏移</param>
        public void PushBytes(byte[] PushBytesData, int Length = -1, int Offset = 0)
        {
            if (PushBytesData == null
                || Offset >= PushBytesData.Length)
            {
                return;
            }

            int pushSize = 0;
            if (Length >= 0)
            {
                pushSize = PushBytesData.Length - Offset >= Length ? Length : PushBytesData.Length - Offset;
            }
            else
            {
                pushSize = PushBytesData.Length - Offset;
            }


            CheckAndFixSize(pushSize);

            Array.Copy(PushBytesData, Offset, _Data, _CurWritaIndex, pushSize);

            _CurWritaIndex += pushSize;
            _Size += pushSize;
        }

        /// <summary>
        /// 压入一个IP终结点
        /// </summary>
        /// <param name="Data"></param>
        public void PushData(System.Net.IPEndPoint Data)
        {
            if (Data == null)
            {
                return;
            }
            Int64 ip = System.BitConverter.ToInt64(Data.Address.GetAddressBytes(), 0);
            int port = Data.Port;

            PushData(ip);
            PushData(port);
        }

        /// <summary>
        /// 弹出一个IP终结点
        /// </summary>
        /// <returns></returns>
        public System.Net.IPEndPoint PopIP()
        {
            System.Net.IPEndPoint Rev = null;
            Int64 ip = PopInt64();
            int port = PopInt();
            Rev = new System.Net.IPEndPoint(ip, port);
            return Rev;
        }

        /// <summary>
        /// 弹出一个字符串,根据写入时候的长度
        /// </summary>
        /// <returns></returns>
        public string PopString()
        {
            int len = PopInt();

            return Encoding.UTF8.GetString(PopLimitLenData(len));
        }

        /// <summary>
        /// 压入一个bool类型的资料
        /// </summary>
        /// <param name="PushStrData">压入bool</param>
        public void PushData(bool BoolData)
        {
            PushByte(BoolData ? (byte)1 : (byte)0);
        }


        /// <summary>
        /// 弹出一个bool
        /// </summary>
        public bool PopBool()
        {
            return PopByte() == 1;
        }


        /// <summary>
        /// 弹出一个byte数据
        /// </summary>
        /// <returns>The byte.</returns>
        public byte PopByte()
        {
            byte Rev = _Data[_CurReadIndex];
            if (_Size <= 0)
            {
                throw new IndexOutOfRangeException("数据资料长度不足,byte");
            }
            _CurReadIndex++;
            _Size--;

            return Rev;
        }

        /// <summary>
        /// 弹出一个ushort数据
        /// </summary>
        /// <returns>The word.</returns>
        public ushort PopWord()
        {
            ushort Rev = BitConverter.ToUInt16(_Data, _CurReadIndex);
            if (_Size < sizeof(ushort))
            {
                throw new IndexOutOfRangeException("数据资料长度不足,Word");
            }
            _CurReadIndex += sizeof(ushort);
            _Size -= sizeof(ushort);
            return Rev;
        }

        /// <summary>
        /// 弹出一个int数据
        /// </summary>
        /// <returns>The int.</returns>
        public int PopInt()
        {
            int Rev = BitConverter.ToInt32(_Data, _CurReadIndex);
            if (_Size < sizeof(int))
            {
                throw new IndexOutOfRangeException("数据资料长度不足,int");
            }
            _CurReadIndex += sizeof(int);
            _Size -= sizeof(int);
            return Rev;
        }

        /// <summary>
        /// 弹出一个uint数据
        /// </summary>
        /// <returns>The U int.</returns>
        public uint PopUInt()
        {
            uint Rev = BitConverter.ToUInt32(_Data, _CurReadIndex);
            if (_Size < sizeof(uint))
            {
                throw new IndexOutOfRangeException("数据资料长度不足,uint");
            }
            _CurReadIndex += sizeof(uint);
            _Size -= sizeof(uint);
            return Rev;
        }

        /// <summary>
        /// 弹出一个int64数据
        /// </summary>
        /// <returns>The int64.</returns>
        public Int64 PopInt64()
        {
            Int64 Rev = BitConverter.ToInt64(_Data, _CurReadIndex);
            if (_Size < sizeof(Int64))
            {
                throw new IndexOutOfRangeException("数据资料长度不足,Int64");
            }
            _CurReadIndex += sizeof(Int64);
            _Size -= sizeof(Int64);
            return Rev;
        }

        /// <summary>
        /// 弹出一个uint64数据
        /// </summary>
        /// <returns>The uint64.</returns>
        public UInt64 PopUInt64()
        {
            UInt64 Rev = BitConverter.ToUInt64(_Data, _CurReadIndex);
            if (_Size < sizeof(UInt64))
            {
                throw new IndexOutOfRangeException("数据资料长度不足,UInt64");
            }
            _CurReadIndex += sizeof(UInt64);
            _Size -= sizeof(UInt64);
            return Rev;
        }

        /// <summary>
        /// 弹出一个float数据
        /// </summary>
        /// <returns>The float.</returns>
        public float PopFloat()
        {
            float Rev = BitConverter.ToSingle(_Data, _CurReadIndex);
            if (_Size < sizeof(float))
            {
                throw new IndexOutOfRangeException("数据资料长度不足,float");
            }
            _CurReadIndex += sizeof(float);
            _Size -= sizeof(float);
            return Rev;
        }

        /// <summary>
        /// 压入一个float数组
        /// </summary>
        /// <param name="FloatArrray"></param>
        public void PushFloatArray(float[] FloatArrray)
        {
            int Len = Convert.ToInt32(FloatArrray.Length);
            PushData(Len);
            for (int i = 0; i < Len; i++)
            {
                PushData(BitConverter.GetBytes(FloatArrray[i]));
            }
        }

        /// <summary>
        /// 弹出一个float数组
        /// </summary>
        /// <returns></returns>
        public float[] PopFloatArray()
        {
            int Len = PopInt();
            float[] Rev = new float[Len];
            for (int i = 0; i < Len; i++)
            {
                Rev[i] = PopFloat();
            }

            return Rev;
        }

        /// <summary>
        /// 弹出一个V2
        /// </summary>
        /// <returns></returns>
        public Vector2 PopV2()
        {
            Vector2 Rev = new Vector2();
            Rev.x = PopFloat();
            Rev.y = PopFloat();
            return Rev;
        }

        /// <summary>
        /// 压入一个V2
        /// </summary>
        /// <returns></returns>
        public void PushData(Vector2 Data)
        {
            PushData(Data.x);
            PushData(Data.y);
        }

        /// <summary>
        /// 弹出一个V3
        /// </summary>
        /// <returns></returns>
        public Vector3 PopV3()
        {
            Vector3 Rev = new Vector3();
            Rev.x = PopFloat();
            Rev.y = PopFloat();
            Rev.z = PopFloat();
            return Rev;
        }

        /// <summary>
        /// 压入一个V3
        /// </summary>
        /// <returns></returns>
        public void PushData(Vector3 Data)
        {
            PushData(Data.x);
            PushData(Data.y);
            PushData(Data.z);
        }

        /// <summary>
        /// 弹出一个V4
        /// </summary>
        /// <returns></returns>
        public Vector4 PopV4()
        {
            Vector4 Rev = new Vector4();
            Rev.x = PopFloat();
            Rev.y = PopFloat();
            Rev.z = PopFloat();
            Rev.w = PopFloat();
            return Rev;
        }

        /// <summary>
        /// 弹出一个Color
        /// </summary>
        /// <returns></returns>
        public Color PopColor()
        {
            Color Rev = new Color();
            Rev.r = PopFloat();
            Rev.g = PopFloat();
            Rev.b = PopFloat();
            Rev.a = PopFloat();
            return Rev;
        }

        /// <summary>
        /// 压入一个V4
        /// </summary>
        /// <returns></returns>
        public void PushData(Vector4 Data)
        {
            PushData(Data.x);
            PushData(Data.y);
            PushData(Data.z);
            PushData(Data.w);
        }

        ///// <summary>
        ///// 消息专用包头压入(因为直接用string,可能造成超长内存申请,固定长度)
        ///// </summary>
        ///// <param name="Head"></param>
        //public void PushHead(string Head)
        //{
        //    byte[] StrBytes = UTF8Encoding.UTF8.GetBytes(Head);
        //    int Len = StrBytes.Length;

        //    PushBytes(StrBytes, 3);
        //}

        ///// <summary>
        ///// 消息专用包头弹出(因为直接用string,可能造成超长内存申请,固定长度)
        ///// </summary>
        //public string PopHead()
        //{
        //    return UTF8Encoding.UTF8.GetString(PopLimitLenData(3));
        //}

        /// <summary>
        /// 输出消息的处理
        /// </summary>
        /// <param name="Msg"></param>
        void ShowMsg(string Msg)
        {
            UnityEngine.Debug.Log(Msg);
        }

        /// <summary>
        /// 将现有资料做一下异或
        /// </summary>
        /// <param name="XorCode">异或码</param>
        public void Oxr(byte XorCode)
        {
            for (int i = _CurReadIndex; i < _CurWritaIndex; i++)
            {
                _Data[i] ^= XorCode;
            }
        }
    }

    /// <summary>
    /// 派生一个用于网络传输的消息类型
    /// </summary>
    public class CNetData : CIOData
    {
        public CNetData(int Size = 1024 * 2)
            : base(Size)
        {

        }


        /// <summary>
        /// 当前可用的大小
        /// </summary>
        public int freeSize
        {
            get
            {
                return _Data.Length - _CurWritaIndex;
            }
        }

        /// <summary>
        /// 写入流消息,把当前的资料全部写入一个流(注意,写入之后不从资料中清除)
        /// </summary>
        /// <param name="WriteStream"></param>
        public void WriteDataToStream(System.Net.Sockets.Socket Socket)
        {
            if (Socket.Connected)
            {
                Socket.Send(_Data, _CurReadIndex, _CurWritaIndex, System.Net.Sockets.SocketFlags.None);
            }
        }

        /// <summary>
        /// 开始异步发送消息
        /// </summary>
        /// <param name="Socket"></param>
        /// <param name="CallBack"></param>
        /// <returns></returns>
        public IAsyncResult BeginSendData(System.Net.Sockets.Socket Socket, System.AsyncCallback CallBack)
        {
            byte[] SendData = PopAllData();
            return Socket.BeginSend(SendData, 0, SendData.Length, System.Net.Sockets.SocketFlags.None, CallBack, null);
        }
    }



}
