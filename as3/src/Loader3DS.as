package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.objects.A3DS;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.events.Event;
	import flash.net.URLLoader;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class Loader3DS extends Basic
	{
		private var loader:URLLoader;
		
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		protected var a3ds:A3DS;
		
		public function Loader3DS()
		{
			super();
			
			init();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene, viewport);
			addChild(fps);
		}
		
		protected function init(e:Event = null):void
		{
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 5, 5000);
			addCamera(camera);
			camera.z = -300;
			
			viewport = new Viewport3D(600, 400, scene, camera);
			addViewport(viewport);
			
			//Start 3DS
			a3ds = new A3DS();
			a3ds.load("asset/man.3ds");
			scene.addEntity(a3ds);
			
			a3ds.rotationX = -90;
			a3ds.scale = 3;
			a3ds.y = -50;

			startRendering();
			
			showInfo();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			a3ds.rotationY ++;
			
			super.onRenderTick(e);
		}
	}
}