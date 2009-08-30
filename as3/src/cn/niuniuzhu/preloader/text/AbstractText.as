package cn.niuniuzhu.preloader.text
{
	import cn.niuniuzhu.preloader.Preloader;
	
	import flash.events.Event;
	import flash.text.AntiAliasType;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	
	public class AbstractText implements IText
	{
		protected var _drawingBoard:Preloader;
		
		protected var _text:TextField;
		protected var _tf:TextFormat;
		
		protected var _precent:Number = 0;
		protected var _userInsetFont:Boolean = false;

		public function set precent(value:Number):void
		{
			_precent = value;
		}
		
		public function get precent():Number
		{
			return _precent;
		}
		
		public function get text():TextField
		{
			return _text;
		}
		
		public function get format():TextFormat
		{
			return _tf;
		}
		
		public function get textWidth():Number
		{
			return _text.width;
		}
		
		public function get textHeight():Number
		{
			return _text.height;
		}
		
		public function AbstractText(drawingBoard:Preloader, textFormat:TextFormat = null)
		{
			_drawingBoard = drawingBoard;
			if (textFormat) _tf = textFormat;
			
			addText();
			
			_drawingBoard.addEventListener(Event.ENTER_FRAME, updateView);
		}
		
		protected function addText():void
		{
			_text = new TextField();
			if (_tf) _text.defaultTextFormat = _tf;
			_text.autoSize = TextFieldAutoSize.CENTER;
			_text.mouseEnabled = false;
			if (_userInsetFont)
			{
				_text.antiAliasType = AntiAliasType.ADVANCED;
				_text.sharpness = -400;
				_text.embedFonts = true;
			}
			_text.text = "000%";
			_drawingBoard.addChild(_text);
		}
		
		protected function updateView(e:Event = null):void
		{
			_text.text = (precent >> 0).toString() + "%";
		}
		
		public function stop():void
		{
			_drawingBoard.removeEventListener(Event.ENTER_FRAME, updateView);
		}
	}
}