//package
//{
//	import br.com.stimuli.loading.BulkLoader;
//	import br.com.stimuli.loading.BulkProgressEvent;
//	
//	import cn.alchemy3d.container.Entity;
//	import cn.alchemy3d.external.MD2;
//	import cn.alchemy3d.lights.Light3D;
//	import cn.alchemy3d.lights.LightType;
//	import cn.alchemy3d.render.Material;
//	import cn.alchemy3d.render.RenderMode;
//	import cn.alchemy3d.render.Texture;
//	import cn.alchemy3d.terrain.MeshTerrain;
//	import cn.alchemy3d.tools.Basic;
//	import cn.alchemy3d.tools.FPS;
//	
//	import flash.events.Event;
//	import flash.events.KeyboardEvent;
//	import flash.geom.ColorTransform
//	import flash.net.URLLoader;
//	import flash.text.TextField;
//	import flash.text.TextFormat;
//	import flash.ui.Keyboard;
//	
//	import gs.TweenLite;
//
//	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
//	public class TestMD2 extends Basic
//	{
//		protected var bl:BulkLoader;
//		
//		private var loader:URLLoader;
//		
//		//protected var p:Plane;
//		
//		protected var md2:MD2;
//		protected var md22:MD2;
//		
//		protected var t0:Texture;
//		protected var t1:Texture;
//		
//		protected var lightObj:Entity;
//		protected var lightObj2:Entity;;
//		protected var lightObj3:Entity;
//		
//		protected var light:Light3D;
//		protected var light2:Light3D;
//		protected var light3:Light3D;
//		
//		protected var center:Entity;
//		
//		public function TestMD2()
//		{
//			super(640, 480, 90, 50, 3800);
//			
//			bl = new BulkLoader("main-site");
//			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
//			bl.add("asset/skin01.jpg", {id:"0"});
//			bl.add("asset/wood01.jpg", {id:"1"});
//			bl.start();
//			//init();
//		}
//		
//		private function showInfo():void
//		{
//			var fps:FPS = new FPS(viewport);
//			addChild(fps);
//			
//			var tformat:TextFormat = new TextFormat("宋体", 16, 0xffffff, true);
//			var tf:TextField = new TextField();
//			tf.defaultTextFormat = tformat;
//			tf.autoSize = "left";
//			tf.text = "MD2加载器Demo";
//			tf.x = 280;
//			tf.y = 20;
//			addChild(tf);
//		}
//		
//		protected function init(e:Event = null):void
//		{
//			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
//			
//			t0 = new Texture(bl.getBitmapData("0"));
//			t1 = new Texture(bl.getBitmapData("1"));
//			
//			center = new Entity();
//			center.z = 300;
//			viewport.scene.addChild(center);
//			
//			var m:Material = new Material();
//			m.ambient = new ColorTransform(1, 1, 1, 1);
//			m.diffuse = new ColorTransform(.4, .4, .4, 1);
//			m.specular = new ColorTransform(0, 0, 0, 1);
//			
//			var m2:Material = new Material();
//			m2.ambient = new ColorTransform(1, 1, 1, 1);
//			m2.diffuse = new ColorTransform(0, 0, 0, 1);
//			m2.specular = new ColorTransform(0, 0, 0, 1);
//			
//			lightObj = new Entity();
//			lightObj.y = 1000;
//			lightObj.x = -1000;
//			lightObj.z = -1000;
//			viewport.scene.addChild( lightObj );
//			
//			lightObj2 = new Entity();
//			lightObj2.y = 1000;
//			lightObj2.x = 700;
//			lightObj2.z = 0;
//			viewport.scene.addChild(lightObj2);
//			
//			lightObj3 = new Entity();
//			lightObj3.y = 1000;
//			lightObj3.x = 500;
//			lightObj3.z = 1000;
//			viewport.scene.addChild(lightObj3);
//			
//			md2 = new MD2(m, t1, RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32);
//			md2.lightEnable = true;
//			md2.load("asset/md2/tris.md2");
//			this.viewport.scene.addChild( md2 );
//			
//			md2.rotationX = -90;
//			md2.rotationY = 45;
//			md2.y = 10000;
//			md2.x = 0;
//			md2.z = 400;
//			md2.scale = 3;
//			
////			var p:Primitives = new Primitives(m, t0, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32);
////			p.mesh.lightEnable = true;
////			p.toPlane(500,500,2,2);
////			p.z = 300;
////			this.viewport.scene.addChild( p );
//			
//			var terrain:MeshTerrain = new MeshTerrain(null, m2, null);
//			terrain.mesh.lightEnable = true;
//			terrain.buildOn(10000, 10000, 2000);
//			terrain.z = 0;
//			
//			viewport.scene.addChild(terrain);
//
//			light = new Light3D(lightObj);
//			viewport.scene.addLight(light);
//			light.type = LightType.POINT_LIGHT;
//			light.mode = LightType.HIGH_MODE;
//			light.bOnOff = LightType.LIGHT_ON;
//			light.ambient = new ColorTransform(0, 0, 0, 1);
//			light.diffuse = new ColorTransform(1, 0, 0, 1);
//			light.specular = new ColorTransform(1, 0, 0, 1);
//			light.attenuation1 = 0.0001;
//			light.attenuation2 = 0;
//			
//			light2 = new Light3D(lightObj2);
//			viewport.scene.addLight(light2);
//			light2.type = LightType.POINT_LIGHT;
//			light2.mode = LightType.HIGH_MODE;
//			light2.bOnOff = LightType.LIGHT_ON;
//			light2.ambient = new ColorTransform(0, 0, 0, 1);
//			light2.diffuse = new ColorTransform(0, 1, 0, 1);
//			light2.specular = new ColorTransform(0, 1, 0, 1);
//			light2.attenuation1 = 0.0001;
//			light2.attenuation2 = 0;
//			
//			light3 = new Light3D(lightObj3);
//			viewport.scene.addLight(light3);
//			light3.type = LightType.POINT_LIGHT;
//			light3.mode = LightType.HIGH_MODE;
//			light3.bOnOff = LightType.LIGHT_ON;
//			light3.ambient = new ColorTransform(0, 0, 0, 1);
//			light3.diffuse = new ColorTransform(0, 0, 1, 1);
//			light3.specular = new ColorTransform(0, 0, 1, 1);
//			light3.attenuation1 = 0.0001;
//			light3.attenuation2 = 0;
//
//			startRendering();
//			
//			showInfo();
//			
//			moveLight1(100);
//			moveLight2(100);
//			moveLight3(100);
//			
//			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
//		}
//		
//		protected function onKeyDown(e:KeyboardEvent):void
//		{
//			if ( e.keyCode == Keyboard.UP )
//				camera.z += 50;
//				
//			if ( e.keyCode == Keyboard.DOWN )
//				camera.z -= 50;
//		}
//		
//		protected function moveLight1(dir:int = 1):void
//		{
//			var target:int = 200 * dir + 100;
//			TweenLite.to(lightObj, 3, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
//		}
//		
//		protected function moveLight2(dir:int = 1):void
//		{
//			var target:int = 200 * dir;
//			TweenLite.to(lightObj2, 3.5, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
//		}
//		
//		protected function moveLight3(dir:int = 1):void
//		{
//			var target:int = 200 * dir;
//			TweenLite.to(lightObj3, 4, { y:target, onComplete:moveLight3, onCompleteParams:[dir * -1]});
//		}
//		
//		override protected function onRenderTick(e:Event = null):void
//		{
//			super.onRenderTick(e);
//			
//			//center.z ++;
////			center.rotationY ++;
//			md2.z += 5;
//			
//			//camera.target = md2.worldPosition;
//			var mx:Number = viewport.mouseX / 300;
//			var my:Number = - viewport.mouseY / 100;
//			camera.x += ( md2.x - camera.x ) * .3;
//			camera.y += ( md2.y - camera.y ) * .3;
//			camera.z += ( md2.z - camera.z ) * .3;
//			//camera.hover(mx, my, 10);
//		}
//	}
//}
package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.external.MD2;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.render.Material;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.render.Texture;
	import cn.alchemy3d.terrain.MeshTerrain;
	import cn.alchemy3d.terrain.TerrainAddressMode;
	import cn.alchemy3d.tools.Basic;
	import cn.alchemy3d.tools.FPS;
	
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.geom.ColorTransform;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;
	
	import gs.TweenLite;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class TestMD2 extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		//protected var p:Plane;
		
		protected var md2:MD2;
		protected var md22:MD2;
		
		protected var t0:Texture;
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
			super(640, 480, 90, 10, 10000);
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/texture.jpg", {id:"1"});
			bl.start();
