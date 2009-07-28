package cn.alchemy3d.base
{
	public class Pointer
	{
		protected const NULL:uint = 0;
		
		internal var _pointer:uint;
		
		public function get pointer():uint
		{
			return _pointer;
		}
		
		public function Pointer()
		{
			_pointer = NULL;
		}

	}
}