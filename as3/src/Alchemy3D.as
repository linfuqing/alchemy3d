package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Screen2D;
	import cn.alchemy3d.view.View3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;

	[SWF(width="800",height="400",backgroundColor="#000000",frameRate="60")]
	public class Alchemy3D extends Screen2D
	{
		private var viewport:View3D;
		private var viewport2:View3D;
		
		private var camera:Camera3D;
		private var camera2:Camera3D;
		
		private var scene:Scene3D;
		
		protected var p:Plane;
		protected var p2:Plane;
		
		public function Alchemy3D()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.HIGH;
			stage.frameRate = 60;
			
			scene = new Scene3D();
			camera = new Camera3D(0, 90, 100, 5000);
			camera2 = new Camera3D(0, 90, 100, 5000);
			camera2.eye.z = 1200;
			camera2.eye.y = 500;
			camera2.eye.rotationX = 90;
			viewport = new View3D(400, 400, scene, camera);
			viewport2 = new View3D(400, 400, scene, camera2);
			viewport2.x = 400;
			
			addChild(viewport);
			//addChild(viewport2);
			
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			p = new Plane(1, 200, 200, 1, 1, "test");
			scene.addChild(p);
			p.z = 500;
			
			p2 = new Plane(1, 250, 250, 1, 1);
			scene.addChild(p2);
			p2.x = 300;
			p2.z = 1200;
			p2.rotationX = 45;
			p2.rotationY = 45;
			
//			lib.buffer.position = p.verticesPointer;
//			for (var i:int = 0; i < 10; i ++)
//			{
//				trace(lib.buffer.readDouble());
//			}

			startRendering();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			p.rotationX ++;
			p.rotationY ++;
			p.rotationZ ++;
			
			//camera.eye.rotationY ++;
			//camera.fov ++;
			
//			p2.rotationX --;
//			p2.rotationY --;
//			p2.rotationZ --;
			
			super.onRenderTick(e);
		}
	}
}