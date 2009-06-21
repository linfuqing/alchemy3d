package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.View3D;
	
	import flash.display.Sprite;
	import flash.events.Event;

	[SWF(backgroundColor="#000000", width="800", height="600", frameRate="120")]
	public class Alchemy3D extends Sprite
	{
		private var viewport:View3D;
		private var scene:Scene3D;
		private var camera:Camera3D;
		
		protected var lib:Alchemy3DLib;
		
		public function Alchemy3D()
		{
			super();
			
			scene = new Scene3D();
			camera = new Camera3D();
			viewport = new View3D(800, 600, scene, camera);
			
			lib = Alchemy3DLib.getInstance();
			
			var p:Plane = new Plane(1, 500, 500, 1, 1);
			scene.addChild(p);
			
			addEventListener(Event.ENTER_FRAME, onRenderTick);
		}
		
		protected function onRenderTick(e:Event):void
		{
			viewport.render();
		}
	}
}