//			init();
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
			
			t1 = new Texture(bl.getBitmapData("1"));
			
			camera.z = -1000;
			
			center = new Entity();
			center.z = 0;
			viewport.scene.addChild(center);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(0, 0, 0, 1);
			m.diffuse = new ColorTransform(.4, .4, .4, 1);
			m.specular = new ColorTransform(0, 0, 0, 1);
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(0, .1, 0, 1);
			m2.diffuse = new ColorTransform(0, 1, 0, 1);
			m2.specular = new ColorTransform(0, 0, 0, 1);
			m2.power = 0;
			
			lightObj = new Entity();
			lightObj.y = 1000;
			lightObj.x = 0;
			lightObj.z = 300;
			viewport.scene.addChild( lightObj );
			
			lightObj2 = new Entity();
			lightObj2.y = 3000;
			lightObj2.x = 0;
			lightObj2.z = 4000;
			viewport.scene.addChild(lightObj2);
			
			lightObj3 = new Entity();
			lightObj3.y = 4000;
			lightObj3.x = 0;
			lightObj3.z = 0;
			viewport.scene.addChild(lightObj3);

//			md2 = new MD2(m, t1, RenderMode.RENDER_TEXTRUED_TRIANGLE_INVZB_32);
//			md2.lightEnable = true;
//			md2.load("asset/md2/tris.md2");
//			viewport.scene.addChild( md2 );
//			
//			md2.rotationX = -90;
////			md2.y = -1000;
//			md2.x = 0;
//			md2.z = 0;
//			md2.scale = 4;
			
