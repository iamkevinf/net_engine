﻿////////////////////////////////////////////////////////////////////////
//
// 作者：Kevin
// 时间：
// 功能：
//
///////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using TVR_Server.Basic;
using UnityEngine;

namespace TVR
{
    public class MessageBody
    {
        public static readonly System.Int32 HEADER_TYPE_BYTES = sizeof(System.UInt16);
        public static readonly System.Int32 HEADER_LEN_BYTES = sizeof(System.UInt32);
        public static readonly System.Int32 HEADER_SIZE = HEADER_TYPE_BYTES + HEADER_LEN_BYTES;

        public System.UInt32 size;
        public System.UInt16 type;
        public byte[] data;

        public MessageBody()
        {
            size = 0;
            type = 0;
        }

        public int GetSize()
        {
            return (int)size + HEADER_LEN_BYTES;
        }

        public byte[] Serialize()
        {
            CIOData iodata = CIOData.GetCIOData();
            iodata.PushData<System.UInt32>(size);
            iodata.PushData<System.UInt16>(type);

            iodata.PushBytes(data, (int)size - HEADER_TYPE_BYTES);

            byte[] ret = new byte[iodata.Size];
            iodata.GetData(ref ret);
            CIOData.BackCIOData(iodata);
            return ret;
        }

        public void UnSerialize(byte[] buffer)
        {
            CIOData iodata = CIOData.GetCIOData();
            iodata.SetData(buffer);
            size = iodata.PopUInt();
            type = iodata.PopWord();
            data = iodata.PopAllData();
            CIOData.BackCIOData(iodata);
        }
    }

} // end of namespace TVR