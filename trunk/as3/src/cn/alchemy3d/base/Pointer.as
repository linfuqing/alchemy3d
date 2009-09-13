package cn.alchemy3d.base
{
	import flash.events.EventDispatcher;

	public class Pointer extends EventDispatcher
	{
		use namespace Library;
		
		protected const NULL :uint = 0;
		
		protected const TRUE :uint = 1;
		protected const FALSE:uint = 0;
		
		protected var _pointer:uint = NULL;
		
		public function get pointer():uint
		{
			return _pointer;
		}
		
		public function Pointer()
		{
			super(null);
			initialize();
		}
		
		protected function initialize():void
		{
			
		}
		
	}
}