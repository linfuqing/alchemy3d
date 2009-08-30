package cn.niuniuzhu.preloader.drawer
{
	import cn.niuniuzhu.preloader.Preloader;
	
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.Event;
	
	public class AbstractDrawer implements IDrawer
	{
		protected var _drawingBoard:Preloader;
		protected var _background:Sprite;
		protected var _track:Sprite;
		protected var _mask:Shape;
		
		private var _drawer:Sprite;
		
		protected var _precent:Number = 0;

		public function set precent(value:Number):void
		{
			_precent = value;
		}
		
		public function get precent():Number
		{
			return _precent;
		}
		
		public function get drawer():Sprite
		{
			return _drawer;
		}
		
		public function get drawerWidth():Number
		{
			return _drawer.width;
		}
		
		public function get drawerHeight():Number
		{
			return _drawer.height;
		}
		
		public function AbstractDrawer(drawingBoard:Preloader)
		{
			_drawingBoard = drawingBoard;
			
			_background = drawBackground();
			_track = drawTrack();
			_mask = drawMask();
			
			_drawer = new Sprite();
			
			if (_background) _drawer.addChild(_background);
			if (_track) _drawer.addChild(_track);
			
			_drawingBoard.addChild(_drawer);
			
			_drawingBoard.addEventListener(Event.ENTER_FRAME, updateView);
		}
		
		protected function drawBackground():Sprite
		{
			return null;
		}
		
		protected function drawTrack():Sprite
		{
			return null;
		}
		
		protected function drawMask():Shape
		{
			return null;
		}
		
		protected function updateView(e:Event = null):void
		{
		}
		
		public function stop():void
		{
			_drawingBoard.removeEventListener(Event.ENTER_FRAME, updateView);
		}
	}
}