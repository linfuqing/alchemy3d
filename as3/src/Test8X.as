package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.external.MD2;
	import cn.alchemy3d.external.Primitives;
	import cn.alchemy3d.fog.Fog;
	import cn.alchemy3d.geom.Mesh3D;
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

	[SWF(width="640",height="400",backgroundColor="#ffffff",frameRate="60")]
	public class Test8X extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		//protected var p:Plane;
		
		protected var md2:MD2;
		protected var terrain:MeshTerrain;
		
		protected var t0:Texture;
		protected var t1:Texture;
		
		protected var lightObj:Primitives;
		protected var lightObj2:Primitives;;
		protected var lightObj3:Entity;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		protected var center:Entity;
		
		private var _fogState:Boolean = true;
		private var _textureState:Boolean = true;
		private var _materialState:Boolean = true;
		private var _renderModeState:int = 1;
		private var _lightState:Boolean = true;
		private var isTarget:Boolean = false;
		
		public function Test8X()
		{
			super(640, 400, 100, 20, 6000);
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/sky_beiz.jp_L04899.jpg", {id:"0"});
			bl.add("asset/texture.jpg", {id:"1"});
			bl.add("asset/md2/blade_black.jpg", {id:"2"});
			bl.add("asset/HighMap.jpg", {id:"3"});
			bl.start();
		}
		
		protected function init(e:Event):void
		{
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			
			t0 = new Texture(bl.getBitmapData("2"));
			t1 = new Texture(bl.getBitmapData("1"));
			t0.perspectiveDist = 1000;
			t1.perspectiveDist = 1000;
			t1.addressMode = Texture.ADDRESS_MODE_WRAP;
			
			viewport.backgroundColor = 0xffffffff;
			
			camera.z = -1000;
			camera.y = 500;
			
			center = new Entity("center");
			center.z = 500;
			center.y = 100;
			viewport.scene.addChild(center);
			
			var fog:Fog = new Fog(new ColorTransform(1, 1, 1, 1), 2000, 5000, 1, Fog.FOG_LINEAR);
			scene.addFog(fog);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.2, .2, .2, 1);
			m.diffuse = new ColorTransform(.6, .6, .6, .5);
			m.specular = new ColorTransform(0, 0, 0, 1);
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(.1, .1, .1, 1);
			m2.diffuse = new ColorTransform(1, 0.6, 0, 0.5);
			m2.specular = new ColorTransform(0, 0, 0, 1);
			m2.power = 0;
			
			lightObj = new Primitives(m, null, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
			lightObj.toPlane(50, 50, 1, 1);
			lightObj.y = 1500;
			lightObj.x = 0;
			lightObj.z = 0;
			viewport.scene.addChild( lightObj );
			
			lightObj2 = new Primitives(m, null, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
			lightObj2.toPlane(50, 50, 1, 1);
			lightObj2.y = 1500;
			lightObj2.x = 0;
			lightObj2.z = 0;
			viewport.scene.addChild(lightObj2);
//			
//			lightObj3 = new Entity();
//			lightObj3.y = 40000;
//			lightObj3.x = 0;
//			lightObj3.z = 0;
//			viewport.scene.addChild(lightObj3);

			md2 = new MD2(m, t0, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
			md2.addEventListener(Event.COMPLETE, MD2LoadComplete);
			md2.load("asset/md2/tris.jpg", 5);
			md2.lightEnable = true;
			md2.mesh.useMipmap = true;
			md2.mesh.mipDist = 3000;
			md2.mesh.fogEnable = true;
			md2.mesh.terrainTrace = true;
			
			md2.rotationX = -90;
			md2.rotationY = -90;
			md2.scale = 8;
			
//			var p:Primitives = new Primitives(m2, null, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
//			p.toPlane(150, 150, 1, 1);
//			p.mesh.octreeDepth = 0;
//			p.z = 0;
//			p.y = -10;
//			p.rotationX = 45;
//			this.viewport.scene.addChild(p);
			
			terrain = new MeshTerrain(bl.getBitmapData("3"), m2, t1, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
			terrain.buildOn(15000, 15000, 3000);
			terrain.mesh.lightEnable = true;
			terrain.mesh.octreeDepth = 2;
			terrain.mesh.useMipmap = true;
			terrain.mesh.mipDist = 4000;
			terrain.mesh.fogEnable = true;
//			terrain.mesh.setAttribute(Mesh3D.ALPHA_KEY, 127);
			terrain.z = 3000;
			
			viewport.scene.addChild(terrain);

			light = new Light3D(lightObj);
			viewport.scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.MID_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 0, 0, 1);
			light.specular = new ColorTransform(0, 0, 0, 1);
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
			light2.attenuation1 = 0.0001;
			light2.attenuation2 = 0;
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
		}
		
		protected function MD2LoadComplete(e:Event = null):void
		{
			viewport.scene.addChild( md2 );
			md2.play(true, "stand");
			
			startRendering();
			
			showInfo();
			
			moveLight1(1);
			moveLight2(1);
//			moveLight3(1);
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if ( e.keyCode == 67 )
			{
				isTarget = ! isTarget;
			}
			
			if ( e.keyCode == Keyboard.UP )
			{
				md2.play(false, "run");
				md2.rotationY = -90;
				md2.z += 25;
				center.z += 25;
				camera.z += 25;
			}
				
			if ( e.keyCode == Keyboard.DOWN )
			{
				md2.play(false, "run");
				md2.rotationY = 90;
				md2.z -= 25;
				center.z -= 25;
				camera.z -= 25;
			}
			
			if ( e.keyCode == Keyboard.LEFT )
			{
				md2.play(false, "run");
				md2.rotationY = 180;
				md2.x -= 25;
				camera.x -= 25;
			}
			
			if ( e.keyCode == Keyboard.RIGHT )
			{
				md2.play(false, "run");
				md2.rotationY = 0;
				md2.x += 25;
				camera.x += 25;
			}
				
			if ( e.keyCode == 70 )
			{
				if (_fogState)
				{
					md2.mesh.setAttribute(Mesh3D.FOG_KEY, 0);
					terrain.mesh.setAttribute(Mesh3D.FOG_KEY, 0);
				}
				else
				{
					md2.mesh.setAttribute(Mesh3D.FOG_KEY, 1);
					terrain.mesh.setAttribute(Mesh3D.FOG_KEY, 1);
				}
				
				_fogState = ! _fogState;
			}
			
			if ( e.keyCode == 84 )
			{
				if (_textureState)
				{
					md2.mesh.setAttribute(Mesh3D.TEXTURE_KEY, 0);
					terrain.mesh.setAttribute(Mesh3D.TEXTURE_KEY, 0);
				}
				else
				{
					md2.mesh.setAttribute(Mesh3D.TEXTURE_KEY, t0.pointer);
					terrain.mesh.setAttribute(Mesh3D.TEXTURE_KEY, t1.pointer);
				}
				
				_textureState = ! _textureState;
			}
			
			if ( e.keyCode == 76 )
			{
				if (_lightState)
				{
					md2.mesh.lightEnable = false;
					md2.mesh.setAttribute(Mesh3D.LIGHT_KEY, 0);
					terrain.mesh.setAttribute(Mesh3D.LIGHT_KEY, 0);
				}
				else
				{
					md2.mesh.lightEnable = true;
					md2.mesh.setAttribute(Mesh3D.LIGHT_KEY, 1);
					terrain.mesh.setAttribute(Mesh3D.LIGHT_KEY, 1);
				}
				
				_lightState = ! _lightState;
			}
			
			if ( e.keyCode == 82 )
			{
				_renderModeState ++;
				
				if (_renderModeState == 1)
				{
					md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
				}
				else if (_renderModeState == 2)
				{
					md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
				}
				else if (_renderModeState == 3)
				{
					md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32);
				}
				else if (_renderModeState == 4)
				{
					md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
				}
				
				_renderModeState = _renderModeState >= 4 ? 0 : _renderModeState;
			}
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var target:int = 8000 * dir + 100;
			TweenLite.to(lightObj, 5, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 8000 * dir;
			TweenLite.to(lightObj2, 5, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight3(dir:int = 1):void
		{
			var target:int = 200 * dir;
			TweenLite.to(lightObj3, 4, { y:target, onComplete:moveLight3, onCompleteParams:[dir * -1]});
		}
		
		private var k:Number;
		
		override protected function onRenderTick(e:Event = null):void
		{
			k = md2.worldPosition.y;
			
			super.onRenderTick(e);
			
			k = md2.worldPosition.y - k;
			
			if ( isTarget )
			{
				camera.target = md2.worldPosition;
				
				var mx:Number = viewport.mouseX / 100;
				var my:Number = - viewport.mouseY / 350;
				
				camera.hover(mx, my, 10);
			}
			else
				camera.target = null;
				
			camera.y += k;
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("宋体", 14, 0, true);
			var tf:TextField = new TextField();
			tf.defaultTextFormat = tformat;
			tf.autoSize = "left";
			tf.selectable = false;
			tf.text = "Terrain Demo";
			tf.x = 280;
			tf.y = 20;
			addChild(tf);
			
			var tformat1:TextFormat = new TextFormat("宋体", 12, 0, false, false, false, null, null, "right");
			var tf1:TextField = new TextField();
			tf1.defaultTextFormat = tformat1;
			tf1.autoSize = "right";
			tf.selectable = false;
			tf1.text = "\"R\":change render mode";
			tf1.x = 500;
			tf1.y = 40;
			addChild(tf1);
			
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat1;
			tf2.autoSize = "right";
			tf2.selectable = false;
			tf2.text = "\"T\":change texture";
			tf2.x = 500;
			tf2.y = 60;
			addChild(tf2);
			
			var tf3:TextField = new TextField();
			tf3.defaultTextFormat = tformat1;
			tf3.autoSize = "right";
			tf3.selectable = false;
			tf3.text = "\"C\":change camera mode";
			tf3.x = 500;
			tf3.y = 80;
			addChild(tf3);
			
			var tf4:TextField = new TextField();
			tf4.defaultTextFormat = tformat1;
			tf4.autoSize = "right";
			tf4.selectable = false;
			tf4.text = "\"L\":enable light";
			tf4.x = 500;
			tf4.y = 100;
			addChild(tf4);
			
			var tf5:TextField = new TextField();
			tf5.defaultTextFormat = tformat1;
			tf5.autoSize = "right";
			tf5.selectable = false;
			tf5.text = "\"F\":enable fog effect";
			tf5.x = 500;
			tf5.y = 120;
			addChild(tf5);
			
			var tf6:TextField = new TextField();
			tf6.defaultTextFormat = tformat1;
			tf6.autoSize = "right";
			tf6.selectable = false;
			tf6.text = "\"Arrow\":move the model";
			tf6.x = 500;
			tf6.y = 140;
			addChild(tf6);
		}
	}
}