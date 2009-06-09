package cn.alchemy3d.view
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.PixelSnapping;
	import flash.display.Sprite;

	public class Viewport3D extends Sprite
	{
		public var viewWidth:Number;
		public var viewHeight:Number;
		
		public var leftClip:Number;
		public var rightClip:Number;
		public var topClip:Number;
		public var bottomClip:Number;
		
		public var centerX:Number;
		public var centerY:Number;
		
		public var bitmapData:BitmapData;
		
		private var bitmap:Bitmap;
		
		public function Viewport3D(width:Number, height:Number)
		{
			super();
			
			this.viewWidth = width;
			this.viewHeight = height;
			
			this.centerX = width * 0.5;
			this.centerY = height * 0.5;
			
			this.leftClip = - centerX;
			this.rightClip = centerX;
			this.topClip = - centerY;
			this.bottomClip = centerY;
			
			this.bitmapData = new BitmapData(viewWidth, viewHeight, true);
			this.bitmap = new Bitmap(bitmapData, PixelSnapping.NEVER, false);
			addChild(bitmap);
		}
		
	}
}