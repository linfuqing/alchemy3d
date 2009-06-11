package cn.alchemy3d.lib
{
	import cmodule.alchemy3d.CLibInit;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class Alchemy3DLib
	{
		public var buffer:ByteArray;
		public var alchemy3DLib:Object;
		
		private static var _instance:Alchemy3DLib;
		
		public static function getInstance():Alchemy3DLib
		{
			if(_instance == null)
			{
				_instance = new Alchemy3DLib();
			}
			
			return _instance;
		}
		
		public function Alchemy3DLib()
		{
			var clib:CLibInit = new CLibInit();
			alchemy3DLib = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.alchemy3d");
			buffer = (ns::gstate).ds;
		}
	}
}