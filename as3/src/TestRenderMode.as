package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.Mesh3D;
	import cn.alchemy3d.objects.primitives.Cube;
	import cn.alchemy3d.objects.primitives.Sphere;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.scene.Scene3D;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.Viewport3D;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.geom.ColorTransform;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;
	
	import gs.TweenLite;
	import gs.easing.Back;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="40")]
	public class TestRenderMode extends Basic
	{
		protected var p:Mesh3D;
		protected var p2:Mesh3D;
		protected var center:Entity;
		
		protected var t1:Texture;
		protected var t2:Texture;
		protected var bl:BulkLoader;
		
		protected var light:Light3D;
		protected var light2:Light3D;
		protected var light3:Light3D;
		protected var lightObj:Sphere;
		protected var lightObj2:Sphere;
		protected var lightObj3:Sphere;
		
		protected var side:Mesh3D;

		public function TestRenderMode()
		{
			super();
			
//			stage.scaleMode = StageScaleMode.NO_SCALE;
//			stage.align = StageAlign.TOP_LEFT;
//			stage.quality = StageQuality.BEST;
//			stage.frameRate = 40;
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.addEventListener(BulkProgressEvent.PROGRESS, onProgress);
			bl.add("asset/wood01.jpg", {id:"0"});
			bl.add("asset/skin01.jpg", {id:"1"});
			bl.start();
			
			var tformat:TextFormat = new TextFormat("宋体", 14, 0xffffff, null, null, null, null, null, "center");
			tf4 = new TextField();
			tf4.defaultTextFormat = tformat;
			tf4.autoSize = "center";
			tf4.text = "Loading texture : 0.00%";
			tf4.selectable = false;
			tf4.x = 220;
			tf4.y = 240;
			addChild(tf4);
		}
		
		private function onProgress(e:BulkProgressEvent):void
		{
			tf4.text = "Loading texture : " + (e.percentLoaded * 100).toFixed(2) + "%";
		}
		
		private var tf4:TextField;
		private var tf3:TextField;
		private var tf5:TextField;
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("arial", 10, 0xffffff, null, null, null, null, null, "right");
			var tformat1:TextFormat = new TextFormat("宋体", 14, 0xffffff, null, null, null, null, null, "center");
			var tformat2:TextFormat = new TextFormat("宋体", 14, 0xffffff, true, null, null, null, null, "center");
			
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat;
			tf2.autoSize = "right";
			tf2.text = "Alchemy3D\nAuthor : 牛牛猪 / LinFuQing";
			tf2.selectable = false;
			tf2.x = 490;
			tf2.y = 430;
			addChild(tf2);
			
			tf3 = new TextField();
			tf3.defaultTextFormat = tformat1;
			tf3.autoSize = "center";
			tf3.selectable = false;
			tf3.x = 270;
			tf3.y = 35;
			addChild(tf3);
			
			tf5 = new TextField();
			tf5.defaultTextFormat = tformat2;
			tf5.autoSize = "center";
			tf5.text = "Alchemy3D 13种渲染模式";
			tf5.selectable = false;
			tf5.x = 180;
			tf5.y = 10;
			addChild(tf5);
		}
		
		protected function init(e:Event = null):void
		{
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			bl.removeEventListener(BulkProgressEvent.PROGRESS, onProgress);
			
			removeChild(tf4);
			tf4 = null;
			
			t1 = new Texture(bl.getBitmapData("0"));
			t2 = new Texture(bl.getBitmapData("1"));
			
			//============设置材质===============
			var m:Material = new Material(new ColorTransform(.1, .1, .1, 1), new ColorTransform(1, 1, 1, 1), new ColorTransform(.7, .7, .7, 1), null, 32);
			var m2:Material = new Material(new ColorTransform(.1, .1, .1, 1), new ColorTransform(.8, .8, .8, 1), new ColorTransform(1, 1, 1, 1), null, 16);
			
			var lightM:Material = new Material(new ColorTransform(1, 0, 0, 1));
			var lightM1:Material = new Material(new ColorTransform(0, 1, 0, 1));
			var lightM2:Material = new Material(new ColorTransform(0, 0, 1, 1));
			//============设置材质===============
			
			camera.z = -200;
			
			lightObj = new Sphere(lightM, null, 6, 3, 2)
			scene.addEntity(lightObj);
			lightObj.lightEnable = false;
			lightObj.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			
			lightObj2 = new Sphere(lightM1, null, 6, 3, 2)
			scene.addEntity(lightObj2);
			lightObj2.lightEnable = false;
			lightObj2.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			
			lightObj3 = new Sphere(lightM2, null, 6, 3, 2)
			scene.addEntity(lightObj3);
			lightObj3.lightEnable = false;
			lightObj3.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			
			p = new Cube(m, t1, 90, 90, 90, 4, 4, 4, 0, 0, "p");
			scene.addEntity(p);
			p.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			p.x = 90;
			p.z = 230;
			
			p2 = new Cube(m2, t2, 90, 90, 90, 4, 4, 4, 0, 0, "p");
			scene.addEntity(p2);
			p2.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			p2.x = -90;
			p2.z = 230;
			
			center = new Entity();
			scene.addEntity(center);
			center.x = 0;
			center.y = 0;
			center.z = 230;
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = 150;
			light.source.x = 0;
			light.source.z = 200;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 0, 0, 1);
			light.specular = new ColorTransform(1, 0, 0, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .00002;
			
			light2 = new Light3D(lightObj2);
			scene.addLight(light2);
			light2.type = LightType.POINT_LIGHT;
			light2.mode = LightType.HIGH_MODE;
			light2.bOnOff = LightType.LIGHT_ON;
			light2.source.y = 0;
			light2.source.x = -250;
			light2.source.z = 200;
			light2.ambient = new ColorTransform(0, 0, 0, 1);
			light2.diffuse = new ColorTransform(0, 1, 0, 1);
			light2.specular = new ColorTransform(0, 1, 0, 1);
			light2.attenuation1 = .001;
			light2.attenuation2 = .00002;
			
			light3 = new Light3D(lightObj3);
			scene.addLight(light3);
			light3.type = LightType.POINT_LIGHT;
			light3.mode = LightType.HIGH_MODE;
			light3.bOnOff = LightType.LIGHT_ON;
			light3.source.y = -150;
			light3.source.x = 150;
			light3.source.z = 200;
			light3.ambient = new ColorTransform(0, 0, 0, 1);
			light3.diffuse = new ColorTransform(0, 0, 1, 1);
			light3.specular = new ColorTransform(0, 0, 1, 1);
			light3.attenuation1 = .001;
			light3.attenuation2 = .000005;
			
			side = p;
			
			showInfo();
			
			startRendering();
//			addEventListener(MouseEvent.CLICK, onMouseClick);
//			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			
			callBack();
			
			changeRenderMode();
		}
		
		private function callBack():void
		{
			TweenLite.delayedCall(3, moveTarget);
		}
		
		private function moveTarget(x:Number = 0, y:Number = 0, z:Number = 0, angleX:Number = 0, angleY:Number = 0, angleZ:Number = 0):void
		{
			TweenLite.to(center, 1.5, {x:Math.random() * 280 - 140});
			TweenLite.to(camera, 1.5, {x:Math.random() * 900 - 450, y:Math.random() * 900 - 450, z:Math.random() * 500 - 300, onComplete:callBack, ease:Back.easeIn});
		}
		
		private var ri:uint = 0x000001;
		private function changeRenderMode():void
		{
			switch (ri)
			{
				case RenderMode.RENDER_WIREFRAME_TRIANGLE_32:
					tf3.text = "32位线框模式";
					p.lightEnable = false;
					p2.lightEnable = false;
					break;
					
				case RenderMode.RENDER_TEXTRUED_TRIANGLE_INVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / 线性插值纹理映射";
					p.lightEnable = false;
					p2.lightEnable = false;
					break;
					
				case RenderMode.RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / 双线性插值纹理";
					p.lightEnable = false;
					p2.lightEnable = false;
					break;
					
				case RenderMode.RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / Flat光照模型 / 线性插值纹理映射";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / Gouraud光照模型 / 线性插值纹理映射";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32:
					tf3.text = "32位模式 / 1/z深度缓冲 / Flat光照模型";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / Gouraud光照模型";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / 透视矫正纹理映射";
					p.lightEnable = false;
					p2.lightEnable = false;
					break;
					
				case RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / 线性分段透视矫正纹理映射";
					p.lightEnable = false;
					p2.lightEnable = false;
					break;
					
				case RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / 透视矫正纹理映射 / Flat光照模型";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32:
					tf3.text = "32位模式 / 支持1/z深度缓冲 / 线性分段透视矫正纹理映射 / Flat光照模型";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_FLAT_TRIANGLE_32:
					tf3.text = "32位Flat模式不带Z缓冲";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
					
				case RenderMode.RENDER_FLAT_TRIANGLEFP_32:
					tf3.text = "32位Flat定点模式不带Z缓冲";
					p.lightEnable = true;
					p2.lightEnable = true;
					break;
			}
			
			p.renderMode = p2.renderMode = ri;
			
			ri <<= 1;
			
			if (ri == 0x001000) ri = 0x000001;
			
			TweenLite.delayedCall(8, changeRenderMode);
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			p.rotationX ++;
			p.rotationY ++;
			p.rotationZ ++;
			p2.rotationX ++;
			p2.rotationY ++;
			p2.rotationZ ++;
			
			super.onRenderTick(e);
			
			camera.target = center.worldPosition;
//			var mx:Number = viewport.mouseX / 200;
//			var my:Number = - viewport.mouseY / 200;
//			
//			camera.hover(mx, my, 10);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if (e.keyCode == Keyboard.UP)
			{
				camera.z += 3;
			}
			
			if (e.keyCode == Keyboard.DOWN)
			{
				camera.z -= 3;
			}
		}
		
		protected function onMouseClick(e:MouseEvent):void
		{
//			p.rotationY += 10;
//			p2.rotationY += 10;
			if (side == p)
			{
//				side.renderMode = RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32;
				side = p2;
			}
			else
			{
				side = p;
			}
		}
	}
}