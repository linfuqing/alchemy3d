package cn.rc3dx.view.stats
{
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.utils.getTimer;

	public class AbstractStatsView extends MovieClip
	{
		protected var _fps:int;
		protected var lastFrameTime:int;
		protected var currentFrameTime:int;
		
		public function AbstractStatsView()
		{
			super();
			setupListeners();
		}
		
		protected function setupListeners():void
		{
			addEventListener(Event.ENTER_FRAME, onFrame);
		}
		
		protected function onFrame(event:Event):void
		{
			currentFrameTime = getTimer();
			fps = 1000/(currentFrameTime - lastFrameTime);
			lastFrameTime = currentFrameTime;
		}
		
		public function set fps(fps:int):void
		{
			_fps = fps;	
		}
		
		public function get fps():int
		{
			return _fps;
		}
	}
}