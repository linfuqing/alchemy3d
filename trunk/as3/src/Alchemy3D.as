package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.View3D;
	
	import flash.display.Sprite;

	[SWF(backgroundColor="#000000", width="800", height="600", frameRate="120")]
	public class Alchemy3D extends Sprite
	{
		private var viewport:View3D;
		private var scene:Scene3D;
		private var camera:Camera3D;
		
		public function Alchemy3D()
		{
			super();
			
			scene = new Scene3D();
			camera = new Camera3D();
			viewport = new View3D(800, 600, scene, camera);
		}
		
	}
}