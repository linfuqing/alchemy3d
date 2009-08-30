package cn.niuniuzhu.preloader.drawer
{
	import cn.niuniuzhu.preloader.Preloader;
	import cn.niuniuzhu.preloader.drawer.sprites.LSprite;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.filters.BlurFilter;
	import flash.geom.ColorTransform;
	import flash.geom.Point;

	public class RollDrawer extends AbstractDrawer
	{
		private var blur:BlurFilter;
		private var alphaTrans:ColorTransform;
		
		private var blurOut:BitmapData;
		
		private var p:Point = new Point();
		private var loopSprite:LSprite;
		
		public function RollDrawer(drawingBoard:Preloader)
		{
			super(drawingBoard);
			
            drawer.addChild(drawEffectLayer());
		}
		
		override protected function drawBackground():Sprite
		{
			loopSprite = new LSprite();
			loopSprite.alpha = 0;
			return loopSprite;
		}
		
		private function drawEffectLayer():Bitmap
		{
			blur = new BlurFilter(2, 2, 1);
			alphaTrans = new ColorTransform(.5, 1, 1, 1, 0, 0, 0, -3);
			
			var bitmapWidth:int = loopSprite.offsetWidth * 2;
			
			blurOut = new BitmapData(bitmapWidth, bitmapWidth, true, 0);
            var blurOutBmp:Bitmap = new Bitmap(blurOut);
			
			return blurOutBmp;
		}
		
		override protected function updateView(e:Event = null):void
		{
			if (blurOut)
			{
				blurOut.applyFilter(blurOut, blurOut.rect, p, blur);
				blurOut.draw(loopSprite);
				blurOut.colorTransform(blurOut.rect, alphaTrans);
			}
		}
	}
}