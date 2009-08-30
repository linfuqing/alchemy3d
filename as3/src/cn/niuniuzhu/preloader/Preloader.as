package cn.niuniuzhu.preloader
{
	import cn.niuniuzhu.preloader.action.AbstractAction;
	import cn.niuniuzhu.preloader.action.Stable;
	import cn.niuniuzhu.preloader.action.Track;
	import cn.niuniuzhu.preloader.drawer.AbstractDrawer;
	import cn.niuniuzhu.preloader.drawer.CircleDrawer;
	import cn.niuniuzhu.preloader.drawer.RollDrawer;
	import cn.niuniuzhu.preloader.text.AbstractText;
	import cn.niuniuzhu.preloader.text.NormalText;
	import cn.niuniuzhu.preloader.text.TextType;
	
	import flash.display.Sprite;
	import flash.events.Event;
	
	public class Preloader extends Sprite
	{
		public static const PRELOAD_DISPOSED:String = "preload_disposed";
		
		private var _drawer:AbstractDrawer;
		private var _text:AbstractText;
		private var _action:AbstractAction;
		
		private var _insetFont:Class;
		
		private var showText:Boolean = false;
		
		public function set precent(value:Number):void
		{
			_drawer.precent = value;
			if (showText) _text.precent = value;
		}
		
		public function get precent():Number
		{
			return _drawer.precent;
		}
		
		public function get drawer():AbstractDrawer
		{
			return _drawer;
		}
		
		public function get text():AbstractText
		{
			return _text;
		}
		
		public function Preloader(viewType:String = "ring", textType:TextType = null, action:String = "stable")
		{
			switch (viewType)
			{
				case "ring":
					_drawer = new CircleDrawer(this);
					break;
				case "roll_circle":
					_drawer = new RollDrawer(this);
					break;
				default:
					throw new Error("Type error!");
					break;
			}
			
			if (textType)
			{
				showText = true;
				
				switch (textType.type)
				{
					case "normal":
						_text = new NormalText(this, textType.userInsetFont, textType.font, textType.size, textType.color, textType.center, textType.bold);
						break;
					default:
						throw new Error("Type error!");
						break;
				}
			}
			
			switch (action)
			{
				case "track":
					_action = new Track(this, 20);
					break;
				default:
					_action = new Stable(this);
					break;
			}
		}
		
		public function stop():void
		{
			_drawer.stop();
			if (_text) _text.stop();
			if (_action) _action.stop();
		}
		
		public function dispose():void
		{
			while(numChildren > 0)
			{
	       		removeChildAt(0);
	       	}
	       	
	       	dispatchEvent(new Event(PRELOAD_DISPOSED));
		}
	}
}