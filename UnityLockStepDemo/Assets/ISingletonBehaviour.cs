using UnityEngine;
using System.Collections;

namespace TVR
{
    /// <summary>
    /// Be Careful for overusing Pattern of Singleton
    /// As a note, this is made as MonoBehaviour because we need Coroutines.
    /// if You need a fatten Singleton use Pure ISingleton
    /// </summary>
    public abstract class ISingletonBehaviour<T> : MonoBehaviour where T : MonoBehaviour
    {
        protected static T m_instance;

        private static object m_lock = new object();

        public static T Instance
        {
            get
            {
                //Singleton have been Destory!
                if (m_applicationIsQutting)
                {
                    Debug.LogWarning("[Singleton] Instance '" + typeof(T) +
                    "' already destroyed on application quit." +
                    " Won't create again - returning null.");
                    return null;
                }

                //Referring Lock
                lock (m_lock)
                {
                    if (m_instance == null)
                    {
                        m_instance = (T)FindObjectOfType(typeof(T));

                        if (FindObjectsOfType(typeof(T)).Length > 1)
                        {
                            Debug.LogError("[Singleton] Something went really wrong " +
                                " - there should never be more than 1 singleton!" +
                                " Reopening the scene might fix it.");
                            return m_instance;
                        }


                        if (m_instance == null)
                        {
                            GameObject singleton = new GameObject();
                            m_instance = singleton.AddComponent<T>();
                            singleton.name = "[singleton]" + typeof(T).ToString();

                            (m_instance as ISingletonBehaviour<T>).InitInstance();

                            DontDestroyOnLoad(singleton);

                            Debug.Log("[Singleton] An instance of " + typeof(T) +
                                " is needed in the scene, so '" + singleton +
                                "' was created with DontDestroyOnLoad.");
                        }
                        else
                        {
                            //Debug.Log("[Singleton] Using instance already created: " +
                            //    m_instance.gameObject.name);
                        }
                    }

                    return m_instance;
                }
            }
        }

        private static bool m_applicationIsQutting = false;
        protected static bool m_singletonInited = false;

        /// <summary>
        /// When Unity quits, it destroys objects in a random order.
        /// In principle, a Singleton is only destroyed when application quits.
        /// If any script calls Instance after it have been destroyed, 
        ///   it will create a buggy ghost object that will stay on the Editor scene
        ///   even after stopping playing the Application. Really bad!
        /// So, this was made to be sure we're not creating that buggy ghost object.
        /// </summary> 
        protected virtual void OnApplicationQuit()
        {
            m_applicationIsQutting = true;
        }

        protected virtual void InitInstance() { m_singletonInited = true; }

        protected virtual void ClearInstance() { m_singletonInited = false; }

    }

    /// <summary>
    /// Easy Singleton
    /// </summary>
    public abstract class ISingleton<T>
    {
        protected static T m_instance;

        private static object m_lock = new object();

        public static T Instance
        {
            get
            {
                lock (m_lock)
                {
                    if (m_instance == null)
                    {
                        m_instance = (T)System.Activator.CreateInstance(typeof(T), true);
                        (m_instance as ISingleton<T>).InitInstance();
                    }
                    return m_instance;
                }
            }
        }

        protected abstract void InitInstance();

        protected abstract void ClearInstance();
    }
}
