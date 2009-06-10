package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.engines.ZBufferEngine;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Viewport3D;
	
	import flash.display.Sprite;

	[SWF(backgroundColor="#000000", width="800", height="600", frameRate="120")]
	public class Alchemy3D extends Sprite
	{
		private var viewport:Viewport3D;
		private var scene:Scene3D;
		private var engine:ZBufferEngine;
		private var camera:Camera3D;
		
		public function Alchemy3D()
		{
			super();
			
			viewport = new Viewport3D(800, 600);
			scene = new Scene3D();
			engine = new ZBufferEngine();
			camera = new Camera3D();
		}
		
	}
}