package
{
	import cn.alchemy3d.objects.external.A3DS;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class Test3DS extends Basic
	{
		private var loader:URLLoader;
		
		protected var a3ds:A3DS;
		
		public function Test3DS()
		{
			super();
			
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
			//Start 3DS
			a3ds = new A3DS();
			a3ds.load("asset/3ds/EarthII.3ds");
			scene.addEntity(a3ds);
			
			a3ds.rotationX = -90;
			a3ds.scale = 2.5;
			a3ds.y = -50;
			a3ds.z = 100;

			startRendering();
			
			showInfo();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			a3ds.rotationY ++;
			
			super.onRenderTick(e);
		}
	}
}