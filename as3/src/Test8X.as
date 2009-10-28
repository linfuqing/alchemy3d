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

	[SWF(width="640",height="400",backgroundColor="#000000",frameRate="60")]
	public class Test8X extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		//protected var p:Plane;
		
		protected var md2:MD2;
		protected var terrain:MeshTerrain;
		
		protected var t0:Texture;
		protected var t1:Texture;
		protected var t2:Texture;
		
		protected var center:Primitives;
		protected var lightObj:Primitives;
		protected var lightObj2:Primitives;
		protected var lightObj3:Entity;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		private var _fogState:Boolean = true;
		private var _textureState:Boolean = true;
		private var _materialState:Boolean = true;
		private var _renderModeState:int = 1;
		private var _lightState:Boolean = true;
		private var isTarget:Boolean = false;
		
		public function Test8X()
		{
			super(640, 400, 100, 20, 3000);
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/texture.jpg", {id:"1"});
			bl.add("asset/md2/blade_black.jpg", {id:"2"});
			bl.add("asset/map64.jpg", {id:"3"});
			bl.add("asset/water2.png", {id:"4"});
			bl.start();
		}
		
		protected function init(e:Event):void
		{
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			
			t0 = new Texture(bl.getBitmapData("2"));
			t1 = new Texture(bl.getBitmapData("1"));
			t2 = new Texture(bl.getBitmapData("4"));
			
			t0.perspectiveDist = 0;
			t1.perspectiveDist = 0;
			t2.perspectiveDist = 0;
			
//			viewport.backgroundColor = 0xffffffff;
			
			camera.z = -400;
			camera.y = 200;
			
			var fog:Fog = new Fog(new ColorTransform(1, 1, 1, 1), 1000, 2500, 1, Fog.FOG_LINEAR);
			scene.addFog(fog);
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.2, .2, .2, 1);
			m.diffuse = new ColorTransform(.6, .6, .6, 1);
			m.specular = new ColorTransform(0, 0, 0, 1);
			m.power = 0;
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(.1, .1, .1, 1);
			m2.diffuse = new ColorTransform(1, 0.6, 0, 1);
			m2.specular = new ColorTransform(0, 0, 0, 1);
			m2.power = 0;
			
			var m3:Material = new Material();
			m3.ambient = new ColorTransform(0, .2, .4, 1);
			m3.diffuse = new ColorTransform(.1, .2, .533, .6);
			m3.specular = new ColorTransform(1, 1, 1, 1);
			m3.power = 1024;
			
			lightObj = new Primitives(m, null, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
			lightObj.toPlane(50, 50, 1, 1);
			lightObj.y = 1000;
			lightObj.x = 0;
			lightObj.z = 0;
			viewport.scene.addChild( lightObj );
			
			lightObj2 = new Primitives(m, null, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
			lightObj2.toPlane(50, 50, 1, 1);
			lightObj2.y = 1000;
			lightObj2.x = 0;
			lightObj2.z = 0;
			viewport.scene.addChild(lightObj2);
			
//			lightObj3 = new Entity();
//			lightObj3.y = 40000;
//			lightObj3.x = 0;
//			lightObj3.z = 0;
//			viewport.scene.addChild(lightObj3);

//			md2 = new MD2(m, t0, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
//			md2.addEventListener(Event.COMPLETE, MD2LoadComplete);
//			md2.load("asset/md2/tris.jpg", 5);
//			md2.lightEnable = true;
//			md2.mesh.useMipmap = true;
//			md2.mesh.mipDist = 3000;
//			md2.mesh.fogEnable = true;
//			md2.mesh.terrainTrace = true;
//			
//			md2.rotationX = -90;
//			md2.rotationY = -90;
//			md2.scale = 2;
			
			center = new Primitives(m, null, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
			center.toPlane(80, 80, 1, 1);
			center.mesh.terrainTrace = true;
			viewport.scene.addChild(center);
			
			var p:Primitives = new Primitives(m3, t2, RenderMode.RENDER_TEXTRUED_TRIANGLE_INVZB_ALPHA_32);
			p.toPlane(2200, 2200, 10, 10);
			p.mesh.octreeDepth = 0;
			p.mesh.lightEnable = true;
			p.mesh.addressMode = Mesh3D.ADDRESS_MODE_WRAP;
			p.rotationX = 90;
			p.y = -100;
			viewport.scene.addChild(p);
			
			terrain = new MeshTerrain(bl.getBitmapData("3"), m2, t1, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_GSINVZB_32);
			terrain.buildOn(2200, 2200, 800);
			terrain.mesh.lightEnable = true;
			terrain.mesh.octreeDepth = 0;
//			terrain.mesh.useMipmap = true;
//			terrain.mesh.mipDist = 1000;
			terrain.mesh.addressMode = Mesh3D.ADDRESS_MODE_WRAP;
//			terrain.mesh.fogEnable = true;
			
			viewport.scene.addChild(terrain);

			light = new Light3D(lightObj);
			viewport.scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(0.8, 0.8, 0, 1);
			light.specular = new ColorTransform(0, 0, 0, 1);
			light.attenuation1 = 0.0001;
			light.attenuation2 = 0.00000001;
			
			light2 = new Light3D(lightObj2);
			viewport.scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.HIGH_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 0, 0, 1);
			light2.attenuation1 = 0.0001;
			light2.attenuation2 = 0.00000001;
			
			MD2LoadComplete();
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
			//viewport.scene.addChild( md2 );
			//md2.play(true, "stand");
			
			startRendering();
			
			showInfo();
			
			moveLight1(1);
			moveLight2(1);
//			moveLight3(1);
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			stage.addEventListener(KeyboardEvent.KEY_UP, onKeyUp );
		}
		
		private function onKeyUp( e:KeyboardEvent ):void
		{
			switch( e.keyCode )
			{
				case Keyboard.UP:
				case Keyboard.DOWN:
				case Keyboard.LEFT:
				case Keyboard.RIGHT:
				
				//md2.play(true, "stand");
				
				break;
				
				default:
				break;
			}
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if ( e.keyCode == 67 )
			{
				isTarget = ! isTarget;
			}
			
			if ( e.keyCode == Keyboard.UP )
			{
				//md2.play(true, "run");
				//md2.rotationY = -90;
				//md2.z += speed;
				center.z += speed;
				camera.z += speed;
			}
				
			if ( e.keyCode == Keyboard.DOWN )
			{
				//md2.play(true, "run");
				//md2.rotationY = 90;
				//md2.z -= speed;
				center.z -= speed;
				camera.z -= speed;
			}
			
			if ( e.keyCode == Keyboard.LEFT )
			{
				//md2.play(true, "run");
				//md2.rotationY = 180;
				//md2.x -= speed;
				center.x -= speed;
				camera.x -= speed;
			}
			
			if ( e.keyCode == Keyboard.RIGHT )
			{
				//md2.play(true, "run");
				//md2.rotationY = 0;
				//md2.x += speed;
				center.x += speed;
				camera.x += speed;
			}
				
			if ( e.keyCode == 70 )
			{
				if (_fogState)
				{
					//md2.mesh.setAttribute(Mesh3D.FOG_KEY, 0);
					terrain.mesh.setAttribute(Mesh3D.FOG_KEY, 0);
				}
				else
				{
					//md2.mesh.setAttribute(Mesh3D.FOG_KEY, 1);
					terrain.mesh.setAttribute(Mesh3D.FOG_KEY, 1);
				}
				
				_fogState = ! _fogState;
			}
			
			if ( e.keyCode == 84 )
			{
				if (_textureState)
				{
					//md2.mesh.setAttribute(Mesh3D.TEXTURE_KEY, 0);
					terrain.mesh.setAttribute(Mesh3D.TEXTURE_KEY, 0);
				}
				else
				{
					//md2.mesh.setAttribute(Mesh3D.TEXTURE_KEY, t0.pointer);
					terrain.mesh.setAttribute(Mesh3D.TEXTURE_KEY, t1.pointer);
				}
				
				_textureState = ! _textureState;
			}
			
			if ( e.keyCode == 76 )
			{
				if (_lightState)
				{
					//md2.mesh.lightEnable = false;
					//md2.mesh.setAttribute(Mesh3D.LIGHT_KEY, 0);
					terrain.mesh.setAttribute(Mesh3D.LIGHT_KEY, 0);
				}
				else
				{
					//md2.mesh.lightEnable = true;
					//md2.mesh.setAttribute(Mesh3D.LIGHT_KEY, 1);
					terrain.mesh.setAttribute(Mesh3D.LIGHT_KEY, 1);
				}
				
				_lightState = ! _lightState;
			}
			
			if ( e.keyCode == 82 )
			{
				_renderModeState ++;
				
				if (_renderModeState == 1)
				{
					//md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32);
				}
				else if (_renderModeState == 2)
				{
					//md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_WIREFRAME_TRIANGLE_32);
				}
				else if (_renderModeState == 3)
				{
					//md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32);
				}
				else if (_renderModeState == 4)
				{
					//md2.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
					terrain.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32);
				}
				
				_renderModeState = _renderModeState >= 4 ? 0 : _renderModeState;
			}
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var target:int = 3000 * dir + 100;
			TweenLite.to(lightObj, 3, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 3000 * dir;
			TweenLite.to(lightObj2, 4, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight3(dir:int = 1):void
		{
			var target:int = 200 * dir;
			TweenLite.to(lightObj3, 4, { y:target, onComplete:moveLight3, onCompleteParams:[dir * -1]});
		}
		
		private var k:Number;
		
		override protected function onRenderTick(e:Event = null):void
		{
			k = center.worldPosition.y;
			
			super.onRenderTick(e);
			
			k = center.worldPosition.y - k;
			
			if ( isTarget )
			{
				camera.target = center.worldPosition;
				
				var mx:Number = viewport.mouseX / 350;
				var my:Number = - (viewport.mouseY - 200 )/ 350;
				
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
		
		private var speed:Number = 20;
	}
}