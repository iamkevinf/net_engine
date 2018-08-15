using System;
using System.IO;
using UnityEngine;

public class ProtoSerialize
{
    public static byte[] Serialize<T>(T model)
    {
        try
        {
            //涉及格式转换，需要用到流，将二进制序列化到流中  
            using (MemoryStream ms = new MemoryStream())
            {
                //使用ProtoBuf工具的序列化方法  
                ProtoBuf.Serializer.Serialize<T>(ms, model);
                //定义二级制数组，保存序列化后的结果  
                byte[] result = new byte[ms.Length];
                //将流的位置设为0，起始点  
                ms.Position = 0;
                //将流中的内容读取到二进制数组中  
                ms.Read(result, 0, result.Length);
                return result;
            }
        }
        catch (Exception ex)
        {
            DEBUG.Error("序列化失败: " + ex.ToString());
            return null;
        }
    }

    public static T DeSerialize<T>(byte[] msg)
    {
        try
        {
            using (MemoryStream ms = new MemoryStream())
            {
                //将消息写入流中  
                ms.Write(msg, 0, msg.Length);
                //将流的位置归0  
                ms.Position = 0;
                //使用工具反序列化对象  
                T result = ProtoBuf.Serializer.Deserialize<T>(ms);
                return result;
            }
        }
        catch (Exception ex)
        {
            DEBUG.Error("反序列化失败: " + ex.ToString());
            return default(T);
        }
    }

#if false
    static void BakFile(string filename, string filename_bak)
    {
        using (FileStream file = new FileStream(filename, FileMode.OpenOrCreate))
        {
            //创建一个负责写入的流
            using (FileStream file_bak = new FileStream(filename_bak, FileMode.OpenOrCreate, FileAccess.Write))
            {
                byte[] buffer = new byte[file.Length];

                //因为文件可能比较大所以在读取的时候应该用循坏去读取
                while (true)
                {
                    //返回本次实际读取到的字节数
                    int r = file.Read(buffer, 0, buffer.Length);

                    if (r == 0)
                    {
                        break;
                    }

                    file_bak.Write(buffer, 0, r);//写入
                    file_bak.Close();
                }
            }

            file.Close();
        }
    }

    static bool CheckFile<T>(string filename, T model) where T : global::ProtoBuf.IExtensible, new()
    {
        string filename_bak = string.Format("{0}.bak", filename);

        bool ret = false;
        try
        {
            DeserializeFromFile<T>(filename, ref model);
            ret = true;
        }
        catch
        {
            BakFile(filename_bak, filename);
            ret = false;
        }

        return ret;
    }

    public static void SerializerToFile<T>(string filename, T model) where T : global::ProtoBuf.IExtensible, new()
    {
        string filename_bak = string.Format("{0}.bak", filename);
        BakFile(filename, filename_bak);

        try
        {
            using (FileStream file = new FileStream(filename, FileMode.OpenOrCreate))
            {
                ProtoBuf.Serializer.Serialize<T>(file, model);
                file.Close();
            }

            CheckFile<T>(filename, model);
        }
        catch
        {
            BakFile(filename_bak, filename);
        }
    }

    public static void DeserializeFromFile<T>(string filename, ref T output) where T : global::ProtoBuf.IExtensible, new()
    {
        try
        {
            using (FileStream file = new FileStream(filename, FileMode.OpenOrCreate))
            {
                output = ProtoBuf.Serializer.Deserialize<T>(file);
                file.Close();
            }
        }
        catch
        {
            string filename_bak = string.Format("{0}.bak", filename);
            using (FileStream file = new FileStream(filename_bak, FileMode.OpenOrCreate))
            {
                output = ProtoBuf.Serializer.Deserialize<T>(file);
                file.Close();
            }
        }
    }
#else

    public static void SerializerToFile<T>(string filename, T model) where T : global::ProtoBuf.IExtensible, new()
    {
        if(File.Exists(filename))
        {
            using (FileStream file = new FileStream(filename, FileMode.Truncate))
            {
                ProtoBuf.Serializer.Serialize<T>(file, model);
                file.Close();
            }
        }
        else
        {
            using (FileStream file = new FileStream(filename, FileMode.Create))
            {
                ProtoBuf.Serializer.Serialize<T>(file, model);
                file.Close();
            }
        }
    }

    public static bool DeserializeFromFile<T>(string filename, ref T output) where T : global::ProtoBuf.IExtensible, new()
    {
        try
        {
            using (FileStream file = new FileStream(filename, FileMode.OpenOrCreate))
            {
                output = ProtoBuf.Serializer.Deserialize<T>(file);
                file.Close();
            }

            return true;
        }
        catch
        {
            return false;
        }
    }
#endif
}
