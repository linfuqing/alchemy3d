package
{
	import cn.alchemy3d.container.Entity;
	import cn.alchemy3d.external.A3DS;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.lights.LightType;
	import cn.alchemy3d.render.RenderMode;
	import cn.alchemy3d.tools.Basic;
	import cn.alchemy3d.tools.FPS;
	
	import flash.events.Event;
	import flash.geom.ColorTransform;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;

	[SWF(width="400", height="400", backgroundColor="#000000",frameRate="60")]
	public class Test3DS extends Basic
	{
		private var loader:URLLoader;
		
		protected var a3ds:A3DS;
		protected var lightObj:Entity;
		protected var light:Light3D;
		
		public function Test3DS()
		{
			super(400, 400, 90, 20, 3000);
			
			init();
		}
		
		private function showInfo():void
		{
			var fps:FPS = new FPS(viewport);
			addChild(fps);
			
			var tformat:TextFormat = new TextFormat("宋体", 16, 0xffffff, true);
			var tf:TextField = new TextField();
			tf.defaultTextFormat = tformat;
			tf.autoSize = "left";
			tf.text = "3DS加载器Demo";
			tf.x = 280;
			tf.y = 20;
			addChild(tf);
		}
		
		protected function init(e:Event = null):void
		{
			a3ds = new A3DS(RenderMode.RENDER_TEXTRUED_TRIANGLE_INVZB_32);
			a3ds.load("asset/3ds/scene.jpg");
			viewport.scene.addChild(a3ds);
			
			a3ds.scale = 0.2;
			a3ds.z = 300;
			a3ds.x = 0;
			a3ds.y = 0;
			
			lightObj = new Entity();
			lightObj.y = 350;
			lightObj.x = 330;
			lightObj.z = 300;
			viewport.scene.addChild( lightObj );
			
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
			
			camera.z = 150;
			camera.y = 100;
			camera.rotationX = 35;

			startRendering();
			
			showInfo();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
//			a3ds.rotationY ++;
			
			super.onRenderTick(e);
			camera.target = a3ds.worldPosition;
			var mx:Number = viewport.mouseX / 500;
			var my:Number = - viewport.mouseY / 500;
			
			camera.hover(mx, my, 10);
		}
	}
}