package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.Viewport3D;
	
	import flash.events.Event;
	import flash.geom.ColorTransform;
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
		
		protected var p:Plane;
		
		public function Loader3DS()
		{
			super();
			
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, init);
			loader.load(new URLRequest("asset/3ds/EarthII.3ds"));
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
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(0.1, 0.1, 0.1, 1);
			m.diffuse = new ColorTransform(.8, .8, .8, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 32;
			
			var parent:Entity = new Entity();
			scene.addEntity(parent);
			
			p = new Plane(m, null, 800, 800, 1, 1, "p");
			p.lightEnable = true;
			p.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			p.z = 800;
			scene.addEntity(p, parent);
			
			//Start 3DS
			var length:uint = loader.data.length;
			
			var pos:uint = alchemy3DLib.applyForTmpBuffer(length);
			
			Library.memory.position = pos;
			Library.memory.writeBytes(loader.data, 0, length);
			
			loader.data.clear();
			loader.data = null;
			
//			alchemy3DLib.initialize3DS(scene.pointer, pos, length);
			
			startRendering();
		}
	}
}