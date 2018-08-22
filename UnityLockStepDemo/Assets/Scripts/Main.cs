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
    public class Main : MonoBehaviour
    {
        public Player playerPrefab = null;
        bool m_connected = false;

        void Awake()
        {
            MessageCenter.Instance.AddListener((int)EMessageType.ESCLogin, OnLogin);
            MessageCenter.Instance.AddListener((int)EMessageType.ESCSight, OnSight);
        }

        void Start()
        {
        }

        void OnConnected()
        {
            CSLogin proto = new CSLogin();

            byte[] bytes = ProtoSerialize.Serialize<CSLogin>(proto);
            int size = bytes.Length;

            MessageBody body = new MessageBody();
            body.size = (uint)(size + MessageBody.HEADER_TYPE_BYTES);
            body.type = (ushort)EMessageType.ECSLogin;
            body.data = bytes;

            TCPClient.Instance.Send(body);
        }

        void Update()
        {
            if (!m_connected && TCPClient.Instance.Connected)
            {
                m_connected = true;
                OnConnected();
            }

            TCPClient.Instance.Update();
        }

        private void OnDestroy()
        {
            TCPClient.Instance.Fini();
            MessageCenter.Instance.Fini();
        }

        void OnLogin(params object[] args)
        {
            MessageBody body = args[0] as MessageBody;
            if (body == null)
                return;

            SCLogin ret = ProtoSerialize.DeSerialize<SCLogin>(body.data);
            if (ret == null)
                return;

            Player instance = Instantiate<Player>(playerPrefab);
            instance.OnLogin(ret);
        }

        void OnSight(params object[] args)
        {
            MessageBody body = args[0] as MessageBody;
            if (body == null)
                return;

            SCSight ret = ProtoSerialize.DeSerialize<SCSight>(body.data);
            if (ret == null)
                return;

            for (int i = 0; i < ret.Players.Count; ++i)
            {
                uint UUID = ret.Players[i].UUID;
                Player instance = null;
                if(!PlayerManager.Instance.GetPlayer(UUID, ref instance))
                    instance = Instantiate<Player>(playerPrefab);
                instance.OnSight(ret);
            }
        }
    }

} // end of namespace TVR
