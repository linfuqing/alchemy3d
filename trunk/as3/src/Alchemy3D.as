package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.device.Basic;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.geom.ColorTransform;

	[SWF(width="642",height="482",backgroundColor="#000000",frameRate="60")]
	public class Alchemy3D extends Basic
	{
		private var viewport:Viewport3D;
		private var viewport2:Viewport3D;
		private var viewport3:Viewport3D;
		private var viewport4:Viewport3D;
		
		private var camera:Camera3D;
		private var camera2:Camera3D;
		private var camera3:Camera3D;
		private var camera4:Camera3D;
		
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
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(1, 0, 0, 1);
			m.diffuse = new ColorTransform(1, 0, 0, 1);
			m.specular = new ColorTransform(1, 0, 0, 1);
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			camera.eye.y = 100;
			camera.eye.z = -400;
			
			camera2 = new Camera3D(0, 90, 100, 5000);
			addCamera(camera2);
			camera2.eye.y = 800;
			camera2.eye.rotationX = 45;
			
			camera3 = new Camera3D(0, 90, 100, 5000);
			addCamera(camera3);
			camera3.eye.y = -800;
			camera3.eye.rotationX = -45;
			
			camera4 = new Camera3D(0, 90, 100, 5000);
			addCamera(camera4);
			camera4.eye.y = 200;
			camera4.eye.z = -300;
			
			viewport = new Viewport3D(320, 240, scene, camera);
			viewport.backgroundColor = 0x222222;
			
			viewport2 = new Viewport3D(320, 240, scene, camera2);
			viewport2.backgroundColor = 0x222222;
			viewport2.x = 322;
			
			viewport3 = new Viewport3D(320, 240, scene, camera3);
			viewport3.backgroundColor = 0x222222;
			viewport3.y = 242;
			
			viewport4 = new Viewport3D(320, 240, scene, camera4);
			viewport4.backgroundColor = 0x222222;
			viewport4.x = 322;
			viewport4.y = 242;
			
			addViewport(viewport);
			addViewport(viewport2);
			addViewport(viewport3);
			addViewport(viewport4);
			
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			p = new Plane(m, 250, 250, 1, 1, "test");
			scene.addEntity(p);
			p.y = 250;
			p.z = 500;
			
			p2 = new Plane(m, 250, 250, 1, 1, "test2");
			scene.addEntity(p2);
			p2.z = 500;
			p2.rotationX = 90;

			startRendering();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			var mx:Number = viewport4.mouseX / 1500;
			var my:Number = - viewport4.mouseY / 1500;
			
			camera4.hover(mx, my, 10);
			
//			p.rotationX ++;
			p.rotationY ++;
//			p.rotationZ ++;
			
			//camera.eye.rotationY ++;
			//camera.fov ++;
			
//			p2.rotationX --;
//			p2.rotationY --;
			p2.rotationX ++;
			
			super.onRenderTick(e);
		}
	}
}