//			var p:Primitives = new Primitives(m2, null, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
//			p.toPlane(150, 150, 1, 1);
//			p.mesh.octreeDepth = 0;
//			p.z = 0;
//			p.y = -10;
//			p.rotationX = 45;
//			this.viewport.scene.addChild(p);
			
			var terrain:MeshTerrain = new MeshTerrain(null, m2, t1, RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32);
			terrain.mesh.lightEnable = true;
			terrain.buildOn(80000, 80000, 40000, TerrainAddressMode.MIRROR);
			terrain.mesh.octreeDepth = 3;
			terrain.z = 0;
			
			viewport.scene.addChild(terrain);

			light = new Light3D(lightObj);
			viewport.scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 0, 0, 1);
			light.specular = new ColorTransform(1, 0, 0, 1);
			light.attenuation1 = 0.0001;
			light.attenuation2 = 0;
			
			light2 = new Light3D(lightObj2);
			viewport.scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.HIGH_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 0, 0, 1);
			light2.attenuation1 = 0.00001;
			light2.attenuation2 = 0.000000001;
			
			light3 = new Light3D(lightObj3);
			viewport.scene.addLight(light3);
			light3.type = LightType.POINT_LIGHT;
			light3.mode = LightType.HIGH_MODE;
			light3.bOnOff = LightType.LIGHT_ON;
			light3.ambient = new ColorTransform(0, 0, 0, 1);
			light3.diffuse = new ColorTransform(0, 0, 1, 1);
			light3.specular = new ColorTransform(0, 0, 1, 1);
			light3.attenuation1 = 0.0001;
			light3.attenuation2 = 0;

			startRendering();
			
			showInfo();
			
//			moveLight1(1);
			moveLight2(1);
//			moveLight3(1);
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if ( e.keyCode == Keyboard.UP )
				camera.z += 10;
				
			if ( e.keyCode == Keyboard.DOWN )
				camera.z -= 10;
				
			trace(camera.z);
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var target:int = 200 * dir + 100;
			TweenLite.to(lightObj, 3, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 5000 * dir;
			TweenLite.to(lightObj2, 5, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight3(dir:int = 1):void
		{
			var target:int = 200 * dir;
			TweenLite.to(lightObj3, 4, { y:target, onComplete:moveLight3, onCompleteParams:[dir * -1]});
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			super.onRenderTick(e);
			
//			md2.z += 5;
			
			//center.z ++;
//			center.rotationY ++;
			
			camera.target = center.worldPosition;
			var mx:Number = viewport.mouseX / 100;
			var my:Number = - viewport.mouseY / 100;
			
			camera.hover(mx, my, 10);
		}
	}
}