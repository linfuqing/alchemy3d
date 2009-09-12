package
{
	import cn.alchemy3d.external.A3DS;
	import cn.alchemy3d.tools.Basic;
	import cn.alchemy3d.tools.FPS;
	
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
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
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
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
			a3ds = new A3DS();
			a3ds.load("asset/scene.3ds");
			scene.addEntity(a3ds);
			
			a3ds.scale = 0.1;
			a3ds.z = 300;
			a3ds.x = 0;
			a3ds.y = 0;

			startRendering();
			
			showInfo();
		}
		
		override protected function onRenderTick(e:Event = null):void
		{
			a3ds.rotationY ++;
			
			super.onRenderTick(e);
			camera.z = 150;
			camera.y = 50;
			/*camera.target = a3ds.worldPosition;
			var mx:Number = viewport.mouseX / 500;
			var my:Number = - viewport.mouseY / 500;
			
			camera.hover(mx, my, 10);*/
		}
	}
}