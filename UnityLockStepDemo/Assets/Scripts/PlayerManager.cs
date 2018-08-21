////////////////////////////////////////////////////////////////////////
//
// 作者：Kevin
// 时间：
// 功能：
//
///////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace TVR
{
    public class PlayerManager : ISingleton<PlayerManager>
    {
        public Dictionary<System.UInt32, Player> m_allPlayer = new Dictionary<uint, Player>();
        public Player PlayerAvatar = null;

        protected override void InitInstance()
        {
        }

        protected override void ClearInstance()
        {
        }

        public void AddOne(Player player)
        {
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
