//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from: proto/player.proto
namespace proto.player
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"PlayerInfo")]
  public partial class PlayerInfo : global::ProtoBuf.IExtensible
  {
    public PlayerInfo() {}
    

    private uint _UUID = default(uint);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"UUID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(uint))]
    public uint UUID
    {
      get { return _UUID; }
      set { _UUID = value; }
    }

    private double _PosX = default(double);
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"PosX", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(double))]
    public double PosX
    {
      get { return _PosX; }
      set { _PosX = value; }
    }

    private double _PosY = default(double);
    [global::ProtoBuf.ProtoMember(3, IsRequired = false, Name=@"PosY", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(double))]
    public double PosY
    {
      get { return _PosY; }
      set { _PosY = value; }
    }

    private double _PosZ = default(double);
    [global::ProtoBuf.ProtoMember(4, IsRequired = false, Name=@"PosZ", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(double))]
    public double PosZ
    {
      get { return _PosZ; }
      set { _PosZ = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"CSLogin")]
  public partial class CSLogin : global::ProtoBuf.IExtensible
  {
    public CSLogin() {}
    
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"SCLogin")]
  public partial class SCLogin : global::ProtoBuf.IExtensible
  {
    public SCLogin() {}
    

    private uint _UUID = default(uint);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"UUID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(uint))]
    public uint UUID
    {
      get { return _UUID; }
      set { _UUID = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"CSReady")]
  public partial class CSReady : global::ProtoBuf.IExtensible
  {
    public CSReady() {}
    
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"SCReady")]
  public partial class SCReady : global::ProtoBuf.IExtensible
  {
    public SCReady() {}
    
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"SCFrameInit")]
  public partial class SCFrameInit : global::ProtoBuf.IExtensible
  {
    public SCFrameInit() {}
    

    private ulong _CurFrameID = default(ulong);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"CurFrameID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong CurFrameID
    {
      get { return _CurFrameID; }
      set { _CurFrameID = value; }
    }

    private ulong _NxtFrameID = default(ulong);
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"NxtFrameID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong NxtFrameID
    {
      get { return _NxtFrameID; }
      set { _NxtFrameID = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"UserFrame")]
  public partial class UserFrame : global::ProtoBuf.IExtensible
  {
    public UserFrame() {}
    

    private uint _UUID = default(uint);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"UUID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(uint))]
    public uint UUID
    {
      get { return _UUID; }
      set { _UUID = value; }
    }
    private readonly global::System.Collections.Generic.List<uint> _KeyInfo = new global::System.Collections.Generic.List<uint>();
    [global::ProtoBuf.ProtoMember(2, Name=@"KeyInfo", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public global::System.Collections.Generic.List<uint> KeyInfo
    {
      get { return _KeyInfo; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"CSFrame")]
  public partial class CSFrame : global::ProtoBuf.IExtensible
  {
    public CSFrame() {}
    

    private uint _UUID = default(uint);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"UUID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(uint))]
    public uint UUID
    {
      get { return _UUID; }
      set { _UUID = value; }
    }

    private ulong _FrameID = default(ulong);
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"FrameID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong FrameID
    {
      get { return _FrameID; }
      set { _FrameID = value; }
    }
    private readonly global::System.Collections.Generic.List<uint> _KeyInfo = new global::System.Collections.Generic.List<uint>();
    [global::ProtoBuf.ProtoMember(3, Name=@"KeyInfo", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    public global::System.Collections.Generic.List<uint> KeyInfo
    {
      get { return _KeyInfo; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"SCFrame")]
  public partial class SCFrame : global::ProtoBuf.IExtensible
  {
    public SCFrame() {}
    

    private ulong _FrameID = default(ulong);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"FrameID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong FrameID
    {
      get { return _FrameID; }
      set { _FrameID = value; }
    }

    private ulong _NextFrameID = default(ulong);
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"NextFrameID", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(ulong))]
    public ulong NextFrameID
    {
      get { return _NextFrameID; }
      set { _NextFrameID = value; }
    }
    private readonly global::System.Collections.Generic.List<UserFrame> _Users = new global::System.Collections.Generic.List<UserFrame>();
    [global::ProtoBuf.ProtoMember(3, Name=@"Users", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<UserFrame> Users
    {
      get { return _Users; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"SCSight")]
  public partial class SCSight : global::ProtoBuf.IExtensible
  {
    public SCSight() {}
    
    private readonly global::System.Collections.Generic.List<PlayerInfo> _Players = new global::System.Collections.Generic.List<PlayerInfo>();
    [global::ProtoBuf.ProtoMember(1, Name=@"Players", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<PlayerInfo> Players
    {
      get { return _Players; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
}