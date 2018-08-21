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
    public class Main : MonoBehaviour
    {
        void Start()
        {
        }

        void Update()
        {
            TCPClient.Instance.Update();
        }

        private void OnDestroy()
        {
            TCPClient.Instance.Fini();
            MessageCenter.Instance.Fini();
        }

    }

} // end of namespace TVR
