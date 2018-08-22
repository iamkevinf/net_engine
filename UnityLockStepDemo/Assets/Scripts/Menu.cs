////////////////////////////////////////////////////////////////////////
//
// 作者：Kevin
// 时间：
// 功能：
//
///////////////////////////////////////////////////////////////////////

using proto.message_type;
using proto.player;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace TVR
{
    public class Menu : MonoBehaviour
    {
        void Start()
        {
        }

        void Update()
        {
        }

        public void OnClickReady()
        {
            CSReady proto = new CSReady();

            byte[] bytes = ProtoSerialize.Serialize<CSReady>(proto);
            int size = bytes.Length;

            MessageBody body = new MessageBody();
            body.size = (uint)(size + MessageBody.HEADER_TYPE_BYTES);
            body.type = (ushort)EMessageType.ECSReady;
            body.data = bytes;

            TCPClient.Instance.Send(body);
        }
    }

} // end of namespace TVR
