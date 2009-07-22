package
{
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.device.Device;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.objects.primitives.Sphere;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.geom.ColorTransform;
	
	import gs.TweenLite;
	import gs.easing.Linear;

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Alchemy3D_2 extends Device
	{
		private var viewport:Viewport3D;
		private var camera:Camera3D;
		private var scene:Scene3D;
		private var light:Light3D;
		
		protected var p:Plane;
		protected var p2:Plane;
		
		protected var s:Sphere;
		protected var s2:Sphere;
		protected var s3:Sphere;
		
		private var t:Texture;
		
		public function Alchemy3D_2()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(0, 0.03, 0, 1);
			m.diffuse = new ColorTransform(.5, .9, .3, 1);
			m.specular = new ColorTransform(1, 0, 0, 1);
			
			var m1:Material = new Material();
			m1.ambient = new ColorTransform(.1, 0, 0, 1);
			m1.diffuse = new ColorTransform(0.8, .2, 0.2, 1);
			m1.specular = new ColorTransform(1, 0, 0, 1);
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(0, 0.05, 0, 1);
			m2.diffuse = new ColorTransform(0, .4, 1, 1);
			m2.specular = new ColorTransform(1, 0, 0, 1);
			
			var m3:Material = new Material();
			m3.ambient = new ColorTransform(0, 0, 0, 1);
			m3.diffuse = new ColorTransform(.5, .5, .5, 1);
			m3.specular = new ColorTransform(1, 0, 0, 1);
			
//			t = new Texture();
//			t.load("asset/earth.jpg");
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			
			viewport = new Viewport3D(600, 400, scene, camera);
			addViewport(viewport);
			
			light = new Light3D();
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = 300;
			light.source.x = 300;
			light.source.z = -2300;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 1, 1, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .0000001;
			
			p = new Plane(m, null, 800, 800, 1, 1);
			scene.addEntity(p);
			p.rotationX = 90;
			p.y = -180;
			p.z = 800;
			
			s = new Sphere(m1, null, 180, 16, 12)
			scene.addEntity(s);
			s.z = 1100;
			
			s2 = new Sphere(m2, null, 120, 16, 12)
			scene.addEntity(s2);
			s2.x = -130;
			s2.y = -60;
			s2.z = 750;

			s3 = new Sphere(m3, null, 120, 16, 12)
			scene.addEntity(s3);
			s3.x = 150;
			s3.y = -60;
			s3.z = 700;

//			p2 = new Plane(m3, null, 300, 300, 2, 2);
//			scene.addEntity(p2);
//			p2.rotationX = 45;
//			p2.rotationY = 25;
//			p2.z = 700;
			
			startRendering();
			
			moveLight(1);
//			movePlane(1);
			
			var fps:FPS = new FPS(scene);
			addChild(fps);
		}
		
		protected function moveLight(dir:int = 1):void
		{
			var target:int = 2800 * dir - 500;
			TweenLite.to(light.source, 5, { z:target, onComplete:moveLight, onCompleteParams:[dir * -1]});
		}
		
		protected function movePlane(dir:int = 1):void
		{
			var target:int = 80 * dir;
			TweenLite.to(p2, 4, { rotationX:target, onComplete:movePlane, onCompleteParams:[dir * -1], ease:Linear.easeInOut});
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			camera.target = s.worldPosition;
			var mx:Number = viewport.mouseX / 200;
			var my:Number = - viewport.mouseY / 200;
			
			camera.hover(mx, my, 10);
//			
//			p.rotationX ++;
//			p.rotationY ++;
//			p.rotationZ ++;

//			s2.rotationZ ++;
//			
//			camera.eye.z ++;
//			p.rotationX++;
//			
//			p2.rotationX --;
//			p2.rotationY --;
//			p2.rotationZ --;
//			light.source.x ++;
			
			super.onRenderTick(e);
		}
	}
}