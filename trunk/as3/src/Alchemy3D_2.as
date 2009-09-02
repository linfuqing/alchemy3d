package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.primitives.Plane;
	import cn.alchemy3d.objects.primitives.Sphere;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.geom.ColorTransform;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	import gs.TweenLite;

	[SWF(width="600",height="400",backgroundColor="#000000",frameRate="60")]
	public class Alchemy3D_2 extends Basic
	{
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		
		protected var p:Plane;
		
		protected var s:Sphere;
		protected var s2:Sphere;
		protected var s3:Sphere;
		
		protected var lightObj:Sphere;
		protected var lightObj2:Sphere;;
		protected var lightObj3:Sphere;
		
		protected var t1:Texture;
		protected var t2:Texture;
		protected var bl:BulkLoader;
		
		private var _lightMoving:Boolean = true;
		private var _usingTexture:Boolean = true;
		private var _lightingMode:int = 2;
		
		public function Alchemy3D_2()
		{
			super(600, 400);
			
//			stage.scaleMode = StageScaleMode.NO_SCALE;
//			stage.align = StageAlign.TOP_LEFT;
//			stage.quality = StageQuality.BEST;
//			stage.frameRate = 60;
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/earth.jpg", {id:"0"});
			bl.add("asset/lava01.jpg", {id:"1"});
			bl.start();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("arial", 10, 0xffffff);
			var tf:TextField = new TextField();
			tf.defaultTextFormat = tformat;
			tf.autoSize = "left";
			tf.text = "Press \"W\" to change to Wireframe mode\nPress \"M\" to change to Gouraud mode\nPress \"L\" to change lighting mode\nPress \"T\" for using texture\nPress \"R\" to turn on/off red light\nPress \"G\" to turn on/off green light\nPress \"B\" to turn on/off blue light\nPress \"S\" to begin/stop moving lights";
			tf.x = 0;
			tf.y = 290;
			addChild(tf);
			
			var tformat2:TextFormat = new TextFormat("arial", 12, 0xffffff, null, null, null, null, null, "right");
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat2;
			tf2.autoSize = "right";
			tf2.text = "Alchemy3D Lighting Demo\nAuthor:牛牛猪 / LinFuQing";
			tf2.selectable = false;
			tf2.x = 440;
			tf2.y = 363;
			addChild(tf2);
		}
		
		protected function init(e:Event = null):void
		{
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			
			t1 = new Texture(bl.getBitmapData("0"));
			t2 = new Texture(bl.getBitmapData("1"));
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(0.1, 0.1, 0.1, 1);
			m.diffuse = new ColorTransform(.8, .8, .8, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 32;
			
			var m1:Material = new Material();
			m1.ambient = new ColorTransform(0.2, 0.2, 0.2, 1);
			m1.diffuse = new ColorTransform(.8, .8, .8, 1);
			m1.specular = new ColorTransform(1, 1, 1, 1);
			m1.power = 4;
			
			var m2:Material = new Material();
			m2.ambient = new ColorTransform(0.2, 0.2, 0.2, 1);
			m2.diffuse = new ColorTransform(0.4, .4, .4, 1);
			m2.specular = new ColorTransform(1, 1, 1, 1);
			m2.power = 4;
			
			var m3:Material = new Material();
			m3.ambient = new ColorTransform(0, 0, 0, 1);
			m3.diffuse = new ColorTransform(.6, .6, .6, 1);
			m3.specular = new ColorTransform(1, 1, 1, 1);
			m3.power = 8;
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(0.5, 0, 0, 1);
			lightM.diffuse = new ColorTransform(1, 0, 0, 1);
			
			var lightM2:Material = new Material();
			lightM2.ambient = new ColorTransform(0, .5, 0, 1);
			lightM2.diffuse = new ColorTransform(0, 1, 0, 1);
			
			var lightM3:Material = new Material();
			lightM3.ambient = new ColorTransform(0, 0, 0.5, 1);
			lightM3.diffuse = new ColorTransform(0, 0, 1, 1);
			
			camera.z = -300;
			
			lightObj = new Sphere(lightM, null, 10, 3, 2, "lightO1");
			scene.addEntity(lightObj);
			lightObj.lightEnable = false;
			lightObj.renderMode = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32;
			
			lightObj2 = new Sphere(lightM2, null, 10, 3, 2, "lightO2");
			scene.addEntity(lightObj2);
			lightObj2.lightEnable = false;
			lightObj2.renderMode = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32;
			
			lightObj3 = new Sphere(lightM3, null, 10, 3, 2, "lightO3");
			scene.addEntity(lightObj3);
			lightObj3.lightEnable = false;
			lightObj3.renderMode = RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32;
			
			p = new Plane(m, null, 800, 800, 1, 1, "p");
			scene.addEntity(p);
			p.lightEnable = true;
			p.renderMode = RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32;
			p.rotationX = 90;
			p.y = -180;
			p.z = 800;
			
			s = new Sphere(m1, t1, 180, 16, 12, "s");
			scene.addEntity(s);
			s.lightEnable = true;
			s.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;
			s.z = 1100;

			s3 = new Sphere(m3, null, 120, 16, 12, "s3");
			scene.addEntity(s3);
			s3.lightEnable = true;
			s3.renderMode = RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32;
			s3.x = 150;
			s3.y = -60;
			s3.z = 700;
			
			s2 = new Sphere(m2, t2, 120, 16, 12, "s2");
			scene.addEntity(s2);
			s2.lightEnable = true;
			s2.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;
			s2.x = -130;
			s2.y = -60;
			s2.z = 750;
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = 120;
			light.source.x = 400;
			light.source.z = 0;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(.8, 0, 0, 1);
			light.specular = new ColorTransform(1, 0, 0, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .0000001;
			
			light2 = new Light3D(lightObj2);
			scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.HIGH_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.source.y = 350;
			light2.source.x = -900;
			light2.source.z = 300;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, .8, 0, 1);
			light2.specular = new ColorTransform(0, 1, 0, 1);
			light2.attenuation1 = .001;
			light2.attenuation2 = .0000001;
			
			light3 = new Light3D(lightObj3);
			scene.addLight(light3);
			light3.type = LightType.POINT_LIGHT;
			light3.mode = LightType.HIGH_MODE;
			light3.bOnOff = LightType.LIGHT_ON;
			light3.source.y = -500;
			light3.source.x = -400;
			light3.source.z = 800;
			light3.ambient = new ColorTransform(0, 0, 0, 1);
			light3.diffuse = new ColorTransform(0, 0, .8, 1);
			light3.specular = new ColorTransform(0, 0, 1, 1);
			light3.attenuation1 = .001;
			light3.attenuation2 = .0000001;
			
			showInfo();
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, lightOnOff);
			startRendering();
			
			moveLight1(1);
			moveLight2(1);
			moveLight3(1);
		}
		
		protected function lightOnOff(e:KeyboardEvent):void
		{
			if (e.keyCode == 82)
				light.bOnOff = !light.bOnOff;
			
			if (e.keyCode == 71)
				light2.bOnOff = !light2.bOnOff;
			
			if (e.keyCode == 66)
				light3.bOnOff = !light3.bOnOff;
			
			if (e.keyCode == 76)
			{
				_lightingMode ++ ;
				
				if (_lightingMode > 2) _lightingMode = 0;
				
				switch ( _lightingMode )
				{
					case 0:
						light.mode = LightType.LOW_MODE;
						light2.mode = LightType.LOW_MODE;
						light3.mode = LightType.LOW_MODE;
						break;
					case 1:
						light.mode = LightType.MID_MODE;
						light2.mode = LightType.MID_MODE;
						light3.mode = LightType.MID_MODE;
						break;
					case 2:
						light.mode = LightType.HIGH_MODE;
						light2.mode = LightType.HIGH_MODE;
						light3.mode = LightType.HIGH_MODE;
						break;
				}
			}
			
			if (e.keyCode == 84)
			{
				if (_usingTexture)
				{
					s.texture = null;
					s2.texture = null;
					_usingTexture = false;
				}
				else
				{
					s.texture = t2;
					s2.texture = t1;
					_usingTexture = true;
				}
			}
			
			if (e.keyCode == 83)
			{
				if (_lightMoving)
				{
					TweenLite.killTweensOf(lightObj);
					TweenLite.killTweensOf(lightObj2);
					TweenLite.killTweensOf(lightObj3);
					
					_lightMoving = false;
				}
				else
				{
					moveLight1(1);
					moveLight2(1);
					moveLight3(1);
					
					_lightMoving = true;
				}
			}
			
			if (e.keyCode == 87 )
			{
				p.lightEnable = false;
				p.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
				s.lightEnable = false;
				s.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
				s2.lightEnable = false;
				s2.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
				s3.lightEnable = false;
				s3.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			}
			
			if (e.keyCode == 77 )
			{
				p.lightEnable = true;
				p.renderMode = RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32;
				s.lightEnable = true;
				s.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;
				s2.lightEnable = true;
				s2.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;
				s3.lightEnable = true;
				s3.renderMode = RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32;
			}
		}
		
		protected function moveLight1(dir:int = 1):void
		{
			var target:int = 600 * dir + 1000;
			TweenLite.to(lightObj, 3, { z:target, onComplete:moveLight1, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight2(dir:int = 1):void
		{
			var target:int = 900 * dir;
			TweenLite.to(lightObj2, 3.5, { x:target, onComplete:moveLight2, onCompleteParams:[dir * -1]});
		}
		
		protected function moveLight3(dir:int = 1):void
		{
			var target:int = 500 * dir;
			TweenLite.to(lightObj3, 4, { y:target, onComplete:moveLight3, onCompleteParams:[dir * -1]});
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			s2.rotationY ++;
			
			super.onRenderTick(e);
			
			camera.target = s.worldPosition;
			var mx:Number = viewport.mouseX / 200;
			var my:Number = - viewport.mouseY / 200;
			
			camera.hover(mx, my, 10);
		}
	}
}