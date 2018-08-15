
namespace TVR
{
    using System.Collections.Generic;
    using System;

    public delegate void Messager(params object[] args);

    public enum MessageEvent
    {
        Non = -1,
        /////////////////////
        
        //AMainLanucher 生命周期事件
        MAINAPP,
        
        //IGameState 生命周期事件
        GAMESTATE

       /////////////////////
    }

    public class MessageCenter : ISingleton<MessageCenter>
    {
        protected MessageCenter() { }

        private Dictionary<int, Delegate> m_Listeners = null; 

        protected override void InitInstance()
        {
            m_Listeners = new Dictionary<int, Delegate>();
        }

        protected override void ClearInstance()
        {
            m_Listeners.Clear();
            m_Listeners = null;

            m_instance = null;
        }

        /// <summary>
        /// 增加监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        /// <param name="handler">消息处理函数</param>
        public void AddListener(MessageEvent eventType, Messager handler)
        {
            AddListener((int)eventType, handler);
        }

        /// <summary>
        /// 增加监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        /// <param name="handler">消息处理函数</param>
        public void AddListener(int eventType, Messager handler)
        {
            lock (m_Listeners)
            {
                if (!m_Listeners.ContainsKey(eventType))
                {
                    m_Listeners.Add(eventType, null);
                }
                m_Listeners[eventType] = (Messager)m_Listeners[eventType] + handler;
            }
        }

        /// <summary>
        /// 移除监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        /// <param name="handler">消息处理函数</param>
        public void RemoveListener(MessageEvent eventType, Messager handler)
        {
            RemoveListener((int)eventType, handler);
        }

        /// <summary>
        /// 移除监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        /// <param name="handler">消息处理函数</param>
        public void RemoveListener(int eventType, Messager handler)
        {
            lock (m_Listeners)
            {
                if (m_Listeners.ContainsKey(eventType))
                {
                    m_Listeners[eventType] = (Messager)m_Listeners[eventType] - handler;

                    if (m_Listeners[eventType] == null)
                    {
                        m_Listeners.Remove(eventType);
                    }
                }
            }
        }

        /// <summary>
        /// 清空监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        public void ClearListeners(MessageEvent eventType)
        {
            ClearListeners((int)eventType);
        }

        /// <summary>
        /// 清空监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        public void ClearListeners(int eventType)
        {
            lock (m_Listeners)
            {
                if (m_Listeners.ContainsKey(eventType))
                {
                    m_Listeners[eventType] = null;
                    m_Listeners.Remove(eventType);
                }
            }
        }

        /// <summary>
        /// 触发监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        /// <param name="args">消息信息参数</param>
        public void DoEvent(MessageEvent eventType, params object[] args)
        {
            DoEvent((int)eventType, args);
        }

        /// <summary>
        /// 触发监听器
        /// </summary>
        /// <param name="eventType">消息类型</param>
        /// <param name="args">消息信息参数</param>
        public void DoEvent(int eventType, params object[] args)
        {
            Delegate del;

            if (m_Listeners.TryGetValue(eventType, out del))
            {
                Messager msg = (Messager)del;

                if (msg != null)
                {
                    msg(args);
                }
            }
        }

        /// <summary>
        /// Need To Destory In Application End!
        /// </summary>
        public void Destory()
        {
            m_Listeners.Clear();
            m_Listeners = null;
        }

    }
}