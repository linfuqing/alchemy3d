package
{
	import cmodule.loader3DS.CLibInit;
	
	import cn.alchemy3d.view.Basic;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class Loader3DS extends Basic
	{
		private var loader:URLLoader;
		private var loader3DS:Object;
		private var memory3ds:ByteArray;
		
		public function Loader3DS()
		{
			super();
			
			var clib:CLibInit = new CLibInit();
			loader3DS = clib.init();
			
			var ns:Namespace = new Namespace("cmodule.loader3DS");
			memory3ds = (ns::gstate).ds;
			
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, init);
			loader.load(new URLRequest("asset/3ds/EarthII.3ds"));
		}
		
		protected function init(e:Event = null):void
		{
			loader.removeEventListener(Event.COMPLETE, init);
			
			var pos:uint = loader3DS.applyForTmpBuffer(loader.data.length);
			
			var length:int = loader.data.length;
			
			memory3ds.position = pos;
			memory3ds.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
			memory3ds.position = pos;
			trace(memory3ds.readShort());
			
			loader3DS.load_from_bytes(pos, length);
		}
	}
}