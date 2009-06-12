package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.View3D;
	
	import flash.display.Sprite;
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
			
			scene = new Scene3D();
			camera = new Camera3D();
			viewport = new View3D(800, 600, scene, camera);
			
			lib = Alchemy3DLib.getInstance();
			
			var t:Number = getTimer();
			lib.alchemy3DLib.test(this.graphics, 1000);
			trace(getTimer() - t);
			
			var i:int;
			t = getTimer();
			for (i = 0; i < 1000; i ++)
			{
				this.graphics.beginFill(0, 1);
				this.graphics.drawRect(200, 200, 100, 100);
				this.graphics.endFill();
			}
			trace(getTimer() - t);
		}
		
	}
}