////////////////////////////////////////////////////////////////////////
//
// 作者：Kevin
// 时间：
// 功能：
//
///////////////////////////////////////////////////////////////////////

using proto.player;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace TVR
{
    public class PlayerManager : ISingleton<PlayerManager>
    {
        public Dictionary<System.UInt32, Player> m_allPlayer = new Dictionary<uint, Player>();

        protected override void InitInstance()
        {
        }

        protected override void ClearInstance()
        {
        }

        public void AddOne(Player player)
        {
            if (m_allPlayer.ContainsKey(player.UUID))
                m_allPlayer[player.UUID] = player;
            else
                m_allPlayer.Add(player.UUID, player);
        }

        public bool GetPlayer(uint uid, ref Player player)
        {
            if (!m_allPlayer.ContainsKey(uid))
                return false;

            player = m_allPlayer[uid];

            return true;
        }
    }

} // end of namespace TVR
