using Mina.Core.Buffer;
using System;

namespace TVR
{
    public class CPacket
    {
        public static int HEADER_TOTAL_BYTES = HEADER_LENGTH_BYTES + HEADER_TYPE_BYTES;
        public static int HEADER_LENGTH_BYTES = sizeof(int);
        public static int HEADER_TYPE_BYTES = sizeof(short);
        public static int MIN_MESSAGE_LENGTH = sizeof(short);
        public static int MAX_MESSAGE_LENGTH = 65535;
        /**
         * 从buf中取出数据包的长度,buf的position属性保持不变,在调用此方法假定buf当前的位置就是长度字段的起始位置
         * 
         * @param buf
         * @return -1,buf中没有足够的数据;>= 0
         * @throws IllegalStateException
         *             如果从buf中取得到包长度<MIN_MESSAGE_LENGTH或者大于MAX_MESSAGE_LENGTH会抛出此异常
         */
        public static int PeekPacketLength(IoBuffer buf)
        {
            return SeekLength(buf, true);
        }

        /**
         * 从buf中取出数据包的长度,buf的position属性向前移动HEADER_LEN_BYTES个字节,
         * 在调用此方法假定buf当前的位置就是长度字段的起始位置
         * 
         * @param buf
         * @return -1,buf中没有足够的数据;>= 0
         * @throws IllegalStateException
         *             如果从buf中取得到包长度<MIN_MESSAGE_LENGTH或者大于MAX_MESSAGE_LENGTH会抛出此异常
         */
        public static int PopPacketLength(IoBuffer buf)
        {
            return SeekLength(buf, false);
        }

        /**
         * 
         * 从buf中取出数据包的类型,buf的position属性保持不变
         * 
         * @param buf
         * @return -1,buf中没有足够的数据;>= 0
         * @throws IllegalStateException
         *             如果数据包中的类型小于0会抛出此异常
         */
        public static short PeekPacketType(IoBuffer buf)
        {
            int _pos = buf.Position;
            try
            {
                PopPacketLength(buf);
                return PopPacketType(buf);
            }
            finally
            {
                buf.Position = _pos;
            }
        }

        /**
         * 
         * 从buf中取出数据包的类型,buf的position属性向前移动HEADER_TYPE_BYTES个字节,
         * 在调用此方法假定buf当前的位置就是类型字段的起始位置
         * 
         * @return -1,buf中没有足够的数据;>= 0
         * @throws IllegalStateException
         *             如果数据包中的类型小于0会抛出此异常
         */
        public static short PopPacketType(IoBuffer buf)
        {
            return SeekType(buf, false);
        }

        private static int SeekLength(IoBuffer buf, bool peek)
        {
            int _len = SeekIntFromBuffer(buf, peek);
            if (_len >= 0
                    && (_len < MIN_MESSAGE_LENGTH || _len > MAX_MESSAGE_LENGTH))
            {
                throw new Exception("包长度错误");
            }
            return _len;
        }

        private static short SeekType(IoBuffer buf, bool peek)
        {
            short _type = SeekShort(buf, peek);
            return _type;
        }

        private static short SeekShort(IoBuffer buf, bool peek)
        {
            if (buf.Remaining >= HEADER_TOTAL_BYTES)
            {
                int _op = buf.Position;
                short _value = buf.GetInt16();
                if (peek)
                {
                    buf.Position = _op;
                }
                if (_value < 0)
                {
                    throw new Exception("IO: 获取到非法数据");
                }
                return _value;
            }
            else
            {
                return -1;
            }
        }

        /**
         * 从buf中获得一个16位的无符号的整数
         * 
         * @param buf
         * @param peek
         * @return
         */
        protected static int SeekIntFromBuffer(IoBuffer buf, bool peek)
        {
            if (buf.Remaining >= HEADER_TOTAL_BYTES)
            {
                int _op = buf.Position;
                int _value = buf.GetInt32();
                if (peek)
                {
                    buf.Position = _op;
                }
                if (_value > MAX_MESSAGE_LENGTH)
                {
                    throw new Exception("接收消息标记的长度超过最大限定长度");
                }
                return _value;
            }
            else
            {
                return -1;
            }
        }
    }
}
