using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;

//*************************************************************************
//文件名(File Name):		 TDicLoop
//作者(Author):			    杨子润
//日期(Create Date):		 2015.11.4
//功能(Function):			 池模板类，方便的使用对象池技术
//					使用对象池技术，最大大小的对象数组用于存放对象，当使用的
//					时候从闲置对象中获得一个，使用完之后返回池中，类似内存池
//					技术，但是基本元素是对象而已。
//修改记录(RevisionHistory): 
//*************************************************************************

namespace TVR_Server.Basic
{
    public delegate void PoolObjBackFunc<T>(T Obj) where T : class;

    public class TDicPool<U, T> where T : class,new()
    {
        //默认栈大小
        int m_DefaultSize = 10;
        //池对象字典
        //使用字典而不使用列表是因为列表是快速查找,字典是哈希查找,速度更快更稳定
        //使用栈的原因是频繁使用的对象会被频繁调用，内存命中率更高
        Dictionary<U, Stack<T>> m_DloopDic;

        object m_Lock = new object();

        /// <summary>
        /// 创建默认的结构
        /// </summary>
        /// <param name="Size"></param>
        public TDicPool(int Size = 10)
        {
            m_DefaultSize = Size;
            m_DloopDic = new Dictionary<U, Stack<T>>();
        }

        public void InitCreateObj(U ObjName)
        {
            lock (m_Lock)
            {
                Stack<T> stack = null;
                if (!m_DloopDic.TryGetValue(ObjName, out stack))
                {
                    stack = new Stack<T>(m_DefaultSize);
                    m_DloopDic.Add(ObjName, stack);
                }

                for (int i = 0; i < m_DefaultSize; i++)
                {
                    stack.Push(new T());
                }
            }
        }

        /// <summary>
        /// 获得池对象,池里面没有,就构造一个默认参数对象返回
        /// </summary>
        /// <param name="ObjName">获得对象的名字，或者类型名称</param>
        /// <returns>返回的对象</returns>
        public T GetObj(U ObjName)
        {
            lock (m_Lock)
            {
                Stack<T> stack = null;
                if (!m_DloopDic.TryGetValue(ObjName, out stack))
                {
                    return new T();
                }

                if (stack.Count < 1)
                {
                    return new T();
                }

                return stack.Pop();
            }
        }

        /// <summary>
        /// 归还池对象
        /// </summary>
        /// <param name="ObjName">对象的名称或者类型</param>
        /// <param name="Obj">对象</param>
        /// <param name="BackFunc">返回处理函数</param>
        public void BackObj(U ObjName, T Obj, PoolObjBackFunc<T> BackFunc = null)
        {
            lock (m_Lock)
            {
                Stack<T> stack = null;
                if (!m_DloopDic.TryGetValue(ObjName, out stack))
                {
                    stack = new Stack<T>(m_DefaultSize);
                    m_DloopDic.Add(ObjName, stack);
                }
                if (!stack.Contains(Obj))
                {
                    stack.Push(Obj);
                }
                else
                {
                    UnityEngine.Debug.Log("返回一个已经在池里的对象" + new StackTrace().GetFrames().ToString());
                }
                if (BackFunc != null)
                {
                    BackFunc(Obj);
                }
            }
        }

        /// <summary>
        /// 拥有对象类型的数量
        /// </summary>
        public int KeyCount
        {
            get
            {
                lock (m_Lock)
                {
                    return m_DloopDic.Count;
                }
            }
        }

        /// <summary>
        /// 清空池所有对象
        /// </summary>
        public void Clear()
        {
            lock (m_Lock)
            {
                m_DloopDic.Clear();
            }
        }

        /// <summary>
        /// 删除掉某个名称的所有对象
        /// </summary>
        /// <param name="ObjName">对象的名称</param>
        void RemoveKey(U ObjName)
        {
            lock (m_Lock)
            {
                if (!m_DloopDic.ContainsKey(ObjName))
                {
                    return;
                }

                m_DloopDic.Remove(ObjName);
            }
        }
    }

    /// <summary>
    /// 可以通过int作为hashID的快速查询池
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class IntHashPool<T> where T : class,new()
    {
        /// <summary>
        /// 池对象
        /// </summary>
        protected TDicPool<int, T> m_Pool;
        /// <summary>
        /// 
        /// </summary>
        protected Dictionary<int, T> m_CurUseList;

        protected Dictionary<T, int> m_T_ID_HashTable;

        protected int m_RunNumber = 1;
        protected int m_Size = 10;

        /// <summary>
        /// 获得池对象
        /// </summary>
        protected TDicPool<int, T> Pool
        {
            get
            {
                if (m_Pool == null)
                {
                    m_Pool = new TDicPool<int, T>(m_Size);
                }

                return m_Pool;
            }
        }

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="Size"></param>
        public IntHashPool(int Size)
        {
            m_Size = Size;
        }

        /// <summary>
        /// 根据ID获得对象
        /// </summary>
        /// <param name="ID"></param>
        /// <returns></returns>
        public T GetObjByID(int ID)
        {
            T Rev = null;
            if (!m_CurUseList.TryGetValue(ID, out Rev))
            {
                Rev = null;
            }

            return Rev;
        }

        /// <summary>
        /// 根据对象获得ID
        /// </summary>
        /// <param name="Obj"></param>
        /// <returns></returns>
        public int GetIDByObj(T Obj)
        {
            int Rev = -1;
            if (!m_T_ID_HashTable.TryGetValue(Obj, out Rev))
            {
                Rev = -1;
            }

            return Rev;
        }

        /// <summary>
        /// 获得一个对象
        /// </summary>
        /// <returns></returns>
        public T GetObj()
        {
            T TmpObj = Pool.GetObj(0);

            if (m_CurUseList == null)
            {
                m_CurUseList = new Dictionary<int, T>(m_Size);
                m_T_ID_HashTable = new Dictionary<T, int>(m_Size);
            }
            m_CurUseList.Add(m_RunNumber, TmpObj);
            m_T_ID_HashTable.Add(TmpObj, m_RunNumber);

            m_RunNumber++;
            if (m_RunNumber == -1)
            {
                m_RunNumber = 1;
            }
            return TmpObj;
        }

        /// <summary>
        /// 返还一个对象
        /// </summary>
        /// <param name="Obj"></param>
        public void BackObj(T Obj, PoolObjBackFunc<T> BackFunc = null)
        {
            Pool.BackObj(0, Obj, BackFunc);
            if ((m_CurUseList == null)
                || (m_T_ID_HashTable == null))
            {
                return;
            }

            int ID = -1;
            ID = GetIDByObj(Obj);
            if (ID >= 0)
            {
                m_CurUseList.Remove(ID);
                m_T_ID_HashTable.Remove(Obj);
            }

        }
    }
}