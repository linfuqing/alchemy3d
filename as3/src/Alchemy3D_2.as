package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.device.Device;
	import cn.alchemy3d.lights.PointLight3D;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.objects.primitives.Sphere;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.geom.ColorTransform;

	[SWF(width="400",height="400",backgroundColor="#006699",frameRate="60")]
	public class Alchemy3D_2 extends Device
	{
		private var viewport:Viewport3D;
		private var camera:Camera3D;
		private var scene:Scene3D;
		private var light:PointLight3D;
		
		protected var p:Plane;
		protected var p2:Plane;
		
		protected var s:Sphere;
		
		public function Alchemy3D_2()
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
			
			viewport = new Viewport3D(400, 400, scene, camera);
			addViewport(viewport);
			
			light = new PointLight3D();
			scene.addLight(light);
			light.source.z = 300;
			light.ambient = new ColorTransform(0.0, 0.5, 0.5, 1); 
			
			p = new Plane(m, 300, 300, 1, 1, "test");
			scene.addEntity(p);
			p.rotationZ = 45;
			p.rotationX = 45;
			p.z = 500;
			
//			s = new Sphere(m, 150, 18, 16)
//			scene.addEntity(s);
//			s.z = 500;

//			p2 = new Plane(m, 300, 300, 1, 1, "test2");
//			scene.addEntity(p2);
//			p2.z = 500;
//			p2.x = 200;

			stage.addEventListener(KeyboardEvent.KEY_DOWN, function ():void {onRenderTick();});

			startRendering();
			
			var fps:FPS = new FPS(scene);
			addChild(fps);
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
//			var mx:Number = viewport4.mouseX / 1500;
//			var my:Number = - viewport4.mouseY / 1500;
//			
//			camera4.hover(mx, my, 10);
			
//			p.rotationX ++;
//			p.rotationY ++;
//			p.rotationZ ++;

//			s.rotationY ++;
//			
//			//camera.eye.rotationY ++;
//			//camera.fov ++;
//			
//			p2.rotationX --;
//			p2.rotationY --;
//			p2.rotationZ --;
			light.source.x ++;
			
			super.onRenderTick(e);
		}
	}
}