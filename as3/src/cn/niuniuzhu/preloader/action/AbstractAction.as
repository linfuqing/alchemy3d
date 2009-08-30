package cn.niuniuzhu.preloader.action
{
	import cn.niuniuzhu.preloader.Preloader;
	
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.utils.Timer;
	
	public class AbstractAction implements IAction
	{
		protected var _drawingBoard:Preloader;
		
		public function AbstractAction(drawingBoard:Preloader)
		{
			_drawingBoard = drawingBoard;
			
			_drawingBoard.addEventListener(Event.ENTER_FRAME, updateView);
		}
		
		protected function updateView(e:Event = null):void
		{
			
		}
		
		public function stop():void
		{
			_drawingBoard.removeEventListener(Event.ENTER_FRAME, updateView);
			var timer:Timer = new Timer(20, 0);
			timer.addEventListener(TimerEvent.TIMER, onTimer);
			
			timer.start();
		}
		
		private function onTimer(e:TimerEvent):void
		{
			var timer:Timer = Timer(e.target);
			if (_drawingBoard.alpha > 0)
				_drawingBoard.alpha -= .1;
			else
			{
				timer.stop();
				timer.removeEventListener(TimerEvent.TIMER, onTimer);
				timer = null;
				
				_drawingBoard.dispose();
			}
		}
	}
}