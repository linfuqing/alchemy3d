package cn.alchemy3d.base
{
	import cmodule.alchemy3d.CLibInit;
	
	import flash.utils.ByteArray;
	
	public class Library
	{
		private var _buffer:ByteArray;
		private var _methods:Object;
		
		private var sizeof_double:uint;
		private var sizeof_int:uint;
		
		private static var _instance:Library;
		
		public static function instance():Library
		{
			if(_instance == null)
			{
				_instance = new Library();
			}
			
			return _instance;
		}
		
		public function get doubleTypeSize():uint
		{
			return sizeof_double;
		}
		
		public function get intTypeSize():uint
		{
			return sizeof_int;
		}
		
		public function get buffer():ByteArray
		{
			return _buffer;
		}
		
		public function get methods():Object
		{
			return _methods;
		}
		
		public function Library()
		{
			var clib:CLibInit = new CLibInit();
			_methods = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.alchemy3d");
			_buffer = (ns::gstate).ds;
			
			init();
		}
		
		private function init():void
		{
			sizeof_double = _methods.doubleTypeSize();
			
			sizeof_int    = _methods.intTypeSize();
		}
	}
}