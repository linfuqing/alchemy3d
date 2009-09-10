package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.external.MD2;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.objects.primitives.Sphere;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.events.Event;
	import flash.geom.ColorTransform;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	import gs.TweenLite;

	[SWF(width="800",height="600",backgroundColor="#000000",frameRate="60")]
	public class TestMD2 extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		protected var p:Plane;
		
		protected var md2:MD2;
		
		protected var t1:Texture;
		
		protected var lightObj:Sphere;
		protected var lightObj2:Sphere;;
		protected var lightObj3:Sphere;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		protected var center:Entity;
		
		public function TestMD2()
		{
			super();
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/desert.jpg", {id:"0"});
			bl.start();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("宋体", 16, 0xffffff, true);
			var tf:TextField = new TextField();
			tf.defaultTextFormat = tformat;
			tf.autoSize = "left";
			tf.text = "MD2加载器Demo";
			tf.x = 280;
			tf.y = 20;
			addChild(tf);
		}
		
		protected function init(e:Event = null):void
		{
			t1 = new Texture(bl.getBitmapData("0"));
			
			center = new Entity();
			center.z = 300;
			scene.addEntity(center);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(1, 1, 1, 1);
			m.diffuse = new ColorTransform(.6, .6, .6, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 256;
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(1, 0, 0, 1);
			
			var lightM2:Material = new Material();
			lightM2.ambient = new ColorTransform(0, 1, 0, 1);
			
			var lightM3:Material = new Material();
			lightM3.ambient = new ColorTransform(0, 0, 1, 1);
			
			lightObj = new Sphere(lightM, null, 10, 3, 2)
			lightObj.lightEnable = false;
			lightObj.y = -150;
			lightObj.x = 50;
			lightObj.z = 200;
			lightObj.renderMode = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32;
			scene.addEntity(lightObj);
			
			lightObj2 = new Sphere(lightM2, null, 10, 3, 2, "lightO2");
			lightObj2.lightEnable = false;
			lightObj2.renderMode = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32;
			lightObj2.y = 150;
			lightObj2.x = -150;
			lightObj2.z = 150;
			scene.addEntity(lightObj2);
			
			lightObj3 = new Sphere(lightM3, null, 10, 3, 2, "lightO3");
			lightObj3.lightEnable = false;
			lightObj3.renderMode = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32;
			lightObj3.y = 0;
			lightObj3.x = 70;
			lightObj3.z = 200;
			scene.addEntity(lightObj3);
			
			md2 = new MD2(m, t1);
			md2.load("asset/tris.md2");
//			md2.lightEnable = true;
			scene.addEntity(md2);
			
			md2.rotationX = -90;
			md2.rotationY = 90;
			md2.y = -20;
			md2.x = 50;
			md2.z = 300;
			md2.scale = 6;
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 0, 0, 1);
			light.specular = new ColorTransform(1, 0, 0, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .000001;
			
			light2 = new Light3D(lightObj2);
			scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.HIGH_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 1, 0, 1);
			light2.attenuation1 = .001;
			light2.attenuation2 = .000001;
			
			light3 = new Light3D(lightObj3);
			scene.addLight(light3);
			light3.type = LightType.POINT_LIGHT;
			light3.mode = LightType.HIGH_MODE;
			light3.bOnOff = LightType.LIGHT_ON;
			light3.ambient = new ColorTransform(0, 0, 0, 1);
			light3.diffuse = new ColorTransform(0, 0, 1, 1);
			light3.specular = new ColorTransform(0, 0, 1, 1);
			light3.attenuation1 = .001;
			light3.attenuation2 = .000001;

			startRendering();
			
			showInfo();
			
			moveLight1(1);
			moveLight2(1);
			moveLight3(1);
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var target:int = 300 * dir + 100;
			TweenLite.to(lightObj, 3, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 300 * dir;
			TweenLite.to(lightObj2, 3.5, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight3(dir:int = 1):void
		{
			var target:int = 300 * dir;
			TweenLite.to(lightObj3, 4, { y:target, onComplete:moveLight3, onCompleteParams:[dir * -1]});
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			super.onRenderTick(e);
			
			camera.target = center.worldPosition;
			var mx:Number = viewport.mouseX / 500;
			var my:Number = - viewport.mouseY / 500;
			
			camera.hover(mx, my, 10);
		}
	}
}