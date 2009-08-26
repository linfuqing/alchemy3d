package cn.alchemy3d.lib
{
	import cmodule.alchemy3d.CLibInit;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class Library
	{
		public var buffer:ByteArray;
		public var alchemy3DLib:Object;
		
		private static var _instance:Library;
		
		public static function getInstance():Library
		{
			if(_instance == null)
			{
				_instance = new Library();
			}
			
			return _instance;
		}
		
		public function Library()
		{
			var clib:CLibInit = new CLibInit();
			alchemy3DLib = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.alchemy3d");
			buffer = (ns::gstate).ds;
		}
	}
}