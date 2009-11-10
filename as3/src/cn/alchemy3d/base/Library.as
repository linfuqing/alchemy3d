package cn.alchemy3d.base
{
	import cmodule.alchemy3d.CLibInit;
	
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	
	public class Library extends Sprite
	{
		public static var memory:ByteArray;
		public static var alchemy3DLib:Object;
		
		private static var _instance:Library;
		
		public static function get floatTypeSize():uint
		{
			return 4;
		}
		
		public static function get intTypeSize():uint
		{
			return 4;
		}
		
		public static function getInstance():Library
		{
			if(_instance == null)
			{
				_instance = new Library();
			}
			
			return _instance;
		}
		
		/*public static function getMemory(types:Vector.<String>, position:uint = 0):Array
		{
			if(_instance == null) throw new Error("Library was not initialized");
			
			var dest:Array = new Array(types.length);
			var type:String, i:int = 0;
			
			if (position != 0) Library.memory.position = position;
			
			for (; i < dest.length; i ++)
			{
				type = types[i];
			
				switch (type)
				{
					case "int":
							dest[i] = Library.memory.readInt();
						break;
					case "float":
							dest[i] = Library.memory.readFloat();
						break;
					case "double":
							dest[i] = Library.memory.readDouble();
						break;
					case "uint":
							dest[i] = Library.memory.readUnsignedInt();
						break;
					case "byte":
							dest[i] = Library.memory.readByte();
						break;
					case "short":
							dest[i] = Library.memory.readShort();
						break;
					case "object":
							dest[i] = Library.memory.readObject();
						break;
					case "utf":
							dest[i] = Library.memory.readUTF();
						break;
					case "utfbytes":
//							dest[i] = Library.memory.readUTFBytes();
						break;
				}
			}
			
			return dest;
		}*/
		
		/*public static function setMemory(values:Array, types:Vector.<String>, position:uint = 0):void
		{
			if(_instance == null) throw new Error("Library was not initialized");
			
			if (values.length != types.length)
			{
				Alchemy3DLog.warning("值和类型的长度不相同");
				return;
			}
			
			var item:*, type:String, i:int = 0;
			
			if (position != 0) Library.memory.position = position;
			
			for (; i < values.length; i ++)
			{
				item = values[i];
				type = types[i];
			
				switch (type)
				{
					case "int":
							Library.memory.writeInt(item);
						break;
					case "float":
							Library.memory.writeFloat(item);
						break;
					case "double":
							Library.memory.writeDouble(item);
						break;
					case "uint":
							Library.memory.writeUnsignedInt(item);
						break;
					case "byte":
							Library.memory.writeByte(item);
						break;
					case "short":
							Library.memory.writeShort(item);
						break;
					case "object":
							Library.memory.writeObject(item);
						break;
					case "utf":
							Library.memory.writeUTF(item);
						break;
					case "utfbytes":
							Library.memory.writeUTFBytes(item);
						break;
				}
			}
		}*/
		
		public function Library()
		{
			super();
			
			var clib:CLibInit = new CLibInit();
			alchemy3DLib = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.alchemy3d");
			memory = (ns::gstate).ds;
		}
	}
}