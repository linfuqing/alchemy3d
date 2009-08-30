package cn.niuniuzhu.preloader.drawer
{
	import cn.niuniuzhu.preloader.Preloader;
	
	import flash.display.GradientType;
	import flash.display.Shape;
	import flash.display.SpreadMethod;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.filters.BevelFilter;
	import flash.filters.GlowFilter;
	import flash.geom.Matrix;
	
	public class CircleDrawer extends AbstractDrawer
	{
		private var _radius1:int = 8;
		private var _radius2:int = 13;
		
		public function CircleDrawer(drawingBoard:Preloader)
		{
			super(drawingBoard);
		}
		
		override protected function drawBackground():Sprite
		{
			var backgroundSprite:Sprite = new Sprite();
			var matr:Matrix = new Matrix();
			matr.createGradientBox(_radius2 * 2, _radius2 * 2, 0, 0, 0);

			with(backgroundSprite.graphics)
			{
				beginGradientFill(GradientType.RADIAL, [0x333333, 0x000000], [1, 1], [0, 255], matr, SpreadMethod.PAD);
				drawCircle(_radius2, _radius2, _radius1);
				drawCircle(_radius2, _radius2, _radius2);
				endFill();
			}
			backgroundSprite.filters = [new BevelFilter(2, 90, 0x666666, 1, 1, 1, 5, 5, 1.6, 1)];
			
			return backgroundSprite;
		}
		
		override protected function drawTrack():Sprite
		{
			var trackSprite:Sprite = new Sprite();
			
			var s1:Shape = drawTrackCircle();
			
			var s1Mask:Shape = drawMask();
			s1Mask.x = _radius2
			s1Mask.y = _radius2;
			s1.mask = s1Mask;
			trackSprite.addChild(s1Mask);
			trackSprite.addChild(s1);
			
			var s2:Shape = drawTrackCircle();
			s2.x = s2.y = _radius2 * 2;
			s2.rotation = -180;
			
			var s2Mask:Shape = drawMask();
			s2Mask.x = _radius2
			s2Mask.y = _radius2;
			s2Mask.rotation = 180;
			s2.mask = s2Mask;
			trackSprite.addChild(s2Mask);
			trackSprite.addChild(s2);
			
			trackSprite.filters = [new GlowFilter(0x0099FF, 1, 12, 12, 4, 1, false, false)];
			
			return trackSprite;
		}
		
		private function drawTrackCircle():Shape
		{
			var s:Shape = new Shape();
			with(s.graphics)
			{
				beginFill(0xFFFFFF, .55);
				moveTo(_radius2, _radius2 * 2);
				
				for (var i:int = 90; i < 270; i ++)
				{
					lineTo(Math.cos(Math.PI * i /180) * _radius2 + _radius2, Math.sin(Math.PI * i / 180) * _radius2 + _radius2);
				}
				lineTo(_radius2, _radius2 - _radius1);
				
				for (; i >= 90 ; i --)
				{
					lineTo(Math.cos(Math.PI * i / 180) * _radius1 + _radius2, Math.sin(Math.PI * i / 180) * _radius1 + _radius2);
				}
				lineTo(_radius2, _radius2 * 2);
				
				endFill();
			}
			
			return s;
		}
		
		override protected function drawMask():Shape
		{
			var maskShape:Shape = new Shape();
			var offset:int = 5;
			with(maskShape.graphics)
			{
				beginFill(0xFF0000, 1);
				drawRect(0, -_radius2 - offset, _radius2 + offset, (_radius2 + offset) * 2);
				endFill();
			}
			
			return maskShape;
		}
		
		override protected function updateView(e:Event = null):void
		{
			var mask1:Shape = Shape(_track.getChildAt(0));
			var mask2:Shape = Shape(_track.getChildAt(2));
			
			
			if (precent <= 50)
				mask1.rotation = 180 * (precent / 50);
			else
			{
				mask1.rotation = 180;
				mask2.rotation = 180 + 180 * ((precent - 50) / 50);
			}
		}
	}
}