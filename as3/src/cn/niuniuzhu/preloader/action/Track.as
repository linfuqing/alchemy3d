package cn.niuniuzhu.preloader.action
{
	import cn.niuniuzhu.preloader.Preloader;
	
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.ui.Mouse;
	
	public class Track extends AbstractAction
	{
		private var speedMove:int;
		
		public function Track(drawingBoard:Preloader, speed:int = 5)
		{
			super(drawingBoard);
			
			if (!_drawingBoard.text) throw new Error("TrackAction must setup a text!");
			
			_drawingBoard.addEventListener(Event.ADDED_TO_STAGE, onAdded);
			
			Mouse.hide();
			
			speedMove = speed;
		}
		
		private function distance(p1x:Number, p2x:Number, p1y:Number, p2y:Number):Number
		{
			return (Math.sqrt(Math.pow(p1x - p2x, 2) + Math.pow(p1y - p2y, 2)));
		}
		
		private function follow(from:*, target:*, distancemin:Number):void
		{
			var angle:Number = Math.atan2(from.y - target.y, from.x - target.x);
			var mydistance:Number = distance(from.x, target.x, from.y, target.y) - distancemin;
			
			if (mydistance >= 0)
			{
				from.x = from.x - Math.cos(angle) * (mydistance / speedMove);
				from.y = from.y - Math.sin(angle) * (mydistance / speedMove);
			}
			else
			{
				from.x = from.x - Math.cos(angle) * mydistance;
				from.y = from.y - Math.sin(angle) * mydistance;
			}
		}
		
		override protected function updateView(e:Event = null):void
		{
			_drawingBoard.drawer.drawer.x = _drawingBoard.drawer.drawer.stage.mouseX - _drawingBoard.x;
			_drawingBoard.drawer.drawer.y = _drawingBoard.drawer.drawer.stage.mouseY - _drawingBoard.y;
			
			follow(_drawingBoard.text.text, _drawingBoard.drawer.drawer, _drawingBoard.text.textWidth * .5 + _drawingBoard.drawer.drawerWidth * .5 + 10);
		}
		
		private function onAdded(e:Event):void
		{
			_drawingBoard.removeEventListener(Event.ADDED_TO_STAGE, onAdded);
			
			_drawingBoard.addEventListener(MouseEvent.CLICK, onMouseClick, false, 0, true);
		}
		
		private function onMouseClick(e:MouseEvent):void
		{
			Mouse.hide();
		}
		
		override public function stop():void
		{
			super.stop();
			
			Mouse.show();
		}
	}
}