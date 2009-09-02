package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.Viewport3D;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class Loader3DS extends Basic
	{
		private var loader:URLLoader;
		
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		public function Loader3DS()
		{
			super();
			
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, init);
			loader.load(new URLRequest("asset/man.3ds"));
		}
		
		protected function init(e:Event = null):void
		{
			loader.removeEventListener(Event.COMPLETE, init);
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			camera.z = -300;
			
			viewport = new Viewport3D(600, 400, scene, camera);
			addViewport(viewport);
			
			//Start 3DS
			var length:uint = loader.data.length;
			
			var pos:uint = alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = pos;
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
			alchemy3DLib.initialize3DS(scene.pointer, pos, length);
			
			startRendering();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			super.onRenderTick(e);
		}
	}
}