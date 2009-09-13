package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.external.MD2;
	import cn.alchemy3d.external.Primitives;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.Texture;
	import cn.alchemy3d.tools.Basic;
	import cn.alchemy3d.tools.FPS;
	
	import flash.events.Event;
	import flash.geom.ColorTransform;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	import gs.TweenLite;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class TestMD2 extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		//protected var p:Plane;
		
		protected var md2:MD2;
		protected var md22:MD2;
		
		protected var t1:Texture;
		
		protected var lightObj:Entity;
		protected var lightObj2:Entity;;
		protected var lightObj3:Entity;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		protected var center:Entity;
		
		public function TestMD2()
		{
			super(640, 480);
			
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
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			
			t1 = new Texture(bl.getBitmapData("0"));
			
			center = new Entity();
			center.z = 300;
			viewport.scene.addChild(center);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.3, .3, .3, 1);
			m.diffuse = new ColorTransform(.6, .6, .6, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 4;
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(1, 0, 0, 1);
			
			var lightM2:Material = new Material();
			lightM2.ambient = new ColorTransform(0, 1, 0, 1);
			
			var lightM3:Material = new Material();
			lightM3.ambient = new ColorTransform(0, 0, 1, 1);
			
			lightObj = new Entity();
			lightObj.y = -150;
			lightObj.x = 50;
			lightObj.z = 200;
			viewport.scene.addChild( lightObj );
			
			lightObj2 = new Entity();
			lightObj2.y = 150;
			lightObj2.x = -150;
			lightObj2.z = 150;
			viewport.scene.addChild(lightObj2);
			
			lightObj3 = new Entity();
			lightObj3.y = 0;
			lightObj3.x = 70;
			lightObj3.z = 200;
			viewport.scene.addChild(lightObj3);
			
			md2 = new MD2(m, t1);
			md2.lightEnable = true;
			md2.load("asset/tris.jpg");
			//scene.addEntity(md2);
			this.viewport.scene.addChild( md2 );
			
			md2.rotationX = -90;
			md2.rotationY = 45;
			md2.y = -20;
			md2.x = 0;
			md2.z = 200;
			md2.scale = 10;
			
			var p:Primitives = new Primitives(m,t1);
			//p.lightEnable = true;
			p.toPlane();
			
			this.viewport.scene.addChild( p );
			
//			md22 = new MD2(m, t1);
//			md22.lightEnable = true;
//			md22.load("asset/md2/tris.MD2");
//			scene.addEntity(md22);
//			
//			md22.rotationX = -90;
//			md22.rotationY = 45;
//			md22.y = -20;
//			md22.x = 80;
//			md22.z = 200;
//			md22.scale = 3;
			
			light = new Light3D(lightObj);
			viewport.scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 0, 0, 1);
			light.specular = new ColorTransform(1, 0, 0, 1);
			light.attenuation1 = .0001;
			light.attenuation2 = .0000001;
			
			light2 = new Light3D(lightObj2);
			viewport.scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.HIGH_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 1, 0, 1);
			light2.attenuation1 = .0001;
			light2.attenuation2 = .0000001;
			
			light3 = new Light3D(lightObj3);
			viewport.scene.addLight(light3);
			light3.type = LightType.POINT_LIGHT;
			light3.mode = LightType.HIGH_MODE;
			light3.bOnOff = LightType.LIGHT_ON;
			light3.ambient = new ColorTransform(0, 0, 0, 1);
			light3.diffuse = new ColorTransform(0, 0, 1, 1);
			light3.specular = new ColorTransform(0, 0, 1, 1);
			light3.attenuation1 = .0001;
			light3.attenuation2 = .0000001;

			startRendering();
			
			showInfo();
			
			moveLight1(1);
			moveLight2(1);
			moveLight3(1);
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var target:int = 200 * dir + 100;
			TweenLite.to(lightObj, 3, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 200 * dir;
			TweenLite.to(lightObj2, 3.5, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight3(dir:int = 1):void
		{
			var target:int = 200 * dir;
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