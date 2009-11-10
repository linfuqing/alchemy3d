package
{
	import cn.niuniuzhu.preloader.Preloader;
	import cn.niuniuzhu.preloader.action.ActionType;
	import cn.niuniuzhu.preloader.drawer.DrawerType;
	import cn.niuniuzhu.preloader.text.TextType;
	
	import flash.display.GradientType;
	import flash.display.Loader;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageQuality;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.ProgressEvent;
	import flash.geom.Matrix;
	import flash.net.URLRequest;
	import flash.system.ApplicationDomain;
	import flash.system.LoaderContext;
	import flash.system.Security;

	[SWF(width="640",height="480",backgroundColor="#000000",frameRate="60")]
	public class SWFLoader extends Sprite
	{
		private var ldr:Loader;
		private var preloader:Preloader;
		[Embed(mimeType='application/x-font', source='asset/42______.TTF', fontName='myFont')]
		private var myFont:Class;
		
		public function SWFLoader()
		{
			super();
			
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			stage.quality = StageQuality.BEST;
			stage.frameRate = 60;
			
			with(this.graphics)
			{
				beginGradientFill(GradientType.RADIAL, [0x222222, 0x000000], [1, 1], [0, 255], new Matrix(.4, 0, 0, .3, 320, 240));
				drawRect(0, 0, 640, 480);
				endFill();
			}
			
			Security.allowDomain("*");

			var url:URLRequest = new URLRequest("Test8X.swf");
			
			var domain:ApplicationDomain = ApplicationDomain.currentDomain;
			var policyFile:Boolean = false;
			var context:LoaderContext = new LoaderContext(policyFile,domain);
			
			ldr = new Loader();
			ldr.load(url, context);

			ldr.contentLoaderInfo.addEventListener(Event.COMPLETE, onComplete);
			ldr.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, getProgress);
			
			addPreloader();
		}
		
		private function addPreloader():void
		{
			preloader = new Preloader(DrawerType.RING, new TextType(TextType.NORMAL, true, "myFont", 12, 0xcccccc), ActionType.TRACK);
			preloader.addEventListener(Preloader.PRELOAD_DISPOSED, onComplete);
			preloader.x = (stage.stageWidth - preloader.width) * .5;
			preloader.y = (stage.stageHeight - preloader.height) * .5;
			addChild(preloader);
		}

		private function getProgress(e:ProgressEvent):void
		{
			if (preloader.precent < 100)
			{
				preloader.precent += e.bytesLoaded / e.bytesTotal;
			}
		}

		private function onComplete(e:Event):void
		{
			ldr.contentLoaderInfo.removeEventListener(Event.COMPLETE, onComplete);
			ldr.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, getProgress);
			preloader.removeEventListener(Preloader.PRELOAD_DISPOSED, onComplete);
			
			preloader.stop();
			
			removeChild(preloader);
			
			addChild(ldr);
		}

	}
}