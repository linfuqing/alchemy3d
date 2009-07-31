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
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.geom.ColorTransform;
	import flash.geom.Vector3D;
	import flash.text.TextField;
	import flash.text.TextFormat;

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Cloth extends Device
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		protected var light:Light3D;
		protected var lightObj:Sphere;
		
		protected var p:Plane;
		
		public function Cloth()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
			init();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("arial", 10, 0xffffff);
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat;
			tf2.autoSize = "right";
			tf2.text = "Alchemy3D Lighting Demo\nAuthor:牛牛猪 / LinFuQing";
			tf2.x = 400;
			tf2.y = 370;
			addChild(tf2);
		}
		
		protected function init(e:Event = null):void
		{
			var m:Material = new Material();
			m.ambient = new ColorTransform(0.05, 0.05, 0.05, 1);
			m.diffuse = new ColorTransform(.8, .3, .3, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 4;
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(0.5, 0, 0, 1);
			lightM.diffuse = new ColorTransform(1, 0, 0, 1);
			lightM.specular = new ColorTransform(0.6, 0, 0, 1);
			lightM.power = 4;
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			camera.eye.z = -400;
			
			viewport = new Viewport3D(600, 400, scene, camera);
			addViewport(viewport);
			
			lightObj = new Sphere(lightM, null, 20, 3, 2)
			scene.addEntity(lightObj);
			
			p = new Plane(m, null, 800, 800, 1, 1);
			scene.addEntity(p);
			p.rotationX = 45;
			p.y = -180;
			p.z = 800;
			
			p.setVertices(0, new Vector3D(100, 100, 100, 1));
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.MID_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = -120;
			light.source.x = 400;
			light.source.z = 0;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 0, 0, 1);
			light.specular = new ColorTransform(0.6, 0, 0, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .0000001;
			
			showInfo();
			
			startRendering();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			camera.target = p.worldPosition;
			var mx:Number = viewport.mouseX / 200;
			var my:Number = - viewport.mouseY / 200;
			
			camera.hover(mx, my, 10);
			trace(p);
			
			super.onRenderTick(e);
		}
	}
}