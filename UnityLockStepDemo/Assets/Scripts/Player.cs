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
    public enum EPlayerEvent
    {
        EPE_Key_Forward = 1,
        EPE_Key_Back = 2,
        EPE_Key_Left = 3,
        EPE_Key_Right = 4
    }

    public class Player : MonoBehaviour
    {
        public System.UInt32 UUID = 0;
        public double PosX = 0;
        public double PosY = 0;
        public double PosZ = 0;

        public System.UInt64 CurFrameID = 0;
        public System.UInt64 NxtFrameID = 0;

        public double speed = 0.1;

        public List<EPlayerEvent> CurFrameInfo = new List<EPlayerEvent>();

        void Start()
        {
            MessageCenter.Instance.AddListener((int)EMessageType.ESCReady, OnReady);
            MessageCenter.Instance.AddListener((int)EMessageType.ESCFrameInit, OnFrameInit);
            MessageCenter.Instance.AddListener((int)EMessageType.ESCFrame, OnFrameTurn);
        }

        void FixedUpdate()
        {
            KeyPressEvent();
        }

        public void OnLogin(SCLogin ret)
        {
            if (UUID != 0)
            {
                DEBUG.Error("Invalid HandeLogin UUID = {0}", UUID);
                return;
            }

            if (ret == null)
            {
                DEBUG.Error("Invalid Proto");
                return;
            }

            UUID = ret.UUID;

            PlayerManager.Instance.AddOne(this);

            DEBUG.Log("OnLogin:: UUID={0}", ret.UUID);
        }

        public void OnSight(SCSight ret)
        {
            if (ret == null)
            {
                DEBUG.Error("Invalid Proto");
                return;
            }

            DEBUG.Log("OnSight");

            for(int i = 0; i < ret.Players.Count; ++i)
            {
                PlayerInfo info = ret.Players[i];
                SetPlayerInfo(info);
                PlayerManager.Instance.AddOne(this);
            }
        }

        public void SetPlayerInfo(PlayerInfo info)
        {
            UUID = info.UUID;
            SetPosition(info.PosX, info.PosY, info.PosZ);
        }

        void SetPosition(double x, double y, double z)
        {
            PosX = x; PosY = y; PosZ = z;
            Vector3 pos = new Vector3((float)x, (float)y, (float)z);
            transform.position = pos;
        }

        void OnReady(params object[] args)
        {
            MessageBody body = args[0] as MessageBody;
            if (body == null)
                return;

            SCReady ret = ProtoSerialize.DeSerialize<SCReady>(body.data);
            if (ret == null)
            {
                DEBUG.Error("Invalid Proto");
                return;
            }

            DEBUG.Log("OnReady");
        }

        void OnFrameInit(params object[] args)
        {
            MessageBody body = args[0] as MessageBody;
            if (body == null)
                return;

            SCFrameInit proto = ProtoSerialize.DeSerialize<SCFrameInit>(body.data);
            if(proto == null)
            {
                DEBUG.Error("Invalid Proto");
                return;
            }

            CurFrameID = proto.CurFrameID;
            NxtFrameID = proto.NxtFrameID;
            DEBUG.Log("OnFrameInit:: CurFrameID={0} NxtFrameID={1}", CurFrameID, NxtFrameID);
        }

        void OnFrameTurn(params object[] args)
        {
            MessageBody body = args[0] as MessageBody;
            if (body == null)
                return;

            SCFrame proto = ProtoSerialize.DeSerialize<SCFrame>(body.data);
            if(proto == null)
            {
                DEBUG.Error("Invalid Proto");
                return;
            }

            FrameInfo frameInfo = new FrameInfo();
            frameInfo.CurFrameID = proto.FrameID;
            frameInfo.NxtFrameID = proto.NextFrameID;

            int size = proto.Users.Count;
            for(int i = 0; i < size; ++i)
            {
                UserFrame user = proto.Users[i];

                List<EPlayerEvent> frameInfoPool = new List<EPlayerEvent>();
                for(int j = 0; j < user.KeyInfo.Count; ++j)
                {
                    System.UInt32 key = user.KeyInfo[j];
                    frameInfoPool.Add((EPlayerEvent)key);
                }
                frameInfo.UUID2FrameInfoPool.Add(user.UUID, frameInfoPool);
            }

            FrameTurn(frameInfo);
            CurFrameID = frameInfo.NxtFrameID;
           // DEBUG.Log("OnFrameTurn:: CurFrameID={0}, Size={1}", CurFrameID, body.size);
        }

        public void PlayerReady()
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

        void KeyPressEvent()
        {
            if(Input.GetKey(KeyCode.W))
                PushKeyInfo(EPlayerEvent.EPE_Key_Forward);
            if(Input.GetKey(KeyCode.S))
                PushKeyInfo(EPlayerEvent.EPE_Key_Back);
            if (Input.GetKey(KeyCode.A))
                PushKeyInfo(EPlayerEvent.EPE_Key_Left);
            if (Input.GetKey(KeyCode.D))
                PushKeyInfo(EPlayerEvent.EPE_Key_Right);
        }

        void PushKeyInfo(EPlayerEvent e)
        {
            if (!TCPClient.Instance.Connected)
                return;

            CurFrameInfo.Add(e);
        }

        void SendKeyInfo()
        {
            CSFrame proto = new CSFrame();
            proto.UUID = UUID;
            proto.FrameID = CurFrameID;

            for(int i =0; i < CurFrameInfo.Count; ++i)
            {
                proto.KeyInfo.Add((System.UInt32)CurFrameInfo[i]);
            }
            CurFrameInfo.Clear();

            byte[] bytes = ProtoSerialize.Serialize<CSFrame>(proto);
            int size = bytes.Length;

            MessageBody body = new MessageBody();
            body.size = (uint)(size + MessageBody.HEADER_TYPE_BYTES);
            body.type = (ushort)EMessageType.ECSFrame;
            body.data = bytes;

            TCPClient.Instance.Send(body);
        }

        void MoveForward()
        {
            transform.position = transform.position + (float)speed * transform.forward;
            SetPosition(transform.position.x, transform.position.y, transform.position.z);
        }

        void MoveBack()
        {
            transform.position = transform.position - (float)speed * transform.forward;
            SetPosition(transform.position.x, transform.position.y, transform.position.z);
        }

        void MoveLeft()
        {
            transform.position = transform.position - (float)speed * transform.right;
            SetPosition(transform.position.x, transform.position.y, transform.position.z);
        }

        void MoveRight()
        {
            transform.position = transform.position + (float)speed * transform.right;
            SetPosition(transform.position.x, transform.position.y, transform.position.z);
        }

        void RunKeyInfo(Dictionary<System.UInt32, List<EPlayerEvent>> info)
        {
            foreach(var iter in info)
            {
                System.UInt32 uid = iter.Key;
                Player player = null;
                if (!PlayerManager.Instance.GetPlayer(uid, ref player))
                    continue;

                for (int i =0; i < iter.Value.Count; ++i)
                {
                    EPlayerEvent e = iter.Value[i];
                    switch(e)
                    {
                        case EPlayerEvent.EPE_Key_Forward:
                            player.MoveForward();
                            break;
                        case EPlayerEvent.EPE_Key_Back:
                            player.MoveBack();
                            break;
                        case EPlayerEvent.EPE_Key_Left:
                            player.MoveLeft();
                            break;
                        case EPlayerEvent.EPE_Key_Right:
                            player.MoveRight();
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        void FrameTurn(FrameInfo frameInfo)
        {
            //DEBUG.Log("CurFrameID = {0} frameInfo.CurFrameID = {1}, frameInfo.UUID2FrameInfoPool.Num = {2}", CurFrameID, frameInfo.CurFrameID, frameInfo.UUID2FrameInfoPool.Count);
            if(CurFrameID == frameInfo.CurFrameID)
            {
                // 上报
                SendKeyInfo();
                // 执行
                RunKeyInfo(frameInfo.UUID2FrameInfoPool);
            }
        }
    }

} // end of namespace TVR
