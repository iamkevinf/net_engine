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
    public class Login : MonoBehaviour
    {
        [SerializeField]
        UnityEngine.UI.InputField edt_username = null;
        [SerializeField]
        UnityEngine.UI.InputField edt_password = null;

        void Start()
        {
            MessageCenter.Instance.AddListener((int)EMessageType.ESCLogin, OnLogin);
        }

        void Update()
        {
        }

        public void OnClickLogin()
        {
            if (!edt_username || !edt_password)
                return;

            CSLogin proto = new CSLogin();
            proto.UserName = edt_username.text;
            proto.PassWord = edt_password.text;

            byte[] bytes = ProtoSerialize.Serialize<CSLogin>(proto);
            int size = bytes.Length;

            MessageBody body = new MessageBody();
            body.size = (uint)(size + MessageBody.HEADER_TYPE_BYTES);
            body.type = (ushort)EMessageType.ECSLogin;
            body.data = bytes;

            TCPClient.Instance.OnConnect();
            TCPClient.Instance.Send(body);
        }

        void OnLogin(params object[] args)
        {
            MessageBody body = args[0] as MessageBody;
            if (body == null)
                return;

            SCLogin ret = ProtoSerialize.DeSerialize<SCLogin>(body.data);
            if (ret != null)
            {
                Debug.Log("登陆状态 " + ret.Ret);
            }
        }
    }

} // end of namespace TVR
