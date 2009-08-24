package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
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
	import flash.events.MouseEvent;
	import flash.geom.ColorTransform;
	import flash.text.TextField;
	import flash.text.TextFormat;

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Test extends Device
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		protected var p:Plane;
		protected var p2:Sphere;
		
		protected var t1:Texture;
		protected var bl:BulkLoader;
		
		protected var light:Light3D;
		protected var lightObj:Sphere;

		public function Test()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
//			bl = new BulkLoader("main-site");
//			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
//			bl.add("asset/earth.jpg", {id:"0"});
//			bl.start();

			init();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene, viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("arial", 12, 0xffffff, null, null, null, null, null, "right");
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat;
			tf2.autoSize = "right";
			tf2.text = "Alchemy3D\nAuthor : 牛牛猪 / LinFuQing";
			tf2.selectable = false;
			tf2.x = 440;
			tf2.y = 363;
			addChild(tf2);
		}
		
		protected function init(e:Event = null):void
		{
//			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
//			
//			t1 = new Texture(bl.getBitmapData("0"));
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.01, .01, .01, 1);
			m.diffuse = new ColorTransform(1, 1, 1, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 32;
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 100, 5000);
			addCamera(camera);
			
			viewport = new Viewport3D(600, 400, scene, camera);
			addViewport(viewport);
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(1, 1, 0, 1);
			lightM.diffuse = new ColorTransform(1, 1, 0, 1);
			lightObj = new Sphere(lightM, null, 10, 3, 2)
			scene.addEntity(lightObj);
			
			p = new Plane(m, null, 80, 80, 10, 10, "p");
			scene.addEntity(p);
			p.rotationX = 45;
			p.rotationZ = 85;
			p.z = 20;
			
			p2 = new Sphere(m, null, 20, 32, 24, "p2");
			p.addEntity(p2);
			p2.y = -50;
			p2.z = 0;
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = 600;
			light.source.x = 300;
			light.source.z = 0;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 1, 1, 1);
			light.specular = new ColorTransform(1, 1, 1, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .000001;
			
			showInfo();
			
			startRendering();
			addEventListener(MouseEvent.CLICK, onMouseClick);
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
//			camera.target = p.worldPosition;
//			var mx:Number = viewport.mouseX / 200;
//			var my:Number = - viewport.mouseY / 200;
//			
//			camera.hover(mx, my, 10);
			
			p.rotationZ ++;
//			p.z --;
			
			super.onRenderTick(e);
		}
		
		protected function onMouseClick(e:MouseEvent):void
		{
			p.rotationZ +=5;
			
			trace(p.rotationZ);
		}
	}
}