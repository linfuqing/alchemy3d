package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.View3D;
	
	import flash.display.Sprite;
	import flash.geom.Vector3D;
	import flash.utils.getTimer;

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
			
			lib = Alchemy3DLib.getInstance();
			
			scene = new Scene3D();
			camera = new Camera3D();
			viewport = new View3D(800, 600, scene, camera);
			
			var do3d:DisplayObject3D = new DisplayObject3D("test");
			scene.addChild(do3d);
			do3d.x = 200;
			do3d.y = -200;
			do3d.z = -500;
			do3d.direction = new Vector3D(45, 0, 0);
			trace(getTimer());
			
			lib.alchemy3DLib.test();
			trace(getTimer());
		}
		
	}
}