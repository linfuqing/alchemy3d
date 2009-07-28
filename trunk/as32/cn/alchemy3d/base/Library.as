package cn.alchemy3d.base
{
	import flash.utils.ByteArray;
	
	public class Library
	{
		private var _buffer:ByteArray;
		private var _library:Object;
		
		private static var _instance:Library;
		
		public static function getInstance():Library
		{
			if(_instance == null)
			{
				_instance = new Library();
			}
			
			return _instance;
		}
		
		public function get buffer():ByteArray
		{
			return _buffer;
		}
		
		public function get library():Object
		{
			return _library;
		}
		
		public function Library()
		{
			var clib:CLibInit = new CLibInit();
			_library = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.alchemy3d");
			_buffer = (ns::gstate).ds;
		}
	}
}