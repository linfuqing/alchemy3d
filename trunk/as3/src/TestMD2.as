package
{
	import br.com.stimuli.loading.BulkLoader;
	import br.com.stimuli.loading.BulkProgressEvent;
	
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.external.MD2;
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

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class TestMD2 extends Basic
	{
		protected var bl:BulkLoader;
		
		private var loader:URLLoader;
		
		protected var md2:MD2;
		
		protected var t1:Texture;
		protected var light:Light3D;
		protected var lightObj:Sphere;
		
		public function TestMD2()
		{
			super();
			
			bl = new BulkLoader("main-site");
			bl.addEventListener(BulkProgressEvent.COMPLETE, init);
			bl.add("asset/blade_black.jpg", {id:"0"});
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
			
			var m:Material = new Material();
			m.ambient = new ColorTransform(.1, .1, .1, 1);
			m.diffuse = new ColorTransform(.7, .7, 0.7, 1);
			m.specular = new ColorTransform(1, 1, 1, 1);
			m.power = 48;
			
			var lightM:Material = new Material();
			lightM.ambient = new ColorTransform(1, 1, 1, 1);
			lightM.diffuse = new ColorTransform(1, 1, 1, 1);
			
			lightObj = new Sphere(lightM, null, 10, 3, 2)
			lightObj.y = -30;
			lightObj.x = 50;
			lightObj.z = 300;
			lightObj.renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
			scene.addEntity(lightObj);
			
			light = new Light3D(lightObj);
			scene.addLight(light);
			light.type = LightType.POINT_LIGHT;
			light.mode = LightType.HIGH_MODE;
			light.bOnOff = LightType.LIGHT_ON;
			light.ambient = new ColorTransform(0.4, 0.4, 0, 1);
			light.diffuse = new ColorTransform(.7, .7, 0, 1);
			light.specular = new ColorTransform(1, 1, 0, 1);
			light.attenuation1 = .002;
			light.attenuation2 = .00001;
			
			md2 = new MD2(m, t1);
			md2.load("asset/tris.md2");
			md2.lightEnable = true;
			scene.addEntity(md2);
			
			md2.rotationX = -90;
			md2.scale = 3;
			md2.y = -30;
			md2.z = 300;

			startRendering();
			
			showInfo();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			md2.rotationY ++;
			
			super.onRenderTick(e);
		}
	}
}