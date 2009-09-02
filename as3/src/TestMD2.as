package
{
	import cn.alchemy3d.objects.external.MD2;
	import cn.alchemy3d.view.Basic;
	import cn.alchemy3d.view.stats.FPS;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.text.TextField;
	import flash.text.TextFormat;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class TestMD2 extends Basic
	{
		private var loader:URLLoader;
		
		protected var md2:MD2;
		
		public function TestMD2()
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
			tf.text = "MD2加载器Demo";
			tf.x = 280;
			tf.y = 20;
			addChild(tf);
		}
		
		protected function init(e:Event = null):void
		{
			md2 = new MD2();
			md2.load("asset/tris.md2");
			scene.addEntity(md2);
			
			md2.rotationX = -90;
			md2.scale = 3.5;
			md2.z = 200;

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