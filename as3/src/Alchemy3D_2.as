package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.objects.DisplayObject3D;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Device;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;

	[SWF(width="400",height="400",backgroundColor="#000000",frameRate="60")]
	public class Alchemy3D_2 extends Device
	{
		private var viewport:Viewport3D;
		
		private var camera:Camera3D;
		
		private var scene:Scene3D;
		
		protected var p:Plane;
		protected var p2:Plane;
		
		public function Alchemy3D_2()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.HIGH;
			stage.frameRate = 60;
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			
			viewport = new Viewport3D(400, 400, scene, camera);
			
			addViewport(viewport);
			
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			p = new Plane(1, 300, 300, 1, 1, "test");
			scene.addChild(p);
			p.z = 500;

			p2 = new Plane(1, 300, 300, 1, 1, "test2");
			scene.addChild(p2);
			p2.z = 500;
			//p2.x = 200;

			startRendering();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
//			var mx:Number = viewport4.mouseX / 1500;
//			var my:Number = - viewport4.mouseY / 1500;
//			
//			camera4.hover(mx, my, 10);
			
			p.rotationX ++;
			p.rotationY ++;
			p.rotationZ ++;
//			
//			//camera.eye.rotationY ++;
//			//camera.fov ++;
//			
//			p2.rotationX --;
//			p2.rotationY --;
			p2.rotationZ --;
			
			super.onRenderTick(e);
		}
	}
}