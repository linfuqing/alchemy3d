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
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.render.Texture;
	import cn.alchemy3d.terrain.MeshTerrain;
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

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Test8X extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		//protected var p:Plane;
		
		protected var md2:MD2;
		protected var md22:MD2;
		
		protected var t0:Texture;
		protected var t1:Texture;
		
		protected var lightObj:Entity;
		protected var lightObj2:Primitives;;
		protected var lightObj3:Entity;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		protected var center:Entity;
		
		public function Test8X()
		{
			super(600, 400, 90, 20, 12000);
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/sky_beiz.jp_L04899.jpg", {id:"0"});
			bl.add("asset/texture.jpg", {id:"1"});
			bl.add("asset/md2/blograde.jpg", {id:"2"});
			bl.start();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene);
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
			
			t0 = new Texture(bl.getBitmapData("2"));
			t1 = new Texture(bl.getBitmapData("1"));
			t0.perspectiveDist = 800;
			t1.perspectiveDist = 800;
			t1.addressMode = Texture.ADDRESS_MODE_WRAP;
			t0.addressMode = Texture.ADDRESS_MODE_WRAP;
			
			viewport.backgroundImage = bl.getBitmapData("0");
			
//			camera.z = -2000;
			
			center = new Entity();
			center.z = 0;
			viewport.scene.addChild(center);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(1, 1, 1, 1);
			m.diffuse = new ColorTransform(.4, .4, .4, 1);
			m.specular = new ColorTransform(0, 0, 0, 1);
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(0, .2, 0, 1);
			m2.diffuse = new ColorTransform(0, 1, 0, 1);
			m2.specular = new ColorTransform(0, 0, 0, 1);
			m2.power = 0;
			
			lightObj = new Entity();
			lightObj.y = 40000;
			lightObj.x = 0;
			lightObj.z = 300;
			viewport.scene.addChild( lightObj );
			
			lightObj2 = new Primitives(m, null, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
			lightObj2.toPlane(150, 150, 1, 1);
			lightObj2.y = 5000;
			lightObj2.x = 0;
			lightObj2.z = 2000;
			viewport.scene.addChild(lightObj2);
			
			lightObj3 = new Entity();
			lightObj3.y = 40000;
			lightObj3.x = 0;
			lightObj3.z = 0;
			viewport.scene.addChild(lightObj3);

			md2 = new MD2(m, t0, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32);
			md2.lightEnable = true;
			md2.mesh.useMipmap = true;
			md2.mesh.mipDist = 10000;
			md2.load("asset/md2/tris.md2");
			viewport.scene.addChild( md2 );
			
			md2.rotationX = -90;
			md2.rotationY = -90;
			md2.x = 0;
			md2.z = 500;
			md2.scale = 6;
			
//			var p:Primitives = new Primitives(m2, null, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
//			p.toPlane(150, 150, 1, 1);
//			p.mesh.octreeDepth = 0;
//			p.z = 0;
//			p.y = -10;
//			p.rotationX = 45;
//			this.viewport.scene.addChild(p);
			
			var terrain:MeshTerrain = new MeshTerrain(null, m2, t1, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32);
			terrain.buildOn(60000, 60000, 40000);
			terrain.mesh.lightEnable = true;
			terrain.mesh.octreeDepth = 3;
			terrain.mesh.useMipmap = true;
			terrain.mesh.mipDist = 8000;
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
			light2.mode = LightType.MID_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 0, 0, 1);
			light2.attenuation1 = 0.0002;
			light2.attenuation2 = 0;
			
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
			var target:int = 10000 * dir;
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
			
			md2.z += 5;
//			camera.z += 5;
			
			//center.z ++;
//			center.rotationY ++;
			
//			camera.target = md2.worldPosition;
			
			var mx:Number = viewport.mouseX / 30;
			var my:Number = - viewport.mouseY / 30;
			
			camera.hover(mx, my, 10);
		}
	}
}