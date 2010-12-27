package AlchemyLib.base
{
	import flash.events.EventDispatcher;

	public class Pointer extends EventDispatcher
	{
		use namespace Library;
		
		static protected var isCreated:Boolean = true;
		
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
			
			if(isCreated)
				initialize();
		}
		
		public function destroy(all:Boolean):void
		{
			
		}
		
		protected function initialize():void
		{
			
		}
		
	}
}