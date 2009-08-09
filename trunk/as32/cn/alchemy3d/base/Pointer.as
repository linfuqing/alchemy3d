package cn.alchemy3d.base
{
	public class Pointer
	{
		protected const NULL  :uint = 0;
		
		protected const TRUE  :int = 1;
		protected const FALSE :int = 0;
		
		protected var _pointer:uint;
		
		public function get pointer():uint
		{
			return _pointer;
		}
		
		public function Pointer()
		{
			_pointer = NULL;
		}
		
		protected function initialize():void
		{
			
		}

	}
}