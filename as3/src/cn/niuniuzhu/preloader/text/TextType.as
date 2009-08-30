package cn.niuniuzhu.preloader.text
{
	public class TextType
	{
		public static const NORMAL:String = "normal";
		
		public var type:String;
		public var font:String;
		public var userInsetFont:Boolean;
		public var size:int;
		public var color:uint;
		public var bold:Boolean; 
		public var center:Boolean;
		
		public function TextType(type:String = NORMAL, userInsetFont:Boolean = false, font:String = "arial", size:int = 12, color:uint = 0xFFFFFF, center:Boolean = true, bold:Boolean = true)
		{
			this.type = type;
			this.userInsetFont = userInsetFont;
			this.font = font;
			this.size = size;
			this.color = color;
			this.bold = bold;
			this.center = center;
		}
	}
}