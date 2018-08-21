////////////////////////////////////////////////////////////////////////
//
// 作者：Kevin
// 时间：
// 功能：
//
///////////////////////////////////////////////////////////////////////

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
            PlayerManager.Instance.PlayerAvatar.PlayerReady();
        }
    }

} // end of namespace TVR
