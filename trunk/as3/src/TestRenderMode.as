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
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.geom.ColorTransform;
	import flash.text.TextField;
	import flash.text.TextFormat;
	import flash.ui.Keyboard;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class TestRenderMode extends Basic
	{
		protected var viewport:Viewport3D;
		protected var camera:Camera3D;
		protected var scene:Scene3D;
		
		protected var p:Cube;
		protected var p2:Cube;
		protected var center:Entity;
		
		protected var t1:Texture;
		protected var t2:Texture;
		protected var bl:BulkLoader;
		
		protected var light:Light3D;
		protected var lightObj:Sphere;
		
		protected var side:Mesh3D;

		public function TestRenderMode()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/wood01.jpg", {id:"0"});
			bl.add("asset/lava01.jpg", {id:"1"});
			bl.start();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(scene, viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("arial", 10, 0xffffff, null, null, null, null, null, "right");
			var tf2:TextField = new TextField();
			tf2.defaultTextFormat = tformat;
			tf2.autoSize = "right";
			tf2.text = "Alchemy3D\nAuthor : 牛牛猪 / LinFuQing";
			tf2.selectable = false;
			tf2.x = 490;
			tf2.y = 430;
			addChild(tf2);
			
			var tformat1:TextFormat = new TextFormat("宋体", 12, 0xffffff, null, true, null, null, null, "center");
			var tf3:TextField = new TextField();
			tf3.defaultTextFormat = tformat1;
			tf3.autoSize = "center";
			tf3.text = "32位模式\n1/z深度缓冲\nGouraud光照模型混合纹理映射";
			tf3.selectable = false;
			tf3.x = 90;
			tf3.y = 365;
			addChild(tf3);
			
			var tf4:TextField = new TextField();
			tf4.defaultTextFormat = tformat1;
			tf4.autoSize = "center";
			tf4.text = "32位模式\n1/z深度缓冲\n双线性插值纹理映射";
			tf4.selectable = false;
			tf4.x = 420;
			tf4.y = 365;
			addChild(tf4);
			
			var tformat2:TextFormat = new TextFormat("宋体", 18, 0xffffff, true, null, null, null, null, "center");
			var tf5:TextField = new TextField();
			tf5.defaultTextFormat = tformat2;
			tf5.autoSize = "center";
			tf5.text = "up / down to move camera";
			tf5.selectable = false;
			tf5.x = 205;
			tf5.y = 420;
			addChild(tf5);
		}
		
		protected function init(e:Event = null):void
		{
			bl.removeEventListener(BulkProgressEvent.COMPLETE, init);
			
			t1 = new Texture(bl.getBitmapData("0"));
			t2 = new Texture(bl.getBitmapData("1"));
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.1, .1, .1, 1);
			m.diffuse = new ColorTransform(.8, .8, .8, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 128;
			
			scene = new Scene3D();
			addScene(scene);
			
			camera = new Camera3D(0, 90, 50, 5000);
			addCamera(camera);
			camera.eye.z = -100;
			
			viewport = new Viewport3D(640, 480, scene, camera);
			addViewport(viewport);
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(1, 1, 1, 1);
			lightM.diffuse = new ColorTransform(1, 1, 1, 1);
			lightObj = new Sphere(lightM, null, 10, 3, 2)
			scene.addEntity(lightObj);
			
			p = new Cube(m, t1, 90, 90, 90, 2, 2, 2, 0, 0, "p");
			scene.addEntity(p);
			p.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_FSINVZB_32;
			p.x = 90;
			p.z = 230;
			
			p2 = new Cube(m, t1, 90, 90, 90, 2, 2, 2, 0, 0, "p");
			scene.addEntity(p2);
			p2.renderMode = RenderMode.RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;
			p2.x = -90;
			p2.z = 230;
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.source.y = 400;
			light.source.x = -200;
			light.source.z = 0;
			light.ambient = new ColorTransform(0, 0, 0, 1);
			light.diffuse = new ColorTransform(1, 1, 1, 1);
			light.specular = new ColorTransform(1, 1, 1, 1);
			light.attenuation1 = .001;
			light.attenuation2 = .000001;
			
			side = p;
			
			showInfo();
			
			startRendering();
			addEventListener(MouseEvent.CLICK, onMouseClick);
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
//			camera.target = center.worldPosition;
//			var mx:Number = viewport.mouseX / 200;
//			var my:Number = - viewport.mouseY / 200;
//			
//			camera.hover(mx, my, 10);
			
			p.rotationX ++;
			p.rotationZ ++;
			p2.rotationX ++;
			p2.rotationZ ++;
			
			super.onRenderTick(e);

			trace(p.worldPosition);
		}
		
		protected function onKeyDown(e:KeyboardEvent):void
		{
			if (e.keyCode == Keyboard.UP)
			{
				camera.eye.z += 3;
			}
			
			if (e.keyCode == Keyboard.DOWN)
			{
				camera.eye.z -= 3;
			}
		}
		
		protected function onMouseClick(e:MouseEvent):void
		{
			if (side == p)
			{
				side.renderMode = RenderMode.RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32;
				side = p2;
			}
			else
			{
				side = p;
			}
		}
	}
}