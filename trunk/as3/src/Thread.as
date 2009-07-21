package
{
	import cmodule.thread.CLibInit;
	
	import flash.display.Sprite;
	import flash.utils.ByteArray;

	public class Thread extends Sprite
	{
		public var buffer:ByteArray;
		public var alchemy3DLib:Object;
		
		public function Thread()
		{
			super();
			
			var clib:CLibInit = new CLibInit();
			alchemy3DLib = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.thread");
			buffer = (ns::gstate).ds;
			
			alchemy3DLib.thread();
		}
		
	}
}