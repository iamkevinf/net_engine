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
    public class FrameInfo
    {
        public System.UInt64 CurFrameID = 0;
        public System.UInt64 NxtFrameID = 0;
        public Dictionary<System.UInt32, List<EPlayerEvent>> UUID2FrameInfoPool = new Dictionary<System.UInt32, List<EPlayerEvent>>();
    }

} // end of namespace TVR
