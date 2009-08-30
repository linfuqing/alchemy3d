package cn.niuniuzhu.preloader.drawer.sprites
{
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.filters.BlurFilter;
	import flash.filters.GlowFilter;

	public class LSprite extends Sprite
	{
		private const radius1:int = 2;
		private const radius2:int = 23;
		private const radius3:int = 3;
		private const radius4:int = 34;
		private var offset:int = 10;
		
		private var shape:Shape;
		private var shape2:Shape;
		private var step:int = 0;
		
		public function get offsetWidth():int
		{
			return radius3 + radius4 + offset;
		}
		
		public function LSprite()
		{
			super();
			
			addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		private function init(e:Event):void
		{
			removeEventListener(Event.ADDED_TO_STAGE, init);
			
			drawShape();
			
			addEventListener(Event.ENTER_FRAME, onEnterFrame);
		}
		
		private function drawShape():void
		{
			shape = new Shape();
			with (shape.graphics)
			{
				beginFill(0xFFFFFF, .4);
				drawCircle(radius4 + radius3 + offset, radius4 + radius3 + offset, radius1);
				endFill();
			}
			shape.filters = [new GlowFilter(0x89c97a, 1, 8, 8, 4, 1)];
			addChild(shape);
			
			shape2 = new Shape();
			with (shape2.graphics)
			{
				beginFill(0xFFFFFF, .4);
				drawCircle(radius4 + radius3 + offset, radius4 + radius3 + offset, radius3);
				endFill();
			}
			shape2.filters = [new GlowFilter(0x8fe7f4, 1, 8, 8, 4, 1)];
			addChild(shape2);
		}
		
		private function onEnterFrame(e:Event):void
		{
			shape.x = Math.cos(Math.PI * step / 180) * radius2;
			shape.y = Math.sin(Math.PI * step / 180) * radius2;
			
			shape2.x = - Math.cos(Math.PI * step / 180) * radius4;
			shape2.y = Math.sin(Math.PI * step / 180) * radius4;
			
			step = step >= 360 ? 0 : step + 4;
		}
	}